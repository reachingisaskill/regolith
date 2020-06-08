
#include "Regolith/Managers/ContextGroup.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Utilities/JsonValidation.h"
#include "Regolith/Contexts/LoadScreen.h"
#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/Architecture/Noisy.h"


namespace Regolith
{

  ContextGroup::ContextGroup() :
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
    _dataHandlers.create( "default" );
  }


  ContextGroup::~ContextGroup()
  {
    INFO_LOG( "Deleting Context Group" );
    this->unload();
  }


  void ContextGroup::configure( std::string filename )
  {
    _fileName = filename;
  }


  void ContextGroup::load()
  {
    // Load Json Data
    Json::Value json_data;
    Utilities::loadJsonData( json_data, _fileName );

    // Validate top-level objects
    Utilities::validateJson( json_data, "load_screen", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "data_handlers", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJson( json_data, "contexts", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJson( json_data, "entry_point", Utilities::JSON_TYPE_STRING );


    // Set the load screen pointer
    _loadScreen = dynamic_cast<LoadScreen*>( Manager::getInstance()->getContextManager().getGlobalContextGroup()->getContext( json_data["load_screen"].asString() ) );
    if ( _loadScreen == nullptr )
    {
      Exception ex( "ContextGroup::load()", "Could not find the requested LoadScreen. Is it the context type?" );
      ex.addDetail( "Context Name",  json_data["load_screen"].asString() );
      throw ex;
    }


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

      // Can load game objects from another file if the entry "file" is present
      if ( Utilities::validateJson( handler, "file", Utilities::JSON_TYPE_STRING, false ) )
      {
        Json::Value temp;
        Utilities::loadJsonData( temp, handler["file"].asString() );

        Utilities::validateJson( temp, "game_objects", Utilities::JSON_TYPE_ARRAY );
        objects = temp["game_objects"];
      }
      else
      {
        Utilities::validateJson( handler, "game_objects", Utilities::JSON_TYPE_ARRAY );
        objects = handler["game_objects"];
      }

      DataHandler& current_handler = _dataHandlers.get( handler_name );

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
        Utilities::loadJsonData( context_data, contexts[i]["file"].asString() );
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
    while ( ! _onLoadOperations.empty() )
    {
      _onLoadOperations.front().trigger( this );
      _onLoadOperations.pop();
    }
  }


  void ContextGroup::unload()
  {
    INFO_LOG( "Unloading Contexts" );
    for ( ProxyMap< Context* >::iterator it = _contexts.begin(); it != _contexts.end(); ++it )
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
        cg->setEntryPoint( cg->getContext( _key ) );
        break;

      default:
        break;
    };
  }

}

