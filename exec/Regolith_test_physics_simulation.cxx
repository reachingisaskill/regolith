
#include "Regolith.h"
#include "Regolith/Test/EmptyContext.h"
#include "Regolith/SimpleObject.h"

#include "logtastic.h"


const char* test_config = "test_data/physics_test/config.json";


using namespace Regolith;

int main( int, char** )
{
  logtastic::init();
  logtastic::setLogFileDirectory( "./test_data/logs/" );
  logtastic::addLogFile( "tests_physics_simulation.log" );

  // Create the manager first so that it can register signal handlers
  Manager* man = Manager::createInstance();

  logtastic::start( "Regolith - Full Physics Simulation Test", REGOLITH_VERSION_NUMBER );

  INFO_LOG( "Main : Creating test builders" );
  man->getObjectFactory().addBuilder< SimpleObject >( "simple_object" );
  man->getContextFactory().addBuilder< EmptyContext >( "empty_context" );

  try
  {
    INFO_LOG( "Main : Initialising the manager" );
    man->init( test_config );

    INFO_LOG( "Main : Starting Regolith" );
    man->run();

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

  Manager::killInstance();
  logtastic::stop();
  return 0;
}


