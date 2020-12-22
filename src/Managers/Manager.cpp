
#include "Regolith/Managers/Manager.h"
#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/Components/Engine.h"
#include "Regolith/Components/Window.h"


namespace Regolith
{

  // Forward declare the signal handler
  void deathSignals( int );


  Manager::Manager() :
    _theThreads(),
    _theWindow(),
    _rendererExists( false ),
    _theInput(),
    _theAudio(),
    _theHardware(),
    _theData(),
    _theContexts(),
    _theEngine( _theInput ),
    _objectFactory(),
    _contextFactory(),
    _signalFactory(),
    _teamNames(),
    _typeNames(),
    _title(),
    _defaultColor( { 255, 255, 255, 255 } ),
    _eventStartIndex(0),
    _gameEvents(),
    _gravityConst( 0.0, 0.01 ),
    _dragConst( 0.005 )
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
  // Context Stack manipulation

  void Manager::openEntryPoint()
  {
    DEBUG_LOG( "Manager::openEntryPoint : Opening Entry Point" );
    _theEngine.openContext( _theContexts.getCurrentContextGroup()->getEntryPoint() );
  }


  void Manager::openContext( Context* c )
  {
    DEBUG_LOG( "Manager::openContext : Opening Context" );
    _theEngine.openContext( c );
  }


  void Manager::openContextGroup( ContextGroup* cg )
  {
    DEBUG_LOG( "Manager::openContextGroup : Opening Context Group" );
    DEBUG_STREAM << "Manager::openContextGroup : Context Load Screen @ " << *cg->getLoadScreen();

    // Prepare the context manager
    _theContexts.setNextContextGroup( cg );

    // Tell the engine to queue the load screen for the context group
    _theEngine.openContextGroup( *cg->getLoadScreen() );

    // Tell the context manager that we can trigger the load thread
//    _theContexts.loadNextContextGroup();
    // This funtion MUST be called when the loadscreen context is opened!
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Utility functions

  void Manager::run()
  {
    try
    {
      // Start all the waiting threads
      INFO_LOG( "Manager::run : Starting worker threads" );
      _theThreads.startAll();


      try
      {
        // Load the first context group blocking this thread until completion
        INFO_LOG( "Manager::run : Loading entry point" );
        _theContexts.loadEntryPoint();

        // Reset the stack to the first context
        INFO_LOG( "Manager::run : Loading the first context" );
        this->openEntryPoint();

        // Start the engine!
        INFO_LOG( "Manager::run : Starting the engine." );
        _theEngine.run();
      }
      catch ( Exception& ex )
      {
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
        ERROR_LOG( "Manager::run : A Regolith exception occured unloading data" );
        ERROR_STREAM << ex.elucidate();
        std::cerr << ex.elucidate();
        return;
      }

        // Stop all the threads
      INFO_LOG( "Manager::run : Stopping all worker threads" );
      _theThreads.stopAll();

    }
    catch ( std::exception& ex )
    {
      ERROR_LOG( "Manager::run : A unexpected exception occured." );
      ERROR_STREAM << ex.what();
      _theThreads.error();
    }

    // Join all the threads
    INFO_LOG( "Manager::run : Joining all worker threads" );
    _theThreads.join();
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Accessors for global data

  CollisionTeam Manager::getCollisionTeam( std::string name )
  {
    TeamNameMap::iterator found = _teamNames.find( name );
    if ( found == _teamNames.end() )
    {
      Exception ex( "Manager::getCollisionTeam()", "Could not find requested team name." );
      ex.addDetail( "Team Name", name );
      throw ex;
    }
    return found->second;
  }


  CollisionType Manager::getCollisionType( std::string name )
  {
    TypeNameMap::iterator found = _typeNames.find( name );
    if ( found == _typeNames.end() )
    {
      Exception ex( "Manager::getCollisionType()", "Could not find requested type name." );
      ex.addDetail( "Type Name", name );
      throw ex;
    }
    return found->second;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Configure user events

  void Manager::configureEvents()
  {
    // Load user events, etc
    Uint32 start_num = SDL_RegisterEvents( REGOLITH_EVENT_TOTAL );
    INFO_STREAM << "Manager::configureEvents : Registering " << REGOLITH_EVENT_TOTAL << " user events";

    if ( start_num == (unsigned int)-1 )
    {
      std::string error = SDL_GetError();
      FAILURE_STREAM << "Manager::configureEvents : Could not create required user events : " << error;
      Exception ex( "Manager::configureEvents()", "Could not create user events", true );
      ex.addDetail( "SDL Error", error );
      throw ex;
    }

    for ( unsigned int i = 0; i < (unsigned int)REGOLITH_EVENT_TOTAL; ++i )
    {
      SDL_memset( &_gameEvents[ i ], 0, sizeof(_gameEvents[ i ]) );
      _gameEvents[ i ].type = start_num; // This should be the same number as SDL_USEREVENT
      _gameEvents[ i ].user.code = i;
      _gameEvents[ i ].user.data1 = nullptr;
      _gameEvents[ i ].user.data2 = nullptr;
    }
  }


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

