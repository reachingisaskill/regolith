
#include "Regolith/Components/Engine.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Managers/DataHandler.h"

#include "logtastic.h"


namespace Regolith
{

  // The thread that runs update/step/collisions etc.
  // Primary thread is for rendering ONLY
  void engineProcessingThread( Engine& );

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Engine member function definitions

  Engine::Engine( InputManager& input, SDL_Color& color ) :
    _theRenderer( nullptr ),
    _inputManager( input ),
    _defaultColor( color ),
    _contextStack(),
    _frameTimer(),
    _pause( false ),
    _textureRenderMutex(),
    _queueRenderRate( 1 ),
    _currentDataHandler( nullptr )
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

    // Synchronise the start of the frame
    Condition<bool>& frameSync = Manager::getInstance()->getThreadManager().FrameSynchronisation;
    // Synchronise access to the contexts
    Condition<bool>& renderSync = Manager::getInstance()->getThreadManager().RenderSynchronisation;

    std::unique_lock<std::mutex> frameLock( frameSync.mutex, std::defer_lock );
    std::unique_lock<std::mutex> renderLock( renderSync.mutex, std::defer_lock );
    std::unique_lock<std::mutex> queueLock( _renderQueueMutex, std::defer_lock );


    while ( ! quitFlag )
    {
      try
      {
        while ( ! _pause )
        {
          // Notify the start of the frame
          frameLock.lock();
          frameSync.data = true;
          frameSync.variable.notify_all();
          frameLock.unlock();

          // Acquire the render lock to stop other threads changing the context stack while it is being rendered.
          renderLock.lock();


          // Setup the rendering process
          SDL_SetRenderDrawColor( _theRenderer, _defaultColor.r, _defaultColor.g, _defaultColor.b, _defaultColor.a );
          SDL_RenderClear( _theRenderer );

          // Iterate through all the visible contexts and update as necessary
          for ( ContextStack::reverse_iterator context_it = _visibleStackStart; context_it != _visibleStackEnd; ++context_it )
          {
            // Draw everything to the back buffer
            (*context_it)->render();
          }

          // Blits the back buffer to the front buffer
          SDL_RenderPresent( _theRenderer );



          // Notify that the contexts can be updated
          renderSync.data = true;
          renderSync.variable.notify_all();
          renderLock.unlock();


          if ( queueLock.owns_lock() || queueLock.try_lock() )
          {
            if ( _currentDataHandler != nullptr )
            {
              // Render some of the texture queue for the remaing frame time
              unsigned int counter = 0;
              while ( ( counter < _queueRenderRate ) )
              {
                // Check if there are any surfaces to render
                RawTexture* rawTexture = _currentDataHandler->popRenderTexture();
                if ( rawTexture != nullptr )
                {
                  // Perform the rendering
                  rawTexture.renderTexture( _theRenderer );
                }
                else
                {
                  // Remove the DataHandler pointer
                  _currentDataHandler = nullptr;
                  queueLock.unlock();
                }
              }
            }
            else
            {
              queueLock.unlock();
            }
          }

        }
      }
      catch ( Exception& ex )
      {
        Manager::getInstance()->getThreadManager().error();
        if ( frameLock.owns_lock() )
        {
          frameLock.unlock();
        }
        if ( renderLock.owns_lock() )
        {
          renderLock.unlock();
        }
        FAILURE_LOG( "Regolith Exception thrown from Engine Rendering Thread." );
        std::cerr << ex.elucidate();
      }
      catch ( std::exception& ex )
      {
        Manager::getInstance()->getThreadManager().error();
        if ( frameLock.owns_lock() )
        {
          frameLock.unlock();
        }
        if ( renderLock.owns_lock() )
        {
          renderLock.unlock();
        }
        FAILURE_LOG( "Standard Exception thrown from Engine Processing Thread." );
        std::cerr << ex.what();
      }
    }
  }


  void Engine::renderTextures( DataHandler* handler )
  {
    // Block current thread while the an existing DataHandler is rendered
    UniqueLock lock( _renderQueueMutex, std::defer_lock );
    while ( true )
    {
      lock.lock();
      if ( _currentDataHandler == nullptr )
      {
        break;
      }
      lock.unlock();

      std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
    }

    _currentDataHandler = handler;

    lock.unlock();
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


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Engine processing thread definition

  void engineProcessingThread()
  {
    INFO_LOG( "Engine processing thread start." );

    std::atomic<bool>& quitFlag = Manager::getInstance()->getThreadManager().QuitFlag;

    INFO_LOG( "Engine processing thread initialised and waiting to start" );
    {
      Condition<bool>& startCondition = Manager::getInstance()->getThreadManager().StartCondition;
      std::unique_lock<std::mutex> lk( startCondition.mutex );
      startCondition.variable.wait( lk, [&]()->bool{ return quitFlag || startCondition.data; } );
      lk.unlock();
    }
    INFO_LOG( "Engine processing thread go." );


    Engine& engine = Manager::getInstance()->getEngine();

    // Synchronise the start of the frame
    Condition<bool>& frameSync = Manager::getInstance()->getThreadManager().FrameSynchronisation;
    std::unique_lock<std::mutex> frameLock( frameSync.mutex );

    // Synchronise access to the contexts
    Condition<bool>& renderSync = Manager::getInstance()->getThreadManager().RenderSynchronisation;
    std::unique_lock<std::mutex> renderLock( renderSync.mutex );



    // Reset the timer
    engine._frameTimer.lap();

    // Load the first context while the renderLock is held
    engine.performStackOperations();
    renderLock.unlock();



    try
    {

      INFO_LOG( "Engine processing thread waiting for first command" );
      while ( ! quitFlag )
      {
        // Handle global events with no context
        engine._inputManager.handleEvents( nullptr );

        while ( ! _pause )
        {
          frameSync.variable.wait( frameLock, [&]()->bool{ return quitFlag || frameSync.data; } );
          if ( quitFlag ) break;
          frameLock.unlock();


          // Handle events globally and context-specific actions using the contexts input handler
          engine._inputManager.handleEvents( _contextStack.front()->inputHandler() );



          renderLock.lock();
          renderSync.variable.wait( renderLock, [&]()->bool{ return quitFlag || renderSync.data; } );
          if ( quitFlag ) break;
          renderLock.unlock();



          float time = (float)engine._frameTimer.lap(); // Only conversion from int to float happens here.

          // Iterate through all the visible contexts and update as necessary
          for ( ContextStack::reverse_iterator context_it = engine._visibleStackStart; context_it != engine._visibleStackEnd; ++context_it )
          {
            Context* this_context = (*context_it);
            if ( ! this_context->isPaused() )
            {
              this_context->update( time );
              this_context->step( time );
              this_context->resolveCollisions();
            }
          }



          // Stack operations must happen separately to the update loop so that the context stack pointers are never invalidated.
          if ( ! engine._stackOperationQueue.empty() )
          {
            engine.performStackOperations();
          }



          frameLock.lock();
        }
      }

      frameLock.unlock();
    }
    catch( Exception& ex )
    {
      Manager::getInstance()->getThreadManager().error();
      if ( frameLock.owns_lock() )
      {
        frameLock.unlock();
      }
      if ( renderLock.owns_lock() )
      {
        renderLock.unlock();
      }
      FAILURE_LOG( "Regolith Exception thrown from Engine Processing Thread." );
      std::cerr << ex.elucidate();
    }
    catch( std::exception& ex )
    {
      Manager::getInstance()->getThreadManager().error();
      if ( frameLock.owns_lock() )
      {
        frameLock.unlock();
      }
      if ( renderLock.owns_lock() )
      {
        renderLock.unlock();
      }
      FAILURE_LOG( "Standard Exception thrown from Engine Processing Thread." );
      std::cerr << ex.what();
    }

    INFO_LOG( "Engine Processing thread stopped." );
  }

}

