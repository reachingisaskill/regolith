
#include "Manager.h"

#include "Exception.h"
#include "Engine.h"
#include "ScenePlatformer.h"
#include "SceneTitle.h"

#include "logtastic.h"


namespace Regolith
{

  Manager::Manager() :
    _theEngine( nullptr ),
    _theWindow( nullptr ),
    _theRenderer( nullptr ),
    _theInput( nullptr ),
    _theAudio( nullptr ),
    _contexts(),
    _fonts(),
    _theBuilder( new ObjectBuilder() ),
    _theSceneBuilder( new SceneBuilder() ),
    _theDialogBuilder( new DialogBuilder() ),
    _rawTextures(),
    _teamNames(),
    _resources( "global_resource_list" ),
    _scenes( "gloabl_scene_list" ),
    _title(),
    _defaultFont( nullptr ),
    _defaultColor( { 255, 255, 255, 255 } ),
    _eventStartIndex(0),
    _gameEvents(),
    _gravityConst( 0.0, 0.01 ),
    _dragConst( 0.005 )
  {
    // Set up the provided factories
    _theBuilder->addFactory( new SpriteFactory() );
    _theBuilder->addFactory( new FPSStringFactory() );
    _theBuilder->addFactory( new ButtonFactory() );

    // Set up the scene factories
    _theSceneBuilder->addFactory( new TitleSceneFactory() );
    _theSceneBuilder->addFactory( new PlatformerSceneFactory() );

    // Set up the dialog factories
    _theDialogBuilder->addFactory( new MenuDialogFactory() );

    // Create the default teams
    _teamNames[ "hud" ] = DEFAULT_TEAM_HUD;
    _teamNames[ "environment" ] = DEFAULT_TEAM_ENVIRONMENT;
    _teamNames[ "npc" ] = DEFAULT_TEAM_NPC;
    _teamNames[ "player" ] = DEFAULT_TEAM_PLAYER;
  }



  void Manager::openContext( Context* c )
  {
    DEBUG_LOG( "Opening Context" );
    
    _contexts.push_front( c );
    c->giveFocus();
  }


  void Manager::transferContext( Context* c )
  {
    DEBUG_LOG( "Transferring Context" );

    _contexts.front()->takeFocus();
    _contexts.pop_front();
    
    _contexts.push_front( c );
    c->giveFocus();
  }


  void Manager::closeContext()
  {
    DEBUG_LOG( "Closing Current Context" );
    
    _contexts.front()->takeFocus();
    _contexts.pop_front();
    
    if ( ! _contexts.empty() )
    {
      DEBUG_LOG( "Returning focus" );
      _contexts.front()->returnFocus();
    }
  }


//  void Manager::popContext()
//  {
//    DEBUG_LOG( "Popping Context" );
//    // Remove the top element
//    _contexts.pop_front();
//
//    // If there's any left, tell them focus has returned
//    if ( ! _contexts.empty() )
//    {
//      DEBUG_LOG( "Returning focus" );
//      _contexts.front()->returnFocus();
//    }
//  }


  Manager::~Manager()
  {

//    _defaultColor = nullptr;
    _defaultFont = nullptr;


    // Remove each of scenes and clear the vector
    INFO_LOG( "Deleteing Scenes" );
    _scenes.clear();


    INFO_LOG( "Deleting Resources" );
    _resources.clear();

    INFO_LOG( "Clearing team list" );
    _teamNames.clear();


    INFO_LOG( "Deleting raw texture data" );
    for ( RawTextureMap::iterator it = _rawTextures.begin(); it != _rawTextures.end(); ++it )
    {
      SDL_DestroyTexture( it->second.texture );
    }
    _rawTextures.clear();


    // Destroy the builder
    delete _theBuilder;
    _theBuilder = nullptr;

    // Destroy the scene builder
    delete _theSceneBuilder;
    _theSceneBuilder = nullptr;


    // Remove each of the fonts and clear the map
    for ( FontMap::iterator it = _fonts.begin(); it != _fonts.end(); ++it )
    {
      TTF_CloseFont( it->second );
    }
    _fonts.clear();


    // Delete the Audio Manager
    delete _theAudio;
    _theAudio = nullptr;

    // Delete the input manager
    delete _theInput;
    _theInput = nullptr;


    // Remove the renderer object
    SDL_DestroyRenderer( _theRenderer );
    _theRenderer = nullptr;

    // Remove the window
    delete _theWindow;
    _theWindow = nullptr;


    // Remove the engine
    Engine::killInstance();
    _theEngine = nullptr;


    // Close the SDL subsystems
    TTF_Quit();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
  }


  void Manager::run()
  {
    if ( getNumberScenes() <= 0 )
    {
      FAILURE_LOG( "Scene list is empty - there are no scenes to render!" );
      Exception ex( "Manager::run()", "No scenes to render", false );
      throw ex;
    }
    // This function should be used to start the "story" class running
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


  Scene* Manager::getScene( size_t scene_num )
  {
    return _scenes[ scene_num ];
  }


  void Manager::loadScene( size_t scene_num )
  {
    DEBUG_STREAM << "Load Scene: " << scene_num;

    DEBUG_STREAM << "Closing " << _contexts.size() << " open contexts";
    while ( ! _contexts.empty() )
    {
      closeContext();
    }

    DEBUG_LOG( "Loading new scene" );
    Scene* theScene = _scenes[ scene_num ];
    _contexts.push_front( theScene );
    _theEngine->setScene( theScene );
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Return existing texture pointer

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

