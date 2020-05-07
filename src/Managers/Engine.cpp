
#include "Managers/Engine.h"
#include "Managers/Manager.h"

#include "logtastic.h"


namespace Regolith
{

  Engine::Engine() :
    _theRenderer( nullptr ),
    _currentWindow( nullptr ),
    _currentScene( nullptr ),
    _frameTimer(),
    _defaultColor( { 0, 0, 0, 255 } ),
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
        DEBUG_LOG( "RENDER LOOP START" );

        // Handle events globally and context-specific actions using the scene's input handler
        _inputManager->handleEvents( contexts.front()->inputHandler() );

        SDL_SetRenderDrawColor( _theRenderer, _defaultColor.r, _defaultColor.g, _defaultColor.b, _defaultColor.a );
        SDL_RenderClear( _theRenderer );

        Uint32 time = _frameTimer.lap();

        if ( ! contexts.empty() )
        {
          DEBUG_STREAM << " CONTEXT STACK SIZE : " << contexts.size();
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
        }
        else
        {
          WARN_LOG( "No contexts left on stack. Exiting" );
          _status = 1; // Can't continue with no contexts
        }

       }

      // Switch state to be used later. Useful for dropping in loading screens etc in a multithreaded engine
      switch ( _status )
      {
        default :
          break;
      }

      // Impossible to continue if there are not contexts
      if ( contexts.empty() )
      {
        INFO_LOG( "Context stack is empty. Quitting." );
        _status = 1;
        _quit = true;
      }
    }
  }


  void Engine::setScene( Scene* scene )
  {
    // If a scene is already loaded, end it
    if ( _currentScene != nullptr )
    {
      DEBUG_LOG( "Stopping current scene" );
      _currentScene->stop();
      _currentScene->takeFocus();
    }

    // Update the current scene pointer
    _currentScene = scene;

    // Call the scene startup function
    _currentScene->start();

    // Tell the scene it has focus
    // Pushes itself onto the context stack
    _currentScene->giveFocus();
  }


  void Engine::registerEvents( InputManager* manager )
  {
    manager->registerEventRequest( this, REGOLITH_EVENT_QUIT );
    manager->registerEventRequest( this, REGOLITH_EVENT_ENGINE_PAUSE );
    manager->registerEventRequest( this, REGOLITH_EVENT_ENGINE_RESUME );
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

