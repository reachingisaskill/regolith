#define LOGTASTIC_DEBUG_OFF

#include "Regolith/Managers/Manager.h"
#include "Regolith/Components/Engine.h"
#include "Regolith/Components/Window.h"

#include "Regolith/GameObjects/MusicTrack.h"
#include "Regolith/GameObjects/SimpleSprite.h"
#include "Regolith/GameObjects/CollidableSprite.h"
#include "Regolith/GameObjects/AnimatedSprite.h"
#include "Regolith/GameObjects/SimpleButton.h"
#include "Regolith/GameObjects/Trigger.h"
#include "Regolith/GameObjects/Region.h"
#include "Regolith/Contexts/TitleScene.h"
#include "Regolith/Contexts/Platformer.h"
#include "Regolith/Contexts/Menu.h"
#include "Regolith/Contexts/LoadScreen.h"
#include "Regolith/GamePlay/Signal.h"

#include "logtastic.h"


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
    // Set up signal handlers
    logtastic::registerSignalHandler( SIGABRT, deathSignals );
    logtastic::registerSignalHandler( SIGFPE, deathSignals );
    logtastic::registerSignalHandler( SIGILL, deathSignals );
    logtastic::registerSignalHandler( SIGINT, deathSignals );
    logtastic::registerSignalHandler( SIGSEGV, deathSignals );
    logtastic::registerSignalHandler( SIGTERM, deathSignals );


    // Set up the object factory
//    _objectFactory.addBuilder<FPSString>( "fps_string" );
    _objectFactory.addBuilder<MusicTrack>( "music_track" );
    _objectFactory.addBuilder<SimpleSprite>( "simple_sprite" );
    _objectFactory.addBuilder<CollidableSprite>( "collidable_sprite" );
    _objectFactory.addBuilder<AnimatedSprite>( "animated_sprite" );
    _objectFactory.addBuilder<SimpleButton>( "simple_button" );
    _objectFactory.addBuilder<Trigger>( "event_trigger" );
    _objectFactory.addBuilder<Region>( "region" );

    // Set up the context factory
    _contextFactory.addBuilder<LoadScreen>( "load_screen" );
    _contextFactory.addBuilder<TitleScene>( "title_scene" );
    _contextFactory.addBuilder<MenuContext>( "menu" );
    _contextFactory.addBuilder<Platformer>( "platformer" );

    // Set up the signal factory
    _signalFactory.addBuilder<InputActionSignal>( "input_action" );
    _signalFactory.addBuilder<InputBooleanSignal>( "input_boolean" );
    _signalFactory.addBuilder<InputFloatSignal>( "input_float" );
    _signalFactory.addBuilder<InputVectorSignal>( "input_vector" );
    _signalFactory.addBuilder<GameEventSignal>( "game_event" );
    _signalFactory.addBuilder<ChangeContextSignal>( "context_change" );
    _signalFactory.addBuilder<ChangeContextGroupSignal>( "context_group_change" );
  }


  Manager::~Manager()
  {
    // Close the threads first
    _theThreads.quit();

    _defaultFont = nullptr;

    INFO_LOG( "Clearing team list" );
    _teamNames.clear();

    INFO_LOG( "Removing each of the fonts and clearing the map" );
    for ( FontMap::iterator it = _fonts.begin(); it != _fonts.end(); ++it )
    {
      TTF_CloseFont( it->second );
    }
    _fonts.clear();

    _theContexts.clear();

    _theData.clear();

    _theAudio.clear();

    INFO_LOG( "Clearing hardware manager" );
    _theHardware.clear();

    INFO_LOG( "Closing the SDL subsystems" );
    TTF_Quit();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
  }


  SDL_Renderer* Manager::requestRenderer()
  {
    if ( _rendererExists )
    {
      Exception ex( "Manager::requestRenderer()", "Renderer already exists - only one thread may own one." );
      throw ex;
    }
    _rendererExists = true;

    return SDL_CreateRenderer( _theWindow.getSDLWindow(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Context Stack manipulation

  void Manager::openContext( Context* c )
  {
    DEBUG_LOG( "Opening Context" );
    _theEngine.stackOperation( Engine::StackOperation( Engine::StackOperation::PUSH, c ) );
  }


  void Manager::transferContext( Context* c )
  {
    DEBUG_LOG( "Transferring Context" );
    _theEngine.stackOperation( Engine::StackOperation( Engine::StackOperation::TRANSFER, c ) );
  }


  void Manager::closeContext()
  {
    DEBUG_LOG( "Closing Current Context" );
    _theEngine.stackOperation( Engine::StackOperation( Engine::StackOperation::POP ) );
  }


  void Manager::setContextStack( Context* c )
  {
    DEBUG_STREAM << "Resetting Context Stack @ " << c;
    _theEngine.stackOperation( Engine::StackOperation( Engine::StackOperation::RESET, c ) );
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Utility functions

  void Manager::run()
  {
    // Start all the waiting threads
    _theThreads.startAll();

    // Load the first context group blocking this thread until completion
    _theContexts.loadEntryPoint();

    // Reset the stack to the first context
    setContextStack( _theContexts.getCurrentContextGroup()->getEntryPoint() );


    // Start the engine!
    _theEngine.run();


    // Join all the threads
    _theThreads.stopAll();
  }


  TTF_Font* Manager::getFontPointer( std::string name )
  {
    FontMap::iterator find = _fonts.find( name );
    if ( find == _fonts.end() )
    {
      ERROR_STREAM << "Could not find requested font : " << name;
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
    INFO_STREAM << "Registering " << REGOLITH_EVENT_TOTAL << " user events";

    if ( start_num == (unsigned int)-1 )
    {
      std::string error = SDL_GetError();
      FAILURE_STREAM << "Could not create required user events : " << error;
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
    FAILURE_STREAM << "Regolith received signal: " << signal;
    FAILURE_LOG( "Trying to die gracefully..." );

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

