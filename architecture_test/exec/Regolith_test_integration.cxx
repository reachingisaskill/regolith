
#include "Regolith.h"
#include "Regolith/Test/TestPlayer.h"

#include "logtastic.h"


const char* test_config = "test_data/test_config.json";


using namespace Regolith;

int main( int, char** )
{
  logtastic::init();
  logtastic::setLogFileDirectory( "./test_data/logs/" );
  logtastic::addLogFile( "tests_integration.log" );
  logtastic::start( "Regolith - Integration Test", REGOLITH_VERSION_NUMBER );

  INFO_LOG( "Initialising the manager" );
  Manager* man = Manager::createInstance();

  try
  {
    INFO_LOG( "Creating custom object builders" );
    man->getObjectFactory()->addBuilder( new TestPlayerBuilder() );

    INFO_LOG( "Creating custom context builders" );
//    man->getObjectFactory()->addBuilder( new TestContextBuilder() );

    INFO_LOG( "Initialising the manager" );
    man->init( test_config );

    INFO_LOG( "Starting Regolith" );
    man->run();

  }
  catch ( Exception& ex )
  {
    FAILURE_LOG( ex.what() );
    std::cerr << ex.elucidate();
  }
  catch ( std::exception& ex )
  {
    FAILURE_LOG( "Unexpected exception occured:" );
    FAILURE_STREAM << ex.what();
  }

  Manager::killInstance();
  logtastic::stop();
  return 0;
}

