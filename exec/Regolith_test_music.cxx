
#include "Regolith.h"
#include "Regolith/Test/JukeboxContext.h"
#include "Regolith/Test/StatusString.h"

#include "logtastic.h"


const char* test_config = "test_data/music_test/config.json";


using namespace Regolith;

int main( int, char** )
{
  logtastic::init();
  logtastic::setLogFileDirectory( "./test_data/logs/" );
  logtastic::addLogFile( "tests_music.log" );

  // Create the manager first so that it can register signal handlers
  Manager* man = Manager::createInstance();

  logtastic::start( "Regolith - Music Functionality Test", REGOLITH_VERSION_NUMBER );

  INFO_LOG( "Main : Creating test builders" );
  man->getObjectFactory().addBuilder< StatusString >( "status_string" );

  man->getContextFactory().addBuilder< JukeboxContext >( "jukebox" );

  try
  {
    INFO_LOG( "Main : Initialising the manager" );
    man->init( test_config );

    INFO_LOG( "Main : Starting Regolith" );
    man->run();

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


