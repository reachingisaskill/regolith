
#include "Managers/Engine.h"
#include "Managers/Manager.h"

#include "logtastic.h"


namespace Regolith
{

  Engine::Engine( Window& win, InputManager& input, SDL_Color& color ) :
    _theRenderer( nullptr ),
    _window( win ),
    _inputManager( input ),
    _defaultColor( color ),
    _contextStack()
    _frameTimer(),
    _quit( false ),
    _pause( false )
  {
  }


  Engine::~Engine()
  {
  }


  void Engine::run()
  {
    // Reset the flags
    _quit = false;
    _pause = false;

    // Reset the timer
    _frameTimer.lap();

    // Load the first context
    performStackOperations();

    while ( ! _quit )
    {
      // Handle global events with no context
      _inputManager->handleEvents( nullptr );

      while ( ! _pause )
      {
        DEBUG_LOG( "RENDER LOOP START" );

        // Handle events globally and context-specific actions using the contexts input handler
        _inputManager->handleEvents( _contextStack.front()->inputHandler() );


        SDL_SetRenderDrawColor( _theRenderer, _defaultColor.r, _defaultColor.g, _defaultColor.b, _defaultColor.a );
        SDL_RenderClear( _theRenderer );

        float time = (float)_frameTimer.lap(); // Only conversion from int to float happens here.


        DEBUG_STREAM << " CONTEXT STACK SIZE : " << _contextStack.size();
        for ( ContextStack::reverse_iterator context_it = _visibleStackStart; context_it != _visibleStackEnd; ++context_it )
        {
          (*context_it)->update( time );
          (*context_it)->step( time );
          (*context_it)->resolveCollisions();
          (*context_it)->render();
        }


        DEBUG_LOG( "RENDER" );
        SDL_RenderPresent( _theRenderer );


        // Stack operations must happen separately to the update loop so that the context stack pointers are never invalidated.
        if ( ! _contextStackOperationQueue.empty() )
        {
          performStackOperations();
        }

       }

      // Impossible to continue if there are not contexts
      if ( _contextStack.empty() )
      {
        INFO_LOG( "Context stack is empty. Quitting." );
        _pause = true;
        _quit = true;
      }
    }
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
        _pause = true;
        _quit = true;
        break;

      case REGOLITH_EVENT_ENGINE_PAUSE :
        INFO_LOG( "Pausing Engine" );
        _pause = 1;
        break;

      case REGOLITH_EVENT_ENGINE_RESUME :
        INFO_LOG( "Resuming Engine" );
        _pause = false;
        break;

      default :
        break;
    }
  }


  void Engine::performStackOperations()
  {
    StackOperationQueue::iterator end = _stackOperationQueue.end();
    for ( StackOperationQueue::iterator it = _stackOperationQueue.begin(); it != end; ++it )
    {
      switch ( it->operation )
      {
        case StackOperation::PUSH :
          DEBUG_LOG( "Opening New Context" );
          if ( ! _contextStack.empty() )
          {
            _contextStack.front()->pauseContext();
          }
          _contextStack.push_front( it->context );
          _contextStack.front()->startContext();
          break;

        case StackOperation::POP :
          DEBUG_LOG( "Closing Current Context" );
          if ( ! _contextStack.empty() )
          {
            _contextStack.front()->stopContext();
            _contextStack.pop_front();
            if ( ! _contextStack.empty() )
            {
              DEBUG_LOG( "Returning focus" );
              _contextStack.front()->resumeContext();
            }
          }
          break;

        case StackOperation::RESET :
          DEBUG_LOG( "Closing All Contexts" );
          while ( ! _contextStack.empty() )
          {
            _contextStack.front()->resumeContext();
            _contextStack.front()->stopContext();
            _contextStack.pop_front();
          }
          DEBUG_LOG( "Loading new base context" );
          _contextStack.push_front( it->context );
          _contextStack.front()->startContext();
          break;

        case StackOperation::TRANSFER :
          DEBUG_LOG( "Transferring Context" );
          if ( ! _contextStack.empty() )
          {
            _contextStack.front()->stopContext();
            _contextStack.pop_front();
          }
          _contextStack.push_front( c );
          _contextStack.front()->startContext();
          break;
      }
    }

    // Set the visiblility start pointer. 
    // Start at the end and work backwards until either:
    //  - We hit the beginning of the stack, OR
    //  - One of the contexts overrides all the previous ones.
    _visibleStackStart = _contextStack.rend();
    while ( ( _visibleStackStart != _contextStack.begin() ) && ( ! (*_visibleStackStart)->overridesPreviousContext() ) )
      --_visibleStackStart();

    // Set the end iterator
    _visibleStackEnd = _contextStack.rend();

    if ( _contextStack.empty() )
    {
      quit();
    }
  }

}

