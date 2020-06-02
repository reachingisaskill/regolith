
#include "Regolith/Managers/Manager.h"
#include "Regolith/Components/Engine.h"
#include "Regolith/Components/Window.h"

#include "Regolith/GameObjects/SimpleSprite.h"
#include "Regolith/GameObjects/CollidableSprite.h"
#include "Regolith/GameObjects/AnimatedSprite.h"
#include "Regolith/GameObjects/SimpleButton.h"
#include "Regolith/GameObjects/Trigger.h"
#include "Regolith/GameObjects/Region.h"
#include "Regolith/Contexts/TitleScene.h"
#include "Regolith/Contexts/Platformer.h"
#include "Regolith/Contexts/Menu.h"
#include "Regolith/GamePlay/Signal.h"

#include "logtastic.h"


namespace Regolith
{

  Manager::Manager() :
    _theThreads(),
    _theWindow(),
    _theInput(),
    _theAudio(),
    _theHardware(),
    _theData(),
    _theContexts(),
    _theEngine( _theInput, _defaultColor ),
    _theRenderer( nullptr ),
    _entryPoint( 0 ),
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
    // Set up the object factory
//    _objectFactory.addBuilder<FPSString>( "fps_string" );
    _objectFactory.addBuilder<SimpleSprite>( "simple_sprite" );
    _objectFactory.addBuilder<CollidableSprite>( "collidable_sprite" );
    _objectFactory.addBuilder<AnimatedSprite>( "animated_sprite" );
    _objectFactory.addBuilder<SimpleButton>( "simple_button" );
    _objectFactory.addBuilder<Trigger>( "event_trigger" );
    _objectFactory.addBuilder<Region>( "region" );

    // Set up the context factory
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
  }


  Manager::~Manager()
  {
    _defaultFont = nullptr;

    INFO_LOG( "Clearing team list" );
    _teamNames.clear();

    INFO_LOG( "Removing each of the fonts and clearing the map" );
    for ( FontMap::iterator it = _fonts.begin(); it != _fonts.end(); ++it )
    {
      TTF_CloseFont( it->second );
    }
    _fonts.clear();

    INFO_LOG( "Destroying the renderer" );
    SDL_DestroyRenderer( _theRenderer );
    _theRenderer = nullptr;

    INFO_LOG( "Clearing hardware manager" );
    _theHardware.clear();

    INFO_LOG( "Closing the SDL subsystems" );
    TTF_Quit();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Context Stack manipulation

  void Manager::openContext( IDNumber c )
  {
    DEBUG_LOG( "Opening Context" );
    _theEngine.stackOperation( Engine::StackOperation( Engine::StackOperation::PUSH, _theContexts.getContext( c ) ) );
  }


  void Manager::transferContext( IDNumber c )
  {
    DEBUG_LOG( "Transferring Context" );
    _theEngine.stackOperation( Engine::StackOperation( Engine::StackOperation::TRANSFER, _theContexts.getContext( c ) ) );
  }


  void Manager::closeContext()
  {
    DEBUG_LOG( "Closing Current Context" );
    _theEngine.stackOperation( Engine::StackOperation( Engine::StackOperation::POP ) );
  }


  void Manager::setContextStack( IDNumber c )
  {
    DEBUG_LOG( "Resetting Context Stack" );
    _theEngine.stackOperation( Engine::StackOperation( Engine::StackOperation::RESET, _theContexts.getContext( c ) ) );
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Utility functions

  void Manager::run()
  {
    // Reset the stack to the first context
    setContextStack( _entryPoint );

    // Start all the waiting threads
    {
      std::lock_guard<std::mutex> lk( _theThreads.StartCondition.mutex );
      _theThreads.StartCondition.data = true;
    }
    _theThreads.StartCondition.variable.notify_all();

    // Start the engine!
    _theEngine.run();
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

}

