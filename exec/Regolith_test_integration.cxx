#define __DEBUG_OFF__

#include "regolith.h"

#include "logtastic.h"


const char* test_config = "test_data/test_config.json";


using namespace Regolith;

int main( int, char** )
{
  logtastic::init();
  logtastic::setLogFileDirectory( "./test_data/logs/" );
  logtastic::addLogFile( "tests_integration.log" );
  logtastic::start( "Regolith - Integration Test", REGOLITH_VERSION_NUMBER );

  try
  {
    INFO_LOG( "Initialising the manager" );
    Manager* man = Manager::createInstance();
    man->init( test_config );

    INFO_LOG( "Starting rendering loop" );
    man->run();

  }
  catch ( Exception& ex )
  {
    FAILURE_LOG( ex.what() );
    std::cerr << ex.elucidate();
    return 0;
  }
  catch ( std::exception& ex )
  {
    FAILURE_LOG( "Unexpected exception occured:" );
    FAILURE_STREAM << ex.what();
    return 0;
  }

  logtastic::stop();
  return 0;
}

