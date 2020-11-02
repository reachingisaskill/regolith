
#include "Regolith/Test/NullContext.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  NullContext::NullContext() :
    Context(),
    _testHandler( nullptr ),
    _timer( 0.0 )
  {
    INFO_LOG( "NullContext::NullContext : Null Context Created" );
  }


  NullContext::~NullContext()
  {
    INFO_LOG( "NullContext::~NullContext : Null Context Destroyed" );
  }


  void NullContext::configure( Json::Value& json_data, ContextGroup& group )
  {
    // Call the base class variant first
    Context::configure( json_data, group );

    // Perform some tests specfied in the "tests" sectino
    Utilities::validateJson( json_data, "tests", Utilities::JSON_TYPE_OBJECT );
    Json::Value& tests = json_data["tests"];

    // Find a data handler to unload, load and unload again.
    if ( Utilities::validateJson( tests, "data_handler_loading", Utilities::JSON_TYPE_STRING, false ) )
    {
      INFO_STREAM << "NullContext::configure : Testing the multithreaded loading using data handler: " << tests["data_handler_loading"].asString();
      _testHandler = group.getDataHandler( tests["data_handler_loading"].asString() );
    }

    // Test loading another context group
    if ( Utilities::validateJson( tests, "context_group_loading", Utilities::JSON_TYPE_OBJECT, false ) )
    {
      Json::Value& cg_loading_data = tests["context_group_loading"];
      Utilities::validateJson( cg_loading_data, "wait_for", Utilities::JSON_TYPE_INTEGER );
      Utilities::validateJson( cg_loading_data, "context_group", Utilities::JSON_TYPE_STRING );

      INFO_STREAM << "NullContext::configure : Testing the context group loading functionality, Context Group : " << cg_loading_data["context_group"].asString() << " after " << cg_loading_data["wait_for"].asFloat() << " seconds.";

      _cg_load_delay = cg_loading_data["wait_for"].asFloat();
      _cg_load = Manager::getInstance()->getContextManager().requestContextGroup( cg_loading_data["context_group"].asString() );
    }
  }

  void NullContext::onStart()
  {
    // Make sure this context stays open
    this->setClosed( false );

    _timer = 0.0;

    if ( _testHandler != nullptr )
    {
      INFO_LOG( "NullContext::onStart : Reloading test data handler" );
      Manager::getInstance()->getDataManager().unload( _testHandler );
      Manager::getInstance()->getDataManager().load( _testHandler );
    }
  }


  void NullContext::onStop()
  {
    // This context is now closed
    this->setClosed( true );
  }


  void NullContext::updateContext( float timestep )
  {
    _timer += timestep;

    if ( _cg_load )
    {
      if ( _timer > _cg_load_delay )
      {
        DEBUG_LOG( "NullContext::updateContext : Opening new context group" );
        Manager::getInstance()->getContextManager().setNextContextGroup( *_cg_load );
      }
    }
  }

}

