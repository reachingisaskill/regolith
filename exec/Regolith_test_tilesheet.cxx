
#include "Regolith.h"
#include "Regolith/Test/EmptyContext.h"
#include "Regolith/Test/SimpleObject.h"
#include "Regolith/Test/TiledObject.h"
#include "Regolith/Test/ContainerObject.h"
#include "Regolith/Test/BonkObject.h"
#include "Regolith/Test/FPSString.h"

#include "testass.h"
#include "logtastic.h"


const char* test_config = "test_data/tilesheet_test/config.json";


using namespace Regolith;

int main( int, char** )
{
  logtastic::init();
  logtastic::setLogFileDirectory( "./test_data/logs/" );
  logtastic::setLogFile( "tests_tilesheets.log" );
  logtastic::setPrintToScreenLimit( logtastic::error );

  testass::control::init( "Regolith", "Tilesheet Simulation" );
  testass::control::get()->setVerbosity( testass::control::verb_short );

  bool success = false;

  // Create the manager first so that it can register signal handlers
  Manager* man = Manager::createInstance();

  logtastic::start( "Regolith - Tilesheet Object Generation Test", REGOLITH_VERSION_NUMBER );

  INFO_LOG( "Main : Creating test builders" );
  man->getObjectFactory().addBuilder< TiledObject >( "tiled_object" );
  man->getObjectFactory().addBuilder< ContainerObject >( "container_object" );
  man->getObjectFactory().addBuilder< BonkObject >( "bonk_object" );
  man->getObjectFactory().addBuilder< FPSString >( "fps_string" );

  man->getContextFactory().addBuilder< EmptyContext >( "empty_context" );

  try
  {
    INFO_LOG( "Main : Initialising the manager" );
    man->init( test_config );

    INFO_LOG( "Main : Starting Regolith" );
    success = man->run();
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


