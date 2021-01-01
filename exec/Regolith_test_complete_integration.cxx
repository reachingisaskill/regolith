
#include "Regolith.h"
#include "Regolith/Test/TestContext.h"
#include "Regolith/Test/EmptyContext.h"
#include "Regolith/Test/TestObject.h"
#include "Regolith/Test/StatusString.h"
#include "Regolith/Test/SimpleObject.h"
#include "Regolith/Test/ContainerObject.h"
#include "Regolith/Test/BonkObject.h"
#include "Regolith/Test/FPSString.h"

#include "testass.h"
#include "logtastic.h"


const char* test_config = "test_data/complete_test/config.json";


using namespace Regolith;

int main( int, char** )
{
  logtastic::init();
  logtastic::setLogFileDirectory( "./test_data/logs/" );
  logtastic::addLogFile( "tests_complete.log" );
  logtastic::setPrintToScreenLimit( logtastic::error );

  testass::control::init( "Regolith", "Complete Integration Test" );
  testass::control::get()->setVerbosity( testass::control::verb_short );

  bool success = false;

  // Create the manager first so that it can register signal handlers
  Manager* man = Manager::createInstance();

  logtastic::start( "Regolith - Complete Integration Test", REGOLITH_VERSION_NUMBER );

  INFO_LOG( "Main : Creating test builders" );
  man->getObjectFactory().addBuilder< TestObject >( "test" );
  man->getObjectFactory().addBuilder< SimpleObject >( "simple_object" );
  man->getObjectFactory().addBuilder< ContainerObject >( "container_object" );
  man->getObjectFactory().addBuilder< BonkObject >( "bonk_object" );
  man->getObjectFactory().addBuilder< StatusString >( "status_string" );
  man->getObjectFactory().addBuilder< FPSString >( "fps_string" );

  man->getContextFactory().addBuilder< TestContext >( "test" );
  man->getContextFactory().addBuilder< EmptyContext >( "empty" );

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

