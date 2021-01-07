
#include "Regolith/Managers/ContextGroup.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Links/LinkContextManager.h"
#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/ObjectInterfaces/DrawableObject.h"
#include "Regolith/ObjectInterfaces/NoisyObject.h"
#include "Regolith/Contexts/Context.h"
#include "Regolith/Audio/Playlist.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  ContextGroup::ContextGroup() :
    _renderRate( 100 ),
    _isGlobalGroup( false ),
    _theAudio(),
    _theData(),
    _fileName(),
    _loadScreen( nullptr ),
    _contexts(),
    _gameObjects(),
    _spawnBuffers(),
//    _onLoadOperations(),
    _entryPoint( nullptr ),
    _defaultPlaylist( nullptr ),
    _isLoaded( false ),
    _loadingState( false ),
    _loadProgress( 0 ),
    _loadTotal( 0 ),
    _loadStatus( "" ),
    _renderPosition( _gameObjects.begin() ),
    _isRendered( false )
  {
  }


  ContextGroup::~ContextGroup()
  {
    INFO_LOG( "ContextGroup::~ContextGroup : Destructing" );

    if ( _isLoaded ) this->unload();
    _contexts.clear();
  }


  void ContextGroup::resetProgress()
  {
    GuardLock lg( _mutexProgress );
    _loadProgress = 0;
  }


  void ContextGroup::loadElement()
  {
    GuardLock lg( _mutexProgress );
    _loadProgress += 1;
  }


  void ContextGroup::setStatus( std::string status )
  {
    GuardLock lg( _mutexProgress );
    _loadStatus = status;
  }


  bool ContextGroup::isLoaded() const
  {
    GuardLock lg( _mutexProgress );
    return _isLoaded;
  }


  float ContextGroup::getLoadProgress() const
  {
    GuardLock lg( _mutexProgress );
    return (float) _loadProgress / _loadTotal;
  }


  std::string ContextGroup::getLoadStatus() const
  {
    GuardLock lg( _mutexProgress );
    return _loadStatus;
  }


  void ContextGroup::open()
  {
    // If a default playlist is mentioned, play it.
    if ( _defaultPlaylist != nullptr )
    {
      _defaultPlaylist->play();
    }
  }


  void ContextGroup::close()
  {
    // Stop all the music which may have originated from this CG
    Manager::getInstance()->clearQueue();
    Manager::getInstance()->stopTrack();

    DEBUG_LOG( "ContextGroup::close : HERE" );
  }


  void ContextGroup::configure( std::string filename, bool isGlobal )
  {
    INFO_STREAM << "ContextGroup::configure : Configuring with filename: " << filename;

    _fileName = filename;
    _isGlobalGroup = isGlobal;

    // Load Json Data
    Json::Value json_data;
    loadJsonData( json_data, _fileName );


    // Validate there is a slot for include files.
    validateJson( json_data, "include_files", JsonType::ARRAY );
    validateJsonArray( json_data["include_files"], 0, JsonType::STRING );

    // Load an entry for every game object in a seperate object list
    Json::Value& include_files = json_data["include_files"];
    for ( Json::ArrayIndex i = 0; i != include_files.size(); ++i )
    {
      std::string file_name = include_files[i].asString();
      Json::Value include_data;
      INFO_STREAM << "ContextGroup::configure : Loading include file : " << file_name;

      loadJsonData( include_data, file_name );

      // Configure data for all the included files
      this->_configureData( include_data );
    }

    // Configure data that is provided within the context group definition
    INFO_LOG( "ContextGroup::configure : Configuring remaining data" );
    this->_configureData( json_data );



    // If a default playlist is declared, configure it
    if ( validateJson( json_data, "default_playlist", JsonType::STRING, false ) )
    {
      std::string pl_name = json_data["default_playlist"].asString();
      _defaultPlaylist = _theAudio.getPlaylist( pl_name );
      INFO_STREAM << "ContextGroup::configure : Default playlist found : " << pl_name;
    }


    // Global groups don't have a load-screen, otherwise find a pointer to it
    if ( ! _isGlobalGroup )
    {
      validateJson( json_data, "load_screen", JsonType::STRING );
      _loadScreen = Manager::getInstance()->getContextManager<ContextGroup>().getGlobalContextGroup()->getContextPointer( json_data["load_screen"].asString() );
      INFO_STREAM << "ContextGroup::configure : Load Screen found : " << json_data["load_screen"].asString();
    }


    // Set the default entry point. Optional if this is the global context group
    if ( validateJson( json_data, "entry_point", JsonType::STRING, (!_isGlobalGroup) ) )
    {
      ContextMap::iterator found = _contexts.find( json_data["entry_point"].asString() );

      if ( found == _contexts.end() )
      {
        Exception ex( "ContextGroup::configure()", "Specified entry point is not found in context group" );
        throw ex;
      }
      _entryPoint = &found->second;
      INFO_STREAM << "ContextGroup::configure : Context Group entry point found : " << json_data["entry_point"].asString();
    }

    // Set the total number of elements to load (used for progress bars)
    _loadTotal = (2*_gameObjects.size()) + _spawnBuffers.size() + _contexts.size() + 1;

    INFO_LOG( "ContextGroup::configure : Configuration complete." );
  }


  void ContextGroup::load()
  {
    {
      GuardLock lg( _mutexProgress );
      if ( _isLoaded ) 
      {
        WARN_LOG( "ContextGroup::load : Attempting to load a context group that is already loaded" );
        return;
      }
      // Group is loading
      _loadingState = true;
    }

    setStatus( "" );
    resetProgress();

    DEBUG_LOG( "ContextGroup::load : Loading" );

    // Load Json Data
    Json::Value json_data;
    loadJsonData( json_data, _fileName );
    Json::Value& include_files = json_data["include_files"];


    DEBUG_LOG( "ContextGroup::load : Loading playlists" );
    for ( Json::ArrayIndex i = 0; i != include_files.size(); ++i )
    {
      std::string file_name = include_files[i].asString();
      Json::Value include_data;
      INFO_STREAM << "ContextGroup::configure : Loading include file : " << file_name;

      loadJsonData( include_data, file_name );
      this->_loadPlaylists( include_data["playlists"] );
    }
    // Load inline data
    this->_loadPlaylists( json_data["playlists"] );


    DEBUG_LOG( "ContextGroup::load : Loading the objects" );
    setStatus( "Building Game Objects" );
    for ( Json::ArrayIndex i = 0; i != include_files.size(); ++i )
    {
      std::string file_name = include_files[i].asString();
      Json::Value include_data;
      INFO_STREAM << "ContextGroup::configure : Loading include file : " << file_name;

      loadJsonData( include_data, file_name );
      this->_loadObjects( include_data["game_objects"] );
    }
    // Load inline data
    this->_loadObjects( json_data["game_objects"] );


    DEBUG_LOG( "ContextGroup::load : Filling the spawn buffers" );
    setStatus( "Filling Spawn Buffers" );
    for ( Json::ArrayIndex i = 0; i != include_files.size(); ++i )
    {
      std::string file_name = include_files[i].asString();
      Json::Value include_data;
      INFO_STREAM << "ContextGroup::configure : Loading include file : " << file_name;

      loadJsonData( include_data, file_name );
      this->_loadSpawnBuffers( include_data["spawn_buffers"] );
    }
    // Load inline data
    this->_loadSpawnBuffers( json_data["spawn_buffers"] );


    DEBUG_LOG( "ContextGroup::load : Loading the contexts" );
    setStatus( "Building Levels" );
    for ( Json::ArrayIndex i = 0; i != include_files.size(); ++i )
    {
      std::string file_name = include_files[i].asString();
      Json::Value include_data;
      INFO_STREAM << "ContextGroup::configure : Loading include file : " << file_name;

      loadJsonData( include_data, file_name );
      this->_loadContexts( include_data["contexts"] );
    }
    // Load inline data
    this->_loadContexts( json_data["contexts"] );


    // Tell the audio handler to initialise now that the objects have registered their sound effects
    DEBUG_LOG( "ContextGroup::load : Initialising audio handler" );
    _theAudio.initialise();


    // Wait for engine rendering process
    DEBUG_LOG( "ContextGroup::load : Waiting for engine rendering" );
    setStatus( "Pre-Rendering" );
    Manager::getInstance()->getContextManager<ContextGroup>().requestRenderContextGroup( this );


    setStatus( "Complete" );
    DEBUG_LOG( "ContextGroup::load : Complete" );
    {
      GuardLock lg( _mutexProgress );
      _isLoaded = true;
    }
  }


  void ContextGroup::unload()
  {
    {
      GuardLock lg( _mutexProgress );
      if ( !_isLoaded ) 
      {
        WARN_LOG( "ContextGroup::unload : Attempting to unload a context group that is not loaded" );
        return;
      }
      // Something is changing
      _isLoaded = false;
      // Group is unloading
      _loadingState = false;
    }

    setStatus( "" );
    resetProgress();

    _renderPosition = _gameObjects.begin();
    _isRendered = false;

    // Wait for the engine to clear all the textures
    Manager::getInstance()->getContextManager<ContextGroup>().requestRenderContextGroup( this );

    INFO_LOG( "ContextGroup::unload : Unloading Audio Configuration" );
    _theAudio.clear();

    INFO_LOG( "ContextGroup::unload : Unloading Data" );
    _theData.clear();

    INFO_LOG( "ContextGroup::unload : Unloading Contexts" );
    for ( ContextMap::iterator it = _contexts.begin(); it != _contexts.end(); ++it )
    {
      delete it->second;
      it->second = nullptr;
    }

    INFO_LOG( "ContextGroup::unload : Clearing spawn buffers" );
    for ( SpawnBufferMap::iterator it = _spawnBuffers.begin(); it != _spawnBuffers.end(); ++it )
    {
      it->second.clear();
    }

    INFO_LOG( "ContextGroup::unload : Unloading Game Objects" );
    for ( PhysicalObjectMap::iterator it = _gameObjects.begin(); it != _gameObjects.end(); ++it )
    {
      delete it->second;
      it->second = nullptr;
    }

  }


  bool ContextGroup::engineRenderLoadedObjects( Camera& camera )
  {
    {
      GuardLock lg( _mutexProgress );
      if ( _isLoaded )
      {
        // This should never happen!
        DEBUG_LOG( "ContextGroup::engineRenderLoadedObjects : Nothing to do." );
        return true;
      }
    }


    if ( _loadingState ) // Loading objects
    {
      DEBUG_LOG( "ContextGroup::engineRenderLoadedObjects : State = Loading" );
      for ( unsigned int i = 0; i < _renderRate; ++i )
      {
        if ( _renderPosition == _gameObjects.end() )
        {
          DEBUG_LOG( "ContextGroup::engineRenderLoadedObjects : Loading complete" );
          _isRendered = true;
          return true;
        }

        DEBUG_LOG( "ContextGroup::engineRenderLoadedObjects : Checking Object" );

        if ( _renderPosition->second->hasTexture() )
        {
          if ( dynamic_cast<DrawableObject*>( _renderPosition->second )->getTexture().update() )
          {
            DEBUG_LOG( "ContextGroup::engineRenderLoadedObjects : Rendering" );
            camera.renderTexture( dynamic_cast<DrawableObject*>( _renderPosition->second )->getTexture() );
          }
        }

        ++_renderPosition;
      }
    }
    else // Unloading objects
    {
      DEBUG_LOG( "ContextGroup::engineRenderLoadedObjects : State = Unloading" );
      for ( unsigned int i = 0; i < _renderRate; ++i )
      {
        if ( _renderPosition == _gameObjects.end() )
        {
          DEBUG_LOG( "ContextGroup::engineRenderLoadedObjects : Unloading complete" );
          _isRendered = true;
          return true;
        }

        if ( _renderPosition->second->hasTexture() )
        {
          if ( dynamic_cast<DrawableObject*>( _renderPosition->second )->getTexture().update() )
          {
            DEBUG_LOG( "ContextGroup::engineRenderLoadedObjects : Clearing" );
            camera.renderTexture( dynamic_cast<DrawableObject*>( _renderPosition->second )->getTexture() );
          }
        }

        ++_renderPosition;
      }
    }

    return false;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Accessors

//  Context* ContextGroup::getContext( std::string name )
//  {
//    ContextMap::iterator found = _contexts.find( name );
//
//    if ( found == _contexts.end() )
//    {
//      Exception ex( "ContextGroup::getContext()", "Could not find requested context." );
//      ex.addDetail( "Context Name", name );
//      throw ex;
//    }
//    else if ( found->second == nullptr )
//    {
//      Exception ex( "ContextGroup::getContext()", "Context is not constructed." );
//      ex.addDetail( "Context Name", name );
//      throw ex;
//    }
//
//    return found->second;
//  }


  Context** ContextGroup::getContextPointer( std::string name )
  {
    ContextMap::iterator found = _contexts.find( name );

    if ( found == _contexts.end() )
    {
      Exception ex( "ContextGroup::getContext()", "Could not find requested context." );
      ex.addDetail( "Context Name", name );
      throw ex;
    }

    return &found->second;
  }


  PhysicalObject* ContextGroup::getPhysicalObject( std::string name )
  {
    PhysicalObjectMap::iterator found = _gameObjects.find( name );

    if ( found == _gameObjects.end() )
    {
      Exception ex( "ContextGroup::getPhysicalObject()", "Could not find requested object." );
      ex.addDetail( "Object Name", name );
      throw ex;
    }

    return found->second;
  }


  PhysicalObject** ContextGroup::getPhysicalObjectPointer( std::string name )
  {
    PhysicalObjectMap::iterator found = _gameObjects.find( name );

    if ( found == _gameObjects.end() )
    {
      Exception ex( "ContextGroup::getPhysicalObject()", "Could not find requested object." );
      ex.addDetail( "Object Name", name );
      throw ex;
    }

    return &found->second;
  }


  PhysicalObject* ContextGroup::spawnPhysicalObject( std::string name )
  {
    SpawnBufferMap::iterator found = _spawnBuffers.find( name );

    if ( found == _spawnBuffers.end() )
    {
      Exception ex( "ContextGroup::spawnPhysicalObject()", "Could not find requested spawn buffer." );
      ex.addDetail( "Buffer Name", name );
      throw ex;
    }

    if ( found->second.spawnable() )
    {
      return found->second.pop();
    }
    else
    {
      return nullptr;
    }
  }


  Spawner ContextGroup::getSpawner( std::string name, ContextLayer* layer )
  {
    SpawnBufferMap::iterator found = _spawnBuffers.find( name );

    if ( found == _spawnBuffers.end() )
    {
      Exception ex( "ContextGroup::getSpawner()", "Could not find requested spawn buffer." );
      ex.addDetail( "Buffer Name", name );
      throw ex;
    }

    return found->second.requestSpawner( layer );
  }

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Helper functions for configuration and loading
  
  void ContextGroup::_configureData( Json::Value& json_data )
  {
    validateJson( json_data, "playlists", JsonType::OBJECT );
    validateJson( json_data, "game_objects", JsonType::OBJECT );
    validateJson( json_data, "spawn_buffers", JsonType::OBJECT );
    validateJson( json_data, "contexts", JsonType::OBJECT );


    // Configure an entry for the playlists in the audio handler
    DEBUG_LOG( "ContextGroup::configure : Configuring audio handler" );
    _theAudio.configure( json_data["playlists"], _theData );


    // Load an entry for every specific game object
    INFO_LOG( "ContextGroup::configure : Configuring game objects." );
    Json::Value& game_objects = json_data["game_objects"];
    for ( Json::Value::iterator o_it = game_objects.begin(); o_it != game_objects.end(); ++o_it )
    {
      std::string obj_name = o_it.key().asString();

      // Make sure there are no duplicates
      if ( _gameObjects.find( obj_name ) != _gameObjects.end() )
      {
        Exception ex( "ContextGroup::configure()", "Two objects provided with the same name. Duplicates are forbidden." );
        ex.addDetail( "Object Name", obj_name );
        throw ex;
      }
      _gameObjects[ obj_name ] = nullptr;
    }


    // Load an entry for every spawn buffer
    INFO_LOG( "ContextGroup::configure : Configuring spawn buffers." );
    Json::Value& spawn_buffers = json_data["spawn_buffers"];
    for ( Json::Value::iterator b_it = spawn_buffers.begin(); b_it != spawn_buffers.end(); ++b_it )
    {
      std::string buffer_name = b_it.key().asString();

      // Make sure there are no duplicates
      if ( _spawnBuffers.find( buffer_name ) != _spawnBuffers.end() )
      {
        Exception ex( "ContextGroup::configure()", "Two spawn buffers requested for the same object. Duplicates are forbidden." );
        ex.addDetail( "Object Name", buffer_name );
        throw ex;
      }
      // Make sure the requested object exists
      if ( _gameObjects.find( buffer_name ) == _gameObjects.end() )
      {
        Exception ex( "ContextGroup::configure()", "Spawn buffer requested for an object that has not been specified." );
        ex.addDetail( "Object Name", buffer_name );
        throw ex;
      }
      if ( ! b_it->isInt() )
      {
        Exception ex( "ContextGroup::configure()", "Spawn buffers must have an integer value specified, greater than 1." );
        ex.addDetail( "Object Name", buffer_name );
        throw ex;
      }

      _spawnBuffers[ buffer_name ] = SpawnBuffer();
    }


    // Load an entry for every context
    INFO_LOG( "ContextGroup::configure : Configuring contexts." );
    Json::Value& contexts = json_data["contexts"];
    for ( Json::Value::iterator c_it = contexts.begin(); c_it != contexts.end(); ++c_it )
    {
      std::string cont_name = c_it.key().asString();

      // Make sure there are no duplicates
      if ( _contexts.find( cont_name ) != _contexts.end() )
      {
        Exception ex( "ContextGroup::configure()", "Two contexts provided with the same name. Duplicates are forbidden." );
        ex.addDetail( "Context Name", cont_name );
        throw ex;
      }
      _contexts[ cont_name ] = nullptr;
    }

  }


  void ContextGroup::_loadPlaylists( Json::Value& json_data )
  {
    _theAudio.load( json_data, _theData );
    loadElement();
  }


  void ContextGroup::_loadObjects( Json::Value& object_data )
  {
    // Load objects
    ObjectFactory& obj_factory = Manager::getInstance()->getObjectFactory();

    for( Json::Value::iterator o_it = object_data.begin(); o_it != object_data.end(); ++o_it )
    {
      std::string obj_name = o_it.key().asString();
      INFO_STREAM << "ContextGroup::load : Building game object: " << obj_name;
      Json::Value object_data;

      // If object details are in a separate file, load them
      if ( o_it->isString() )
      {
        loadJsonData( object_data, o_it->asString() );
      }
      else
      {
        object_data = *o_it;
      }

      try
      {
        GameObject* obj = obj_factory.build( object_data, *this );
        PhysicalObject* phys_obj = dynamic_cast< PhysicalObject* >( obj );

        if ( phys_obj == nullptr )
        {
          Exception ex( "ContextGroup::load()", "Created an object that is not physical. Cannot add to context group." );
          throw ex;
        }

        if ( phys_obj->hasAudio() )
        {
          dynamic_cast< NoisyObject* >( phys_obj )->registerSounds( &_theAudio );
        }

        _gameObjects[ obj_name ] = phys_obj;;
      }
      catch ( Exception& ex )
      {
        ex.addDetail( "Object Name", obj_name );
        throw ex;
      }

      loadElement();
    }
    _renderPosition = _gameObjects.begin();

  }


  void ContextGroup::_loadSpawnBuffers( Json::Value& spawn_buffers )
  {
    // Fill spawn buffers
    for( Json::Value::iterator b_it = spawn_buffers.begin(); b_it != spawn_buffers.end(); ++b_it )
    {
      std::string buffer_name = b_it.key().asString();
      unsigned int number = b_it->asInt();

      _spawnBuffers[ buffer_name ].fill( number, _gameObjects[ buffer_name ], &_theAudio );

      loadElement();
    }

  }


  void ContextGroup::_loadContexts( Json::Value& contexts )
  {
    // Load contexts
    ContextFactory& cont_factory = Manager::getInstance()->getContextFactory();

    for( Json::Value::iterator c_it = contexts.begin(); c_it != contexts.end(); ++c_it )
    {
      std::string cont_name = c_it.key().asString();

      INFO_STREAM << "ContextGroup::load : Building context: " << cont_name;
      Json::Value context_data;

      // Load the context data from another file if a string is provided
      if ( c_it->isString() )
      {
        loadJsonData( context_data, c_it->asString() );
      }
      else
      {
        context_data = *c_it;
      }

      try
      {
        Context* cont = cont_factory.build( context_data, *this );
        _contexts[ cont_name ] = cont;
        DEBUG_STREAM << "ContextGroup::load : Context : " << cont_name << " built @ " << cont;
      }
      catch ( Exception& ex )
      {
        ex.addDetail( "Context Name", cont_name );
        throw ex;
      }

      loadElement();
    }

  }

}

