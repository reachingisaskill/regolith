
#include "Regolith.h"
#include "Regolith/Test/TestCharacter.h"

#include "logtastic.h"


const char* test_config = "test_data/test_config.json";


using namespace Regolith;

int main( int, char** )
{
  logtastic::init();
  logtastic::setLogFileDirectory( "./test_data/logs/" );
  logtastic::addLogFile( "tests_integration.log" );

  // Create the manager first so that it can register signal handlers
  INFO_LOG( "Initialising the manager" );
  Manager* man = Manager::createInstance();

  logtastic::start( "Regolith - Integration Test", REGOLITH_VERSION_NUMBER );

  try
  {
    INFO_LOG( "Creating custom object builders" );
    man->getObjectFactory().addBuilder< Regolith::TestCharacter >( "test_character" );

    INFO_LOG( "Creating custom context builders" );
//    man->getObjectFactory()->addBuilder( new TestContextBuilder() );

//    INFO_LOG( "Adding teams" );
//    man->addTeam( "environment", 0 );
//    man->addTeam( "player", 1 );

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

