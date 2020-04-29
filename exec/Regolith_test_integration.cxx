
#include "regolith.h"
#include "TestPlayer.h"

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
    man->init( test_config );

    ObjectBuilder* builder = man->getObjectBuilder();
    builder->addFactory( new TestPlayerFactory() );

    INFO_LOG( "Starting rendering loop" );
    Scene* the_scene = man->loadScene( 0 );

    man->getEnginePointer()->setScene( the_scene );
    man->getEnginePointer()->run();

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

