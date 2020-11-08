
#include "Regolith/Managers/Manager.h"
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
    _theEngine( _theInput, _defaultColor ),
    _objectFactory(),
    _contextFactory(),
    _signalFactory(),
    _fonts(),
    _teamNames(),
    _title(),
    _defaultFont( nullptr ),
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
    // Close the threads first
    _theThreads.quit();

    INFO_LOG( "Manager::~Manager : Clearing team list" );
    _teamNames.clear();

    INFO_LOG( "Manager::~Manager : Removing each of the fonts and clearing the map" );
    for ( FontMap::iterator it = _fonts.begin(); it != _fonts.end(); ++it )
    {
      TTF_CloseFont( it->second );
    }
    _fonts.clear();

    _theContexts.clear();

    _theData.clear();

    _theAudio.clear();

    INFO_LOG( "Manager::~Manager : Clearing hardware manager" );
    _theHardware.clear();

    INFO_LOG( "Manager::~Manager : Closing the SDL subsystems" );
    TTF_Quit();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
  }


  SDL_Renderer* Manager::requestRenderer()
  {
//    return SDL_CreateRenderer( _theWindow.getSDLWindow(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
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

    // Prepare the context manager
    _theContexts.setNextContextGroup( cg );

    // Tell the engine to queue the load screen for the context group
    _theEngine.openContextGroup( (Context*)cg->getLoadScreen() );

    // Tell the context manager that we can trigger the load thread
    _theContexts.loadNextContextGroup();
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Utility functions

  void Manager::run()
  {
    // Start all the waiting threads
    INFO_LOG( "Manager::run : Starting worker threads" );
    _theThreads.startAll();

    // Load the first context group blocking this thread until completion
    INFO_LOG( "Manager::run : Loading entry point" );
    _theContexts.loadEntryPoint();

    // Reset the stack to the first context
    INFO_LOG( "Manager::run : Loading the first context" );
    this->openEntryPoint();


    // Start the engine!
    INFO_LOG( "Manager::run : Starting the engine." );
    _theEngine.run();


    // Join all the threads
    INFO_LOG( "Manager::run : Stopping all worker threads" );
    _theThreads.stopAll();
  }


  TTF_Font* Manager::getFontPointer( std::string name )
  {
    FontMap::iterator find = _fonts.find( name );
    if ( find == _fonts.end() )
    {
      ERROR_STREAM << "Manager::getFontPointer : Could not find requested font : " << name;
      return _defaultFont;
    }
    return find->second;
  }


  TeamID Manager::getTeamID( std::string name )
  {
    TeamNameMap::iterator found = _teamNames.find( name );
    if ( found == _teamNames.end() )
    {
      Exception ex( "Manager::getTeamID()", "Could not find requested team name. Cannot load object." );
      ex.addDetail( "Team Name", name );
      throw ex;
    }
    return found->second;
  }


  void Manager::renderSurfaces( DataHandler* handler )
  {
    _theEngine.renderTextures( handler );
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

    ERROR_STREAM << "Last SDL Error : " << SDL_GetError();
    ERROR_STREAM << "Last IMG Error : " << IMG_GetError();
    ERROR_STREAM << "Last TTF Error : " << TTF_GetError();
    ERROR_STREAM << "Last MIX Error : " << Mix_GetError();

#ifdef __linux__
    void* array[20];
    size_t size;

    size = backtrace( array, 50 );

    fprintf( stderr, "Error Signal %d:\n", signal );
    backtrace_symbols_fd( array, size, STDERR_FILENO );
#endif

    Manager::getInstance()->quit();
  }

}

