
#include "Engine.h"


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
    _frameTimer.lap();

    bool quit = false;
    while ( ! quit )
    {
      int dx = 0;
      int dy = 0;

      SDL_Event e;
      while ( SDL_PollEvent( &e ) != 0 )
      {
        if ( e.type == SDL_QUIT )
          quit = true;

        _currentWindow->handleEvent( e );
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




      SDL_SetRenderDrawColor( _theRenderer, _defaultColor.r, _defaultColor.g, _defaultColor.b, _defaultColor.a );
      SDL_RenderClear( _theRenderer );


      _currentScene->render();


      SDL_RenderPresent( _theRenderer );
    }
  }


  void Engine::loadScene( Scene* scene )
  {
    scene->load();
    _currentScene = scene;
    _currentCamera = scene->getCamera();
  }

}

