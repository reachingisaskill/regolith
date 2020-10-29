
#include "Regolith/Contexts/NullContext.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  NullContext::NullContext() :
    Context(),
    _testHandler( nullptr )
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
  }

  void NullContext::onStart()
  {
    if ( _testHandler != nullptr )
    {
      INFO_LOG( "NullContext::onStart : Reloading test data handler" );
      Manager::getInstance()->getDataManager().unload( _testHandler );
      Manager::getInstance()->getDataManager().load( _testHandler );
    }
  }

}

