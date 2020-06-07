
#include "Regolith/Managers/ContextGroup.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Utilities/JsonValidation.h"
#include "Regolith/Contexts/LoadScreen.h"


namespace Regolith
{

  ContextGroup::ContextGroup( IDNumber id, NamedVector< Context, true >& contexts ) :
    _theAudio(),
    _dataHandlers( "Data Handlers" ),
    _fileName(),
    _loadScreen( nullptr ),
    _contexts( "Context Map" ),
    _gameObjects( "Game Object Map" ),
    _spawnedObjects(),
    _onLoadOperations(),
    _entryPoint( nullptr )
  {
    _dataHandlers.set( "default", DataHandler );
  }


  ContextGroup::~ContextGroup()
  {
    INFO_LOG( "Deleting Context Group" );
    this->unload();

    INFO_LOG( "Deleting Operations" );
    for ( OperationQueue::iterator it = _onLoadOperations.begin(); it != _onLoadOperations.end(); ++it )
    {
      delete (*it);
    }
    _onLoadOperations.clear();
  }


  void ContextGroup::load()
  {
    // Load Json Data
    Json::Value json_data;
    loadJsonData( json_data, _fileName );

    // Validate top-level objects
    Utilities::validateJson( json_data, "load_screen", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "data_handlers", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJson( json_data, "contexts", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJson( json_data, "entry_point", Utilities::JSON_TYPE_STRING );


    // Set the load screen pointer
    _loadScreen = Manager::getInstance()->getContextManager().getGlobalContextGroup()->getContext( json_data["load_screen"].asString() );


    // Load objects
    ObjectFactory& obj_factory = Manager::getInstance()->getObjectFactory();

    Json::Value& data_handlers = json_data["data_handlers"];
    Json::ArrayIndex data_handlers_size = data_handlers.size();
    for( Json::ArrayIndex h_i = 0; h_i < data_handlers_size; ++h_i )
    {
      Json::Value& handler = data_handlers[h_i];

      Utilities::validateJson( handler, "name", Utilities::JSON_TYPE_ARRAY );
      std::string handler_name;

      INFO_STREAM << "Building data handler: " << handler_name;
      Json::Value objects;

      // Can load game objects from another file if the key is present
      if ( Utilities::validateJson( handler, "file", Utilities::JSON_TYPE_STRING, false ) )
      {
        Json::Value temp;
        loadJsonData( temp, handler["file"].asString() );

        Utilities::validateJson( temp, "game_objects", Utilities::JSON_TYPE_ARRAY );
        objects = temp["game_objects"];
      }
      else
      {
        Utilities::validateJson( handler, "game_objects", Utilities::JSON_TYPE_ARRAY );
        objects = handler["game_objects"];
      }

      DataHandler& current_handler = _dataHandlers.get( handler_name );

      Json::Value& objects = json_data["game_objects"];
      Json::ArrayIndex objects_size = objects.size();
      for( Json::ArrayIndex i = 0; i < objects_size; ++i )
      {
        Utilities::validateJson( objects[i], "name", Utilities::JSON_TYPE_STRING );
        std::string obj_name = objects[i]["name"].asString();

        GameObject* obj = obj_factory.build( objects[i], current_handler );
        _gameObjects.set( obj_name, obj );
      }
    }


    // Load contexts
    ContextFactory& cont_factory = Manager::getInstance()->getContextFactory();

    Json::Value& contexts = json_data["contexts"];
    Json::ArrayIndex contexts_size = contexts.size();
    for( Json::ArrayIndex i = 0; i < contexts_size; ++i )
    {
      Utilities::validateJson( contexts[i], "name", Utilities::JSON_TYPE_STRING );
      std::string cont_name = contexts[i]["name"].asString();

      Json::Value context_data;

      // Load the context data from another file if the key is present
      if ( Utilities::validateJson( contexts[i], "file", Utilities::JSON_TYPE_STRING, false ) )
      {
        loadJsonData( context_data, contexts[i]["file"].asString() );
      }
      else
      {
        context_data = contexts[i];
      }

      Context* cont = cont_factory.build( context_data, *this );
      _contexts.set( cont_name, cont );
    }


    // Set the default entry point
    _entryPoint = getContext( json_data["entry_point"].asString() );


    // Trigger all the signals cached before this context was loaded
    for ( OperationList::iterator it = _onLoadOperations.begin(); it != _onLoadOperations.end(); ++it )
    {
      (*it)->trigger();
    }
  }


  void ContextGroup::unload()
  {
    INFO_LOG( "Unloading Contexts" );
    for ( WrapperMap< Context* >::iterator it = _contexts.begin(); it != _contexts.end(); ++it )
    {
      delete it->second;
    }
    _contexts.clear();

    INFO_LOG( "Unloading Spawned Objects" );
    for ( SpawnedList::iterator it = _spawnedObjects.begin(); it != _spawnedObjects.end(); ++it )
    {
      delete (*it);
    }
    _spawnedObjects.clear();

    INFO_LOG( "Unloading Game Objects" );
    for ( WrapperMap< GameObject* >::iterator it = _gameObjects.begin(); it != _gameObjects.end(); ++it )
    {
      delete it->second;
    }
    _gameObjects.clear();
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Game Object Functions

  PhysicalObject* ContextGroup::spawn( std::string name, const Vector& pos )
  {
    PhysicalObject* new_obj = _gameObjects.get( name )->clone( pos );
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

  ContextGroup::Operation::Operation( Operation op, std::string key, std::string value ) :
    _operation( op ),
    _key( key ),
    _value( value )
  {
  }


  void ContextGroup::Operation::trigger( ContextGroup* cg )
  {
    switch( op )
    {
      case ACTION_SET_ENTRY_POINT :
        op->setEntryPoint( op->getContext( _key ) );
        break;

      default:
        break;
    };
  }

}

