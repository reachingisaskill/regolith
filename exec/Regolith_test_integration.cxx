
#include "Regolith.h"
#include "Regolith/Test/TestContext.h"
#include "Regolith/Test/TestObject.h"

#include "testass.h"
#include "logtastic.h"


const char* test_config = "test_data/integration_test/config.json";


using namespace Regolith;

int main( int, char** )
{
  logtastic::init();
  logtastic::setLogFileDirectory( "./test_data/logs/" );
  logtastic::addLogFile( "tests_integration.log" );
  logtastic::setPrintToScreenLimit( logtastic::error );

  testass::control::init( "Regolith", "Integration Test" );
  testass::control::get()->setVerbosity( testass::control::verb_short );

  bool success = false;

  // Create the manager first so that it can register signal handlers
  Manager* man = Manager::createInstance();

  logtastic::start( "Regolith - Integration Test", REGOLITH_VERSION_NUMBER );

  INFO_LOG( "Main : Creating test builders" );
  man->getObjectFactory().addBuilder< TestObject >( "null" );
  man->getContextFactory().addBuilder< TestContext >( "null" );

  try
  {
    INFO_LOG( "Main : Initialising the manager" );
    man->init( test_config );

    INFO_LOG( "Main : Starting Regolith" );
    man->run();

    success = true;
  }
  catch ( Exception& ex )
  {
    FAILURE_LOG( ex.what() );
    std::cerr << ex.elucidate();
  }
  catch ( std::exception& ex )
  {
    FAILURE_LOG( "Main : Unexpected exception occured:" );
    FAILURE_STREAM << ex.what();
  }

  ASSERT_TRUE( success );

  if ( ! testass::control::summarize() )
  {
    testass::control::printReport( std::cout );
  }

  testass::control::kill();
  Manager::killInstance();
  logtastic::stop();
  return 0;
}

