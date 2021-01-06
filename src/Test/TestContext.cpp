
#include "Regolith/Test/TestContext.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  TestContext::TestContext() :
    Context(),
    _name(),
    _cgSignal(),
    _childSignal(),
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

      INFO_STREAM << "TestContext::configure : Testing the child context loading functionality, Context Group : " << cg_loading_data["context"].asString() << " after " << cg_loading_data["wait_for"].asFloat() << " milliseconds.";

      _childTimer.configure( cg_loading_data["wait_for"].asFloat(), 1 );
//      _childLoad = group.getContextPointer( cg_loading_data["context"].asString() );
      _childSignal.configure( cg_loading_data, group );
    }

    // Test loading another context group
    if ( validateJson( tests, "context_group_loading", JsonType::OBJECT, false ) )
    {
      Json::Value& cg_loading_data = tests["context_group_loading"];
      validateJson( cg_loading_data, "wait_for", JsonType::INTEGER );

      INFO_STREAM << "TestContext::configure : Testing the context group loading functionality, Context Group : " << cg_loading_data["context_group"].asString() << " after " << cg_loading_data["wait_for"].asFloat() << " milliseconds.";

      _timer.configure( cg_loading_data["wait_for"].asFloat(), 1 );
//      _cgLoad = Manager::getInstance()->getContextManager<Context>().getContextGroup( cg_loading_data["context_group"].asString() );
      _cgSignal.configure( cg_loading_data, group );
    }

    // Set the lifetime of the context
    if ( validateJson( tests, "death", JsonType::FLOAT, false ) )
    {
      INFO_STREAM << "TestContext::configure : Suicide after " << tests["death"].asFloat() << " milliseconds.";
      _deathTimer.configure( tests["death"].asFloat(), 1 );
    }
    else
    {
      _deathTimer.configure( 5000.0, 1 ); // Kill self after 5 seconds
    }
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
    if ( _timer.trigger( timestep ) )
    {
      DEBUG_STREAM << "TestContext::updateContext " << _name << " : Opening new context group";
      _cgSignal.trigger();
    }

    if ( _childTimer.trigger( timestep ) )
    {
      DEBUG_STREAM << "TestContext::updateContext : Opening child context";
      _childSignal.trigger();
    }

    if ( _deathTimer.trigger( timestep ) )
    {
      DEBUG_STREAM << "TestContext::updateContext " << _name << " : Suicide";
      this->stopContext();
    }
  }

}

