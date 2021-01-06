
#include "Regolith/Managers/Manager.h"
#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/Components/Engine.h"
#include "Regolith/Components/Window.h"


namespace Regolith
{

  // Forward declare the signal handler
  void deathSignals( int );


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Con/Destruction

  Manager::Manager() :
    _theThreads(),
    _theWindow(),
    _rendererExists( false ),
    _theInput(),
    _theAudio(),
    _theHardware(),
    _theCollision(),
    _theData(),
    _theContexts(),
    _theEngine( _theInput ),
    _objectFactory(),
    _contextFactory(),
    _teamNames(),
    _typeNames(),
    _title(),
    _eventStartIndex(0),
    _gameEvents()
  {
    DEBUG_LOG( "Manager::Manager : Contruction" );
    // Set up signal handlers
    logtastic::registerSignalHandler( SIGABRT, deathSignals );
    logtastic::registerSignalHandler( SIGFPE, deathSignals );
    logtastic::registerSignalHandler( SIGILL, deathSignals );
    logtastic::registerSignalHandler( SIGINT, deathSignals );
    logtastic::registerSignalHandler( SIGSEGV, deathSignals );
    logtastic::registerSignalHandler( SIGTERM, deathSignals );
  }


  Manager::~Manager()
  {
    DEBUG_LOG( "Manager::~Manager : Destruction" );

    INFO_LOG( "Manager::~Manager : Clearing collision data" );
    _theCollision.clear();

    INFO_LOG( "Manager::~Manager : Clearing font data" );
    _theFonts.clear();

    INFO_LOG( "Manager::~Manager : Clearing audio manager" );
    _theAudio.clear();

    INFO_LOG( "Manager::~Manager : Clearing hardware manager" );
    _theHardware.clear();

    INFO_LOG( "Manager::~Manager : Closing the SDL subsystems" );
    TTF_Quit();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
  }


  Camera& Manager::requestCamera()
  {
    return _theWindow.create();
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Run!

  bool Manager::run()
  {
    bool success = true;
    try
    {
      // Start all the waiting threads
      INFO_LOG( "Manager::run : Starting worker threads" );
      _theThreads.startAll();


      try
      {
        // Load the first context group blocking this thread until completion
        INFO_LOG( "Manager::run : Loading entry point" );
        ContextGroup* entryPoint = _theContexts.loadEntryPoint();

        // Reset the stack to the first context
        INFO_LOG( "Manager::run : Loading the first context" );
//        this->openEntryPoint();
        _theEngine.openContextStack( entryPoint->getEntryPoint() );

        // Start the engine!
        INFO_LOG( "Manager::run : Starting the engine." );
        _theEngine.run();
      }
      catch ( Exception& ex )
      {
        success = false;
        ERROR_LOG( "Manager::run : A Regolith error occured during runtime" );
        ERROR_STREAM << ex.elucidate();
        std::cerr << ex.elucidate();
      }


      try
      {
        // Unload everything
        INFO_LOG( "Manager::run : Unloading data" );
        _theContexts.clear();
      }
      catch ( Exception& ex )
      {
        success = false;
        ERROR_LOG( "Manager::run : A Regolith exception occured unloading data" );
        throw ex;
      }

        // Stop all the threads
      INFO_LOG( "Manager::run : Stopping all worker threads" );
      _theThreads.stopAll();

    }
    catch ( std::exception& ex )
    {
      success = false;
      ERROR_LOG( "Manager::run : A unexpected exception occured." );
      ERROR_STREAM << ex.what();
      _theThreads.error();
    }

    // Join all the threads
    INFO_LOG( "Manager::run : Joining all worker threads" );
    _theThreads.join();

    return success;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Event functions

  void Manager::raiseEvent( RegolithEvent eventNum )
  {
    SDL_PushEvent( &_gameEvents[ eventNum ] );
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Signal handler

#ifdef __linux__
#include <execinfo.h>
#include <stdio.h>
#include <unistd.h>
#endif

  void deathSignals( int signal )
  {
    FAILURE_STREAM << "DEATHSIGNAL : Regolith received signal: " << signal;
    FAILURE_LOG( "DEATHSIGNAL : Trying to die gracefully..." );

#ifdef __linux__
    void* array[20];
    size_t size;

    size = backtrace( array, 50 );

    fprintf( stderr, "Error Signal %d:\n", signal );
    backtrace_symbols_fd( array, size, STDERR_FILENO );
#endif

    Manager::getInstance()->error();
  }

}

