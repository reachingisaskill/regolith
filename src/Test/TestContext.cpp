
#include "Regolith/Test/TestContext.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  TestContext::TestContext() :
    Context(),
    _cg_load( nullptr ),
    _timer(),
    _isLoadScreen( false )
  {
    INFO_LOG( "TestContext::TestContext : Null Context Created" );
  }


  TestContext::~TestContext()
  {
    INFO_LOG( "TestContext::~TestContext : Null Context Destroyed" );
  }


  void TestContext::configure( Json::Value& json_data, ContextGroup& group )
  {
    // Call the base class variant first
    Context::configure( json_data, group );

    // Perform some tests specfied in the "tests" sectino
    Utilities::validateJson( json_data, "tests", Utilities::JSON_TYPE_OBJECT );
    Json::Value& tests = json_data["tests"];

    // Load the name of this context
    Utilities::validateJson( tests, "name", Utilities::JSON_TYPE_STRING );
    _name = tests["name"].asString();

    // Test loading another context group
    if ( Utilities::validateJson( tests, "context_group_loading", Utilities::JSON_TYPE_OBJECT, false ) )
    {
      Json::Value& cg_loading_data = tests["context_group_loading"];
      Utilities::validateJson( cg_loading_data, "wait_for", Utilities::JSON_TYPE_INTEGER );
      Utilities::validateJson( cg_loading_data, "context_group", Utilities::JSON_TYPE_STRING );

      INFO_STREAM << "TestContext::configure : Testing the context group loading functionality, Context Group : " << cg_loading_data["context_group"].asString() << " after " << cg_loading_data["wait_for"].asFloat() << " seconds.";

      _timer.configure( cg_loading_data["wait_for"].asFloat(), 1 );
      _cg_load = Manager::getInstance()->getContextManager().getContextGroup( cg_loading_data["context_group"].asString() );
    }

    // If this iteration is acting as a load_screen
    if ( Utilities::validateJson( tests, "load_screen", Utilities::JSON_TYPE_BOOLEAN, false ) )
    {
      _isLoadScreen = tests["load_screen"].asBool();
    }
  }


  void TestContext::onStart()
  {
    // Make sure this context stays open
    this->setClosed( false );

    INFO_STREAM << "TestContext::onStart " << _name << " : Starting test context.";

    _timer.reset();
  }


  void TestContext::onStop()
  {
    // This context is now closed
    this->setClosed( true );
    INFO_STREAM << "TestContext::onStop " << _name << " : Stopping test context.";
  }


  void TestContext::updateContext( float timestep )
  {
    static Manager* manager = Manager::getInstance();

    if ( _timer.trigger( timestep ) )
    {
      DEBUG_STREAM << "TestContext::updateContext " << _name << " : Opening new context group";
      Manager::getInstance()->openContextGroup( _cg_load );
    }

    if ( _isLoadScreen )
    {
      if ( manager->getContextManager().isLoaded() )
      {
        DEBUG_STREAM <<  "TestContext::updateContext " << _name << " : Load screen complete.";
        manager->openEntryPoint();
        this->stopContext();
      }

    }
  }

}

