
#include "Engine.h"
#include "Manager.h"

#include "logtastic.h"


namespace Regolith
{

  Engine::Engine()
  {
  }


  Engine::~Engine()
  {
  }


  void Engine::configure( SDL_Renderer* r, Window* w )
  {
    _theRenderer = r;
    _currentWindow = w;
  }


  void Engine::run()
  {
    Manager* man = Manager::getInstance();
    _frameTimer.lap();

    bool quit = false;
    while ( ! quit )
    {
      int dx = 0;
      int dy = 0;

      SDL_Event e;
      while ( SDL_PollEvent( &e ) != 0 )
      {
        switch ( e.type )
        {
          case SDL_QUIT:
            quit = true;
            break;
          case SDL_WINDOWEVENT :
            _currentWindow->handleEvent( e );

            if ( e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED )
            {
              _currentCamera->updateScale( _currentWindow->getWidth(), _currentWindow->getHeight() );
            }
            break;
          case SDL_KEYDOWN :
            if ( e.key.keysym.sym == SDLK_RETURN )
            {
              man->raiseEvent( REGOLITH_FULLSCREEN );
            }
          case SDL_KEYUP :
            // Code for keyboard events
            break;

          case SDL_MOUSEBUTTONDOWN :
          case SDL_MOUSEBUTTONUP :
          case SDL_MOUSEMOTION :
          case SDL_MOUSEWHEEL :
            // Code for mouse events
            break;

          case SDL_USEREVENT:
            resolveUserEvent( (GameEvent)e.user.code );
            break;
        }

//        _currentScene->handleEvent( e );
      }

      const Uint8* keyStates = SDL_GetKeyboardState( nullptr );

      Uint32 time = _frameTimer.lap();

      if ( keyStates[ SDL_SCANCODE_RIGHT ] )
        dx += 1 * time;
      if ( keyStates[ SDL_SCANCODE_LEFT ] )
        dx -= 1 * time;
      if ( keyStates[ SDL_SCANCODE_DOWN ] )
        dy += 1 * time;
      if ( keyStates[ SDL_SCANCODE_UP ] )
        dy -= 1 * time;

      _currentCamera->move( dx, dy );

      _currentScene->update( time );

      _currentScene->resolveCollisions();


      SDL_SetRenderDrawColor( _theRenderer, _defaultColor.r, _defaultColor.g, _defaultColor.b, _defaultColor.a );
      SDL_RenderClear( _theRenderer );

      _currentScene->render();

      SDL_RenderPresent( _theRenderer );
    }
  }


  void Engine::setScene( Scene* scene )
  {
    _currentScene = scene;
//    _currentCamera = scene->getCamera();
  }


  void Engine::setCamera( Camera* camera )
  {
    _currentCamera = camera;
  }


  void Engine::resolveUserEvent( GameEvent event )
  {
    switch ( event )
    {
      case REGOLITH_SCENE_END :
        // Don't yet have a story class...
        break;
      case REGOLITH_FULLSCREEN :
        _currentWindow->toggleFullScreen();
        break;
      default:
        break;
    }
  }

}

