
#include "Engine.h"
#include "Manager.h"

#include "logtastic.h"


namespace Regolith
{

  Engine::Engine() :
    _theRenderer( nullptr ),
    _currentWindow( nullptr ),
    _currentScene( nullptr ),
    _frameTimer(),
    _defaultColor( { 255, 255, 255, 255 } ),
    _quit( false )
  {
  }


  Engine::~Engine()
  {
  }


  void Engine::configure( SDL_Renderer* r, Window* w, InputManager* m )
  {
    _theRenderer = r;
    _currentWindow = w;
    _inputManager = m;
  }


  void Engine::run()
  {
    _frameTimer.lap();
//    int count = 0;

    while ( ! _quit )
    {
      // Handle events globally and context-specific actions using the scene's input handler
      _inputManager->handleEvents( _currentScene->inputHandler() );

      SDL_SetRenderDrawColor( _theRenderer, _defaultColor.r, _defaultColor.g, _defaultColor.b, _defaultColor.a );
      SDL_RenderClear( _theRenderer );

      Uint32 time = _frameTimer.lap();

      _currentScene->update( time );

      _currentScene->resolveCollisions();

      _currentScene->render();

      SDL_RenderPresent( _theRenderer );
//      if ( count++ > 100 )
//      {
//        _quit = true;
//      }
    }
  }


  void Engine::setScene( Scene* scene )
  {
    _currentScene = scene;
    _currentScene->giveFocus(); // This scene is now the current context for input and events
  }


  void Engine::registerEvents( InputManager* manager )
  {
    manager->registerInputRequest( this, INPUT_EVENT_QUIT );
  }


  void Engine::eventAction( const InputEvent& event )
  {
    switch( event )
    {
      case INPUT_EVENT_QUIT :
        _quit = true;
        break;

      default :
        break;
    }
  }

}

