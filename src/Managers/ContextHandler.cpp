
#include "Regolith/Managers/ContextHandler.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Utilities/JsonValidation.h"
#include "Regolith/Contexts/LoadScreen.h"


namespace Regolith
{

  ContextHandler::ContextHandler( IDNumber id, NamedVector< Context, true >& contexts ) :
    _id( id ),
    _loadScreen( nullptr ),
    _dataCache(),
    _contextCache(),
    _contexts( contexts )
  {
  }


  ContextHandler::~ContextHandler()
  {
    _dataCache.clear();
    _contextCache.clear();
  }


  void ContextHandler::configure( Json::Value& json_data )
  {
    ContextManager& manager = Manager::getInstance()->getContextManager();

    Utilities::validateJson( json_data, "contexts", Utilities::JSON_TYPE_ARRAY );
    Json::Value& contexts = json_data["contexts"];
    Json::ArrayIndex contexts_size = contexts.size();

    // Other setup info goes here

    if ( Utilities::validateJson( json_data, "load_screen", Utilities::JSON_TYPE_STRING, false ) )
    {
      _loadScreen = dynamic_cast<LoadScreen*>( manager.getContext( manager.requestContext( json_data["load_screen"].asString() ) ) );
      if ( _loadScreen == nullptr )
      {
        Exception ex( "ContextHandler::configure()", "Invalid load screen provided." );
        ex.addDetail( "Context Handler", json_data["name"].asString() );
        ex.addDetail( "Load Screen", json_data["load_screen"].asString() );
        throw ex;
      }
    }
    else
    {
      Exception ex( "ContextHandler::configure()", "No load screen is provided for context handler." );
      ex.addDetail( "Context Handler", json_data["name"].asString() );
      throw ex;
    }


    for ( Json::ArrayIndex i = 0; i < contexts_size; ++i )
    {
      buildContext( contexts[i] );
    }
  }


  void ContextHandler::configureDataHandler( DataHandler& handler, std::string name )
  {
    Manager::getInstance()->getDataManager().configureHandler( handler, name );
    _dataCache.insert( handler.getID() );
  }


  Context* ContextHandler::buildContext( Json::Value& json_data )
  {
    Utilities::validateJson( json_data, "name", Utilities::JSON_TYPE_STRING );
    std::string name = json_data["name"].asString();

    Context* obj = nullptr;

    if ( _contexts.exists( name ) && (_contexts.get( name ) != nullptr) )
    {
      INFO_STREAM << "Found existing context: " << name;
      obj = _contexts.get( name );

      _dataCache.insert( obj->dataHandler()->getID() );
      _contextCache.insert( _contexts.getID( name ) );
    }
    else
    {
      INFO_STREAM << "Building new context: " << name;
      Json::Value context_data = json_data;

      if ( Utilities::validateJson( json_data, "file", Utilities::JSON_TYPE_STRING, false ) )
      {
        std::string context_file = json_data["file"].asString();
        INFO_STREAM << "Loading context file: " << context_file;
        Utilities::loadJsonData( context_data, context_file );
      }

      obj = Manager::getInstance()->getContextFactory().build( context_data, *this );

      IDNumber id = _contexts.addObject( obj, name );
      _contextCache.insert( id );
    }

    return obj;
  }

}

