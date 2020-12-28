
#include "Regolith/Test/TestContext.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  TestContext::TestContext() :
    Context(),
    _cg_load( nullptr ),
    _timer(),
    _deathTimer(),
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
    validateJson( json_data, "tests", JsonType::OBJECT );
    Json::Value& tests = json_data["tests"];

    // Load the name of this context
    validateJson( tests, "name", JsonType::STRING );
    _name = tests["name"].asString();

    // Test loading another context group
    if ( validateJson( tests, "context_group_loading", JsonType::OBJECT, false ) )
    {
      Json::Value& cg_loading_data = tests["context_group_loading"];
      validateJson( cg_loading_data, "wait_for", JsonType::INTEGER );
      validateJson( cg_loading_data, "context_group", JsonType::STRING );

      INFO_STREAM << "TestContext::configure : Testing the context group loading functionality, Context Group : " << cg_loading_data["context_group"].asString() << " after " << cg_loading_data["wait_for"].asFloat() << " seconds.";

      _timer.configure( cg_loading_data["wait_for"].asFloat(), 1 );
      _cg_load = Manager::getInstance()->getContextManager().getContextGroup( cg_loading_data["context_group"].asString() );
    }

    // If this iteration is acting as a load_screen
    if ( validateJson( tests, "load_screen", JsonType::BOOLEAN, false ) )
    {
      _isLoadScreen = tests["load_screen"].asBool();
    }

    _deathTimer.configure( 5000.0, 1 ); // Kill self after 5 seconds
  }


  void TestContext::onStart()
  {
    // Make sure this context stays open
    this->setClosed( false );

    INFO_STREAM << "TestContext::onStart " << _name << " : Starting test context.";

    _timer.reset();

    _deathTimer.reset();

//    if ( _isLoadScreen )
//    {
//      Manager::getInstance()->getContextManager().loadNextContextGroup();
//    }
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

//    if ( _isLoadScreen )
//    {
//      if ( manager->getContextManager().isLoaded() )
//      {
//        DEBUG_STREAM <<  "TestContext::updateContext " << _name << " : Load screen complete.";
//        manager->openEntryPoint();
//        this->stopContext();
//      }
//
//    }

    if ( _deathTimer.trigger( timestep ) )
    {
      this->stopContext();
    }
  }

}

