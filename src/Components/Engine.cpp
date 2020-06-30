
#include "Regolith/Components/Engine.h"
#include "Regolith/Managers/Manager.h"

#include "logtastic.h"


namespace Regolith
{

  Engine::Engine( InputManager& input, SDL_Color& color ) :
    _theRenderer( nullptr ),
    _inputManager( input ),
    _defaultColor( color ),
    _contextStack(),
    _frameTimer(),
    _pause( false )
  {
  }


  Engine::~Engine()
  {
  }


  void Engine::run()
  {
    if ( _theRenderer == nullptr )
    {
      Exception ex( "Engine::run()", "Renderer is not set" );
      throw ex;
    }

    // Reset the flags
    std::atomic<bool>& quitFlag = Manager::getInstance()->getThreadManager().QuitFlag;
    _pause = false;

    // Reset the timer
    _frameTimer.lap();

    // Load the first context
    performStackOperations();


    while ( ! quitFlag )
    {
      // Handle global events with no context
      _inputManager.handleEvents( nullptr );

      while ( ! _pause )
      {
        // Handle events globally and context-specific actions using the contexts input handler
        _inputManager.handleEvents( _contextStack.front()->inputHandler() );


        SDL_SetRenderDrawColor( _theRenderer, _defaultColor.r, _defaultColor.g, _defaultColor.b, _defaultColor.a );
        SDL_RenderClear( _theRenderer );

        float time = (float)_frameTimer.lap(); // Only conversion from int to float happens here.


        // Iterate through all the visible contexts and update as necessary
        for ( ContextStack::reverse_iterator context_it = _visibleStackStart; context_it != _visibleStackEnd; ++context_it )
        {
          Context* this_context = (*context_it);
          if ( ! this_context->isPaused() )
          {
            this_context->update( time );
            this_context->step( time );
            this_context->resolveCollisions();
          }
          // Draw everything to the back buffer
          this_context->render();
        }


        // Blits the back buffer to the front buffer
        SDL_RenderPresent( _theRenderer );


        // Stack operations must happen separately to the update loop so that the context stack pointers are never invalidated.
        if ( ! _stackOperationQueue.empty() )
        {
          performStackOperations();
        }
      }
    }
  }


  void Engine::registerEvents( InputManager& manager )
  {
    manager.registerEventRequest( this, REGOLITH_EVENT_QUIT );
    manager.registerEventRequest( this, REGOLITH_EVENT_ENGINE_PAUSE );
    manager.registerEventRequest( this, REGOLITH_EVENT_ENGINE_RESUME );
  }


  void Engine::eventAction( const RegolithEvent& event, const SDL_Event& )
  {
    switch( event )
    {
      case REGOLITH_EVENT_QUIT :
        _pause = true;
        Manager::getInstance()->quit();
        break;

      case REGOLITH_EVENT_ENGINE_PAUSE :
        INFO_LOG( "Pausing Engine" );
        _pause = true;
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
    while ( ! _stackOperationQueue.empty() )
    {
      DEBUG_STREAM << _stackOperationQueue.size() << " Operations Remaining";
      StackOperation& sop = _stackOperationQueue.front();

      switch ( sop.operation )
      {
        case StackOperation::PUSH :
          DEBUG_LOG( "Opening New Context" );
          if ( ! _contextStack.empty() )
          {
            _contextStack.front()->pauseContext();
          }
          _contextStack.push_front( sop.context );
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
          while ( ! _contextStack.empty() )
          {
            _contextStack.front()->resumeContext();
            _contextStack.front()->stopContext();
            _contextStack.pop_front();
          }
          DEBUG_LOG( "Loading new base context" );
          _contextStack.push_front( sop.context );
          _contextStack.front()->startContext();
          break;

        case StackOperation::TRANSFER :
          DEBUG_LOG( "Transferring Context" );
          if ( ! _contextStack.empty() )
          {
            _contextStack.front()->stopContext();
            _contextStack.pop_front();
          }
          _contextStack.push_front( sop.context );
          _contextStack.front()->startContext();
          break;
      }

      _stackOperationQueue.pop();
    }

    if ( _contextStack.empty() )
    {
      Manager::getInstance()->quit();
      return;
    }

    // Set the visiblility start pointer. 
    // Start at the end and work backwards until either:
    //  - We hit the beginning of the stack, OR
    //  - One of the contexts overrides all the previous ones.
    _visibleStackStart = --_contextStack.rend();
    while ( ( _visibleStackStart != _contextStack.rbegin() ) && ( ! (*_visibleStackStart)->overridesPreviousContext() ) )
    {
      --_visibleStackStart;
    }

    // Set the end iterator
    _visibleStackEnd = _contextStack.rend();
  }

}

