
#include "Regolith/Managers/Manager.h"
#include "Regolith/Components/Engine.h"
#include "Regolith/Components/Window.h"

#include "Regolith/GameObjects/SimpleSprite.h"
#include "Regolith/GameObjects/AnimatedSprite.h"
#include "Regolith/Contexts/TitleScene.h"

#include "logtastic.h"


namespace Regolith
{

  Manager::Manager() :
    _theWindow(),
    _theInput(),
    _theAudio(),
    _theEngine( _theInput, _defaultColor ),
    _theRenderer( nullptr ),
    _objectFactory(),
    _contextFactory(),
    _signalFactory(),
    _fonts(),
    _rawTextures(),
    _teamNames(),
    _gameObjects( "manager_game_object_list" ),
    _physicalObjects( "manager_physical_object_list" ),
    _contexts( "manager_context_list" ),
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
//    _objectFactory.addBuilder<Button>( "button" );
    _objectFactory.addBuilder<SimpleSprite>( "simple_sprite" );
    _objectFactory.addBuilder<AnimatedSprite>( "animated_sprite" );

    // Set up the context factory
//    _contextFactory.addBuilder<Platformer>( "platformer" );
    _contextFactory.addBuilder<TitleScene>( "title_scene" );
  }


  Manager::~Manager()
  {
    _defaultFont = nullptr;

    // Remove each of scenes and clear the vector
    INFO_LOG( "Deleteing Scenes" );
    _contexts.clear();

    INFO_LOG( "Deleting Physical Objects" );
    _physicalObjects.clear();

    INFO_LOG( "Deleting GameObjects" );
    _gameObjects.clear();

    INFO_LOG( "Clearing team list" );
    _teamNames.clear();

    INFO_LOG( "Deleting raw texture data" );
    for ( RawTextureMap::iterator it = _rawTextures.begin(); it != _rawTextures.end(); ++it )
    {
      SDL_DestroyTexture( it->second.texture );
    }
    _rawTextures.clear();

    INFO_LOG( "Removing each of the fonts and clearing the map" );
    for ( FontMap::iterator it = _fonts.begin(); it != _fonts.end(); ++it )
    {
      TTF_CloseFont( it->second );
    }
    _fonts.clear();

    INFO_LOG( "Destroying the renderer" );
    SDL_DestroyRenderer( _theRenderer );
    _theRenderer = nullptr;

    INFO_LOG( "Closing the SDL subsystems" );
    TTF_Quit();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
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
    DEBUG_LOG( "Closing All Contexts" );
    _theEngine.stackOperation( Engine::StackOperation( Engine::StackOperation::RESET, c ) );
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Utility functions

  void Manager::run()
  {
    if ( _contexts.size() == 0 )
    {
      FAILURE_LOG( "Context list is empty - there is nothing to render!" );
      Exception ex( "Manager::run()", "No contexts to load", false );
      throw ex;
    }

    // Reset the stack to the first context
    setContextStack( _contexts[0] );

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


  RawTexture Manager::findRawTexture( std::string name ) const
  {
    RawTextureMap::const_iterator found = _rawTextures.find( name );
    if ( found == _rawTextures.end() )
    {
      ERROR_STREAM << "Failed to find raw texture with name: " << name;
      Exception ex( "ID::findRawTexture()", "Could not find raw texture", true );
      ex.addDetail( "Texture name", name );
      throw ex;
    }

    return found->second;
  }

//  Context* Manager::buildContext( Json::Value& ) // See Managers/ManagerBuild.cpp

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

