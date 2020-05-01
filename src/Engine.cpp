
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
    _quit( false ),
    _status( 0 )
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

    ContextStack& contexts = Manager::getInstance()->contextStack();
    ContextStack::reverse_iterator contexts_end;

    while ( ! _quit )
    {
      _inputManager->handleEvents( nullptr );

      while ( ! _status )
      {

        // Handle events globally and context-specific actions using the scene's input handler
        _inputManager->handleEvents( contexts.front()->inputHandler() );

        SDL_SetRenderDrawColor( _theRenderer, _defaultColor.r, _defaultColor.g, _defaultColor.b, _defaultColor.a );
        SDL_RenderClear( _theRenderer );

        Uint32 time = _frameTimer.lap();

        contexts_end = contexts.rend();
        for ( ContextStack::reverse_iterator context_it = contexts.rbegin(); context_it != contexts_end; ++context_it )
        {
          if ( (*context_it)->isAnimated() )
          {
            (*context_it)->update( time );
          }

          if ( (*context_it)->isVisible() )
          {
            (*context_it)->render();
          }
        }

        DEBUG_LOG( "RENDER" );
        SDL_RenderPresent( _theRenderer );
  //      if ( count++ > 100 )
  //      {
  //        _quit = true;
  //      }
       }


      // Story interactions go here!
    }
  }


  void Engine::setScene( Scene* scene )
  {
    _currentScene = scene;
    _currentScene->giveFocus(); // This scene is now the current context for input and events
    _currentScene->start();
  }


  void Engine::registerEvents( InputManager* manager )
  {
    manager->registerInputRequest( this, REGOLITH_EVENT_QUIT );
    manager->registerInputRequest( this, REGOLITH_EVENT_ENGINE_PAUSE );
    manager->registerInputRequest( this, REGOLITH_EVENT_ENGINE_RESUME );
  }


  void Engine::eventAction( const RegolithEvent& event, const SDL_Event& )
  {
    switch( event )
    {
      case REGOLITH_EVENT_QUIT :
        _status = 1;
        _quit = true;
        break;

      case REGOLITH_EVENT_ENGINE_PAUSE :
        INFO_LOG( "Pausing Engine" );
        _status = 1;
        break;

      case REGOLITH_EVENT_ENGINE_RESUME :
        INFO_LOG( "Resuming Engine" );
        _status = 0;
        break;

      default :
        break;
    }
  }

}

