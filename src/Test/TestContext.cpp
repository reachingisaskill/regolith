
#include "Regolith/Test/TestContext.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  TestContext::TestContext() :
    Context(),
    _cg_load( nullptr ),
    _child_load( nullptr ),
    _timer(),
    _childTimer(),
    _deathTimer()
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

    // Load a child context
    if ( validateJson( tests, "child_context_loading", JsonType::OBJECT, false ) )
    {
      Json::Value& cg_loading_data = tests["child_context_loading"];
      validateJson( cg_loading_data, "wait_for", JsonType::INTEGER );
      validateJson( cg_loading_data, "context", JsonType::STRING );

      INFO_STREAM << "TestContext::configure : Testing the child context loading functionality, Context Group : " << cg_loading_data["context"].asString() << " after " << cg_loading_data["wait_for"].asFloat() << " milliseconds.";

      _childTimer.configure( cg_loading_data["wait_for"].asFloat(), 1 );
      _child_load = group.getContextPointer( cg_loading_data["context"].asString() );
    }

    // Test loading another context group
    if ( validateJson( tests, "context_group_loading", JsonType::OBJECT, false ) )
    {
      Json::Value& cg_loading_data = tests["context_group_loading"];
      validateJson( cg_loading_data, "wait_for", JsonType::INTEGER );
      validateJson( cg_loading_data, "context_group", JsonType::STRING );

      INFO_STREAM << "TestContext::configure : Testing the context group loading functionality, Context Group : " << cg_loading_data["context_group"].asString() << " after " << cg_loading_data["wait_for"].asFloat() << " milliseconds.";

      _timer.configure( cg_loading_data["wait_for"].asFloat(), 1 );
      _cg_load = Manager::getInstance()->getContextManager().getContextGroup( cg_loading_data["context_group"].asString() );
    }

    _deathTimer.configure( 5000.0, 1 ); // Kill self after 5 seconds
  }


  void TestContext::onStart()
  {
    // Make sure this context stays open
    this->setClosed( false );

    INFO_STREAM << "TestContext::onStart " << _name << " : Starting test context.";

    _timer.reset();

    _childTimer.reset();

    _deathTimer.reset();
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
      manager->openContextGroup( _cg_load );
    }

    if ( _childTimer.trigger( timestep ) )
    {
      DEBUG_STREAM << "TestContext::updateContext " << _name << " : Opening child context @ " << *_child_load;
      manager->openContext( *_child_load );
    }

    if ( _deathTimer.trigger( timestep ) )
    {
      DEBUG_STREAM << "TestContext::updateContext " << _name << " : Suicide";
      this->stopContext();
    }
  }

}

