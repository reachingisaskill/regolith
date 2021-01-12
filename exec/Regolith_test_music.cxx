
#include "Regolith.h"
#include "Regolith/Test/JukeboxContext.h"
#include "Regolith/Test/StatusString.h"
#include "Regolith/Test/LoadScreen.h"

#include "testass.h"
#include "logtastic.h"


const char* test_config = "test_data/music_test/config.json";


using namespace Regolith;

int main( int, char** )
{
  logtastic::init();
  logtastic::setLogFileDirectory( "./test_data/logs/" );
  logtastic::addLogFile( "tests_music.log" );
  logtastic::setPrintToScreenLimit( logtastic::error );

  testass::control::init( "Regolith", "Music Interface" );
  testass::control::get()->setVerbosity( testass::control::verb_short );

  bool success = false;

  // Create the manager first so that it can register signal handlers
  Manager* man = Manager::createInstance();

  logtastic::start( "Regolith - Music Functionality Test", REGOLITH_VERSION_NUMBER );

  INFO_LOG( "Main : Creating test builders" );
  man->getObjectFactory().addBuilder< StatusString >( "status_string" );

  man->getContextFactory().addBuilder< JukeboxContext >( "jukebox" );
  man->getContextFactory().addBuilder< LoadScreen >( "load_screen" );

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


