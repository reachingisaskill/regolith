
#include "Regolith/Managers/ContextGroup.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Utilities/JsonValidation.h"
#include "Regolith/Contexts/LoadScreen.h"
#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/Architecture/Noisy.h"


namespace Regolith
{

  ContextGroup::ContextGroup() :
    _isGlobalGroup( false ),
    _theAudio(),
    _dataHandlers( "Data Handlers" ),
    _fileName(),
    _loadScreen(),
    _contexts( "Context Map" ),
    _gameObjects( "Game Object Map" ),
    _spawnedObjects(),
    _onLoadOperations(),
    _entryPoint(),
    _isLoaded( false )
  {
  }


  ContextGroup::~ContextGroup()
  {
    INFO_LOG( "ContextGroup::~ContextGroup : Destructing" );

    if ( _isLoaded ) this->unload();
    _contexts.clear();
  }


  void ContextGroup::configure( std::string filename, bool isGlobal )
  {
    _fileName = filename;
    _isGlobalGroup = isGlobal;

    // Load Json Data
    Json::Value json_data;
    Utilities::loadJsonData( json_data, _fileName );

    // Validate top-level objects
    Utilities::validateJson( json_data, "data_handlers", Utilities::JSON_TYPE_OBJECT );
    Utilities::validateJson( json_data, "contexts", Utilities::JSON_TYPE_OBJECT );

    // Global groups don't have a load-screen!
    if ( ! _isGlobalGroup )
    {
      Utilities::validateJson( json_data, "load_screen", Utilities::JSON_TYPE_STRING );
      _loadScreen = Manager::getInstance()->getContextManager().getGlobalContextGroup()->requestContext( json_data["load_screen"].asString() );
    }

    // Load a space for every context
    Json::Value& contexts = json_data["contexts"];
    for( Json::Value::iterator c_it = contexts.begin(); c_it != contexts.end(); ++c_it )
    {
      std::string cont_name = c_it.key().asString();
      _contexts.set( cont_name, nullptr );
    }


    // Set the default entry point. Optional if this is the global context group
    if ( Utilities::validateJson( json_data, "entry_point", Utilities::JSON_TYPE_STRING, (!_isGlobalGroup) ) )
    {
      _entryPoint = requestContext( json_data["entry_point"].asString() );
    }

  }


  void ContextGroup::load()
  {
    if ( _isLoaded ) 
    {
      WARN_LOG( "ContextGroup::load : Attempting to load a context group that is already loaded" );
      return;
    }

    DEBUG_LOG( "ContextGroup::load : Loading" );
    // Load Json Data
    Json::Value json_data;
    Utilities::loadJsonData( json_data, _fileName );

    // Load objects
    ObjectFactory& obj_factory = Manager::getInstance()->getObjectFactory();

    Json::Value& data_handlers = json_data["data_handlers"];
    for( Json::Value::iterator h_it = data_handlers.begin(); h_it != data_handlers.end(); ++h_it )
    {
      std::string handler_name = h_it.key().asString();

      INFO_STREAM << "ContextGroup::load : Building data handler: " << handler_name;
      Json::Value objects;

      // Can load game objects from another file if the entry "file" is present
      if ( h_it->isString() )
      {
        Json::Value temp;
        Utilities::loadJsonData( temp, h_it->asString() );

        Utilities::validateJson( temp, "game_objects", Utilities::JSON_TYPE_OBJECT );
        objects = temp["game_objects"];
      }
      else
      {
        objects = *h_it;
      }

      DataHandler* current_handler = _dataHandlers.set( handler_name, new DataHandler() );

      for( Json::Value::iterator o_it = objects.begin(); o_it != objects.end(); ++o_it )
      {
        std::string obj_name = o_it.key().asString();
        INFO_STREAM << "ContextGroup::load : Building game object: " << obj_name;

        try
        {
          GameObject* obj = obj_factory.build( *o_it, *this, *current_handler );
          _gameObjects.set( obj_name, obj );
        }
        catch ( Exception& ex )
        {
          ex.addDetail( "Object Name", obj_name );
          throw ex;
        }
      }
    }


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
        _contexts.set( cont_name, cont );
      }
      catch ( Exception& ex )
      {
        ex.addDetail( "Context Name", cont_name );
        throw ex;
      }
    }


    // Make sure the sounds each have their channels allocated and ready
    _theAudio.configure();


    // Trigger all the signals cached before this context was loaded
    while ( ! _onLoadOperations.empty() )
    {
      _onLoadOperations.front().trigger( this );
      _onLoadOperations.pop();
    }

    for ( ProxyMap< DataHandler* >::iterator it = _dataHandlers.begin(); it != _dataHandlers.end(); ++it )
    {
      DEBUG_STREAM << "ContextGroup::load : Loading Data Handler : " << it->first << " @ " << it->second;
      it->second->load();
    }

    _isLoaded = true;

    DEBUG_LOG( "ContextGroup::load : Complete" );
  }


  void ContextGroup::unload()
  {
    _isLoaded = false;
    INFO_LOG( "ContextGroup::unload : Unloading Data" );
    for ( ProxyMap< DataHandler* >::iterator it = _dataHandlers.begin(); it != _dataHandlers.end(); ++it )
    {
      delete it->second;
    }
    _dataHandlers.clear();

    INFO_LOG( "ContextGroup::unload : Unloading Contexts" );
    for ( ProxyMap< Context* >::iterator it = _contexts.begin(); it != _contexts.end(); ++it )
    {
      delete it->second;
      it->second = nullptr;
    }
//    _contexts.clear();

    INFO_LOG( "ContextGroup::unload : Unloading Spawned Objects" );
    for ( SpawnedList::iterator it = _spawnedObjects.begin(); it != _spawnedObjects.end(); ++it )
    {
      delete (*it);
    }
    _spawnedObjects.clear();

    INFO_LOG( "ContextGroup::unload : Unloading Game Objects" );
    for ( ProxyMap< GameObject* >::iterator it = _gameObjects.begin(); it != _gameObjects.end(); ++it )
    {
      delete it->second;
    }
    _gameObjects.clear();
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Game Object Functions

  PhysicalObject* ContextGroup::spawn( std::string name, const Vector& pos )
  {
    PhysicalObject* new_obj = getPhysicalObject( name )->clone( pos );
    if ( new_obj->hasAudio() )
    {
      dynamic_cast<Noisy*>( new_obj )->registerSounds( &_theAudio );
    }

    _spawnedObjects.push_back( new_obj );
    return new_obj;
  }


  PhysicalObject* ContextGroup::spawn( PhysicalObject* obj, const Vector& pos )
  {
    PhysicalObject* new_obj = obj->clone( pos );
    if ( new_obj->hasAudio() )
    {
      dynamic_cast<Noisy*>( new_obj )->registerSounds( &_theAudio );
    }

    _spawnedObjects.push_back( new_obj );
    return new_obj;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
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
        cg->setEntryPoint( cg->requestContext( _key ) );
        break;

      default:
        break;
    };
  }

}

