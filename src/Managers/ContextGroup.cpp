
#include "Regolith/Managers/ContextGroup.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/ObjectInterfaces/DrawableObject.h"
#include "Regolith/ObjectInterfaces/NoisyObject.h"
#include "Regolith/Contexts/Context.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  ContextGroup::ContextGroup() :
    _renderRate( 10 ),
    _isGlobalGroup( false ),
    _theAudio( Manager::getInstance()->getAudioManager() ),
    _theData(),
    _fileName(),
    _loadScreen( nullptr ),
    _contexts(),
    _gameObjects(),
    _spawnBuffers(),
//    _onLoadOperations(),
    _entryPoint( nullptr ),
    _isLoaded( false ),
    _loadingState( false ),
    _loadProgress( 0 ),
    _loadTotal( 0 ),
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


  void ContextGroup::configure( std::string filename, bool isGlobal )
  {
    INFO_STREAM << "ContextGroup::configure : Configuring with filename: " << filename;

    _fileName = filename;
    _isGlobalGroup = isGlobal;

    // Load Json Data
    Json::Value json_data;
    Utilities::loadJsonData( json_data, _fileName );

    // Validate top-level objects
    Utilities::validateJson( json_data, "game_objects", Utilities::JSON_TYPE_OBJECT );
    Utilities::validateJson( json_data, "spawn_buffers", Utilities::JSON_TYPE_OBJECT );
    Utilities::validateJson( json_data, "contexts", Utilities::JSON_TYPE_OBJECT );


    // Global groups don't have a load-screen
    if ( ! _isGlobalGroup )
    {
      Utilities::validateJson( json_data, "load_screen", Utilities::JSON_TYPE_STRING );
      _loadScreen = Manager::getInstance()->getContextManager().getGlobalContextGroup()->getContextPointer( json_data["load_screen"].asString() );
    }


    // Load an entry for every game object
    Json::Value& game_objects = json_data["game_objects"];
    for( Json::Value::iterator o_it = game_objects.begin(); o_it != game_objects.end(); ++o_it )
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
    Json::Value& spawn_buffers = json_data["spawn_buffers"];
    for( Json::Value::iterator b_it = spawn_buffers.begin(); b_it != spawn_buffers.end(); ++b_it )
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
    Json::Value& contexts = json_data["contexts"];
    for( Json::Value::iterator c_it = contexts.begin(); c_it != contexts.end(); ++c_it )
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


    // Set the default entry point. Optional if this is the global context group
    if ( Utilities::validateJson( json_data, "entry_point", Utilities::JSON_TYPE_STRING, (!_isGlobalGroup) ) )
    {
      ContextMap::iterator found = _contexts.find( json_data["entry_point"].asString() );

      if ( found == _contexts.end() )
      {
        Exception ex( "ContextGroup::configure()", "Specified entry point is not found in context group" );
        throw ex;
      }
      _entryPoint = &found->second;
    }

    // Set the total number of elements to load (used for progress bars)
    _loadTotal = (2*_gameObjects.size()) + _spawnBuffers.size() + _contexts.size() + 1;
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

    resetProgress();

    DEBUG_LOG( "ContextGroup::load : Loading" );
    // Load Json Data
    Json::Value json_data;
    Utilities::loadJsonData( json_data, _fileName );


    DEBUG_LOG( "ContextGroup::load : Loading the objects" );
    // Load objects
    ObjectFactory& obj_factory = Manager::getInstance()->getObjectFactory();

    Json::Value& object_data = json_data["game_objects"];
    for( Json::Value::iterator o_it = object_data.begin(); o_it != object_data.end(); ++o_it )
    {
      std::string obj_name = o_it.key().asString();
      INFO_STREAM << "ContextGroup::load : Building game object: " << obj_name;
      Json::Value object_data;

      // If object details are in a separate file, load them
      if ( o_it->isString() )
      {
        Utilities::loadJsonData( object_data, o_it->asString() );
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


    DEBUG_LOG( "ContextGroup::load : Filling the spawn buffers" );
    // Fill spawn buffers
    Json::Value& spawn_buffers = json_data["spawn_buffers"];
    for( Json::Value::iterator b_it = spawn_buffers.begin(); b_it != spawn_buffers.end(); ++b_it )
    {
      std::string buffer_name = b_it.key().asString();
      unsigned int number = b_it->asInt();

      _spawnBuffers[ buffer_name ].fill( number, _gameObjects[ buffer_name ], &_theAudio );

      loadElement();
    }


    DEBUG_LOG( "ContextGroup::load : Loading the contexts" );
    // Load contexts
    ContextFactory& cont_factory = Manager::getInstance()->getContextFactory();

    Json::Value& contexts = json_data["contexts"];
    for( Json::Value::iterator c_it = contexts.begin(); c_it != contexts.end(); ++c_it )
    {
      std::string cont_name = c_it.key().asString();

      INFO_STREAM << "ContextGroup::load : Building context: " << cont_name;
      Json::Value context_data;

      // Load the context data from another file if a string is provided
      if ( c_it->isString() )
      {
        Utilities::loadJsonData( context_data, c_it->asString() );
      }
      else
      {
        context_data = *c_it;
      }

      try
      {
        Context* cont = cont_factory.build( context_data, *this );
        _contexts[ cont_name ] = cont;
      }
      catch ( Exception& ex )
      {
        ex.addDetail( "Context Name", cont_name );
        throw ex;
      }

      loadElement();
    }


    DEBUG_LOG( "ContextGroup::load : Configuring audio handler" );
    _theAudio.configure();
    loadElement();


//    DEBUG_LOG( "ContextGroup::load : Performing on-load operations" );
//    // Trigger all the signals cached before this context was loaded
//    while ( ! _onLoadOperations.empty() )
//    {
//      _onLoadOperations.front().trigger( this );
//      _onLoadOperations.pop();
//    }


    // Wait for engine rendering process
    Manager::getInstance()->getContextManager().requestRenderContextGroup( this );


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

    resetProgress();


    _renderPosition = _gameObjects.begin();
    _isRendered = false;


    // Wait for the engine to clear all the textures
    Manager::getInstance()->getContextManager().requestRenderContextGroup( this );


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
        if ( ++_renderPosition == _gameObjects.end() )
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
      }
    }
    else // Unloading objects
    {
      DEBUG_LOG( "ContextGroup::engineRenderLoadedObjects : State = Unloading" );
      for ( unsigned int i = 0; i < _renderRate; ++i )
      {
        if ( ++_renderPosition == _gameObjects.end() )
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
      }
    }

    return false;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Accessors

  Context* ContextGroup::getContext( std::string name )
  {
    ContextMap::iterator found = _contexts.find( name );

    if ( found == _contexts.end() )
    {
      Exception ex( "ContextGroup::getContext()", "Could not find requested context." );
      ex.addDetail( "Context Name", name );
      throw ex;
    }

    return found->second;
  }


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




/*
  // Context Group Operations

  ContextGroup::Operation::Operation( OperationType op, std::string key, std::string value ) :
    _operation( op ),
    _key( key ),
    _value( value )
  {
  }


  void ContextGroup::Operation::trigger( ContextGroup* cg )
  {
    switch( _operation )
    {
      case ACTION_SET_ENTRY_POINT :
        cg->setEntryPoint( cg->getContextPointer( _key ) );
        break;

      default:
        break;
    };
  }
*/

}

