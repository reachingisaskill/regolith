
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
    _onLoadActions(),
    _entryPoint( nullptr )
  {
    _dataHandlers.set( "default", DataHandler );
  }


  ContextGroup::~ContextGroup()
  {
    INFO_LOG( "Deleting Context Group" );
    this->unload();

    INFO_LOG( "Deleting Actions" );
    for ( ActionQueue::iterator it = _onLoadActions.begin(); it != _onLoadActions.end(); ++it )
    {
      delete (*it);
    }
    _onLoadActions.clear();
  }


  void ContextGroup::load()
  {
    // Load Json Data
    Json::Value json_data;
    loadJsonData( json_data, _fileName );

    // Validate top-level objects
    Utilities::validateJson( json_data, "load_screen", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "game_objects", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJson( json_data, "contexts", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJson( json_data, "entry_point", Utilities::JSON_TYPE_STRING );


    // Set the load screen pointer
    _loadScreen = Manager::getInstance()->getContextManager().getGlobalContextGroup()->getContext( json_data["load_screen"].asString() );


    // Load objects
    ObjectFactory& obj_factory = Manager::getInstance()->getObjectFactory();

    Json::Value& objects = json_data["game_objects"];
    Json::ArrayIndex objects_size = objects.size();
    for( Json::ArrayIndex i = 0; i < objects_size; ++i )
    {
      Utilities::validateJson( objects[i], "name", Utilities::JSON_TYPE_STRING );
      std::string obj_name = objects[i]["name"].asString();

      DataHandler& data_handler = _dataHandlers.get( "default" );
      if( Utilities::validateJson( objects[i], "data_handler", Utilities::JSON_TYPE_STRING, false ) )
      {
        std::string handler_name = objects[i]["data_handler"].aString();
        if ( _dataHandlers.exists( handler_name ) )
          data_handler = _dataHandlers.get( handler_name );
        else
          data_handler = _dataHandlers.set( handler_name, DataHandler() );
      }

      GameObject* obj = obj_factory.build( objects[i], data_handler );
      _gameObjects.set( obj_name, obj );
    }


    // Load contexts
    ContextFactory& cont_factory = Manager::getInstance()->getContextFactory();

    Json::Value& contexts = json_data["contexts"];
    Json::ArrayIndex contexts_size = contexts.size();
    for( Json::ArrayIndex i = 0; i < contexts_size; ++i )
    {
      Utilities::validateJson( contexts[i], "name", Utilities::JSON_TYPE_STRING );
      std::string cont_name = contexts[i]["name"].asString();

      Context* cont = cont_factory.build( contexts[i], *this );
      _contexts.set( cont_name, cont );
    }


    // Set the default entry point
    _entryPoint = getContext( json_data["entry_point"].asString() );


    // Trigger all the signals cached before this context was loaded
    for ( ActionList::iterator it = _onLoadActions.begin(); it != _onLoadActions.end(); ++it )
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

}

