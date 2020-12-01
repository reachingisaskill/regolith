
#include "Regolith/Components/Engine.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Managers/ThreadHandler.h"


namespace Regolith
{

  // The thread that runs update/step/collisions etc.
  // Primary thread is for rendering ONLY
  void engineProcessingThread();

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Engine member function definitions

  Engine::Engine( InputManager& input ) :
    _inputManager( input ),
    _contextStack(),
    _openContext( nullptr ),
    _openContextGroup( nullptr ),
    _frameTimer(),
    _pause( false ),
    _renderQueueMutex(),
    _queueRenderRate( 1 ),
    _currentDataHandler( nullptr )
  {
  }


  Engine::~Engine()
  {
  }

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Running control functions

  void Engine::run()
  {
    // Reset the flags
    std::atomic<bool>& quitFlag = Manager::getInstance()->getThreadManager().QuitFlag;
    _pause = false;

    // Synchronise access to the contexts
    std::unique_lock<std::mutex> renderLock( Manager::getInstance()->getThreadManager().RenderMutex );


    DEBUG_LOG( "Engine::run : Engine now running" );

    try
    {

      // Handler global events without a context.
      _inputManager.handleEvents( nullptr );

      // Reset the frame timer before the loop starts
      _frameTimer.lap();
      _frameTimer.resetFPSCount();

      while ( performStackOperations() )
      {
        // If there's an error in another thread, we abandon ship
        if ( quitFlag ) break;

        // Release the context stack
        renderLock.unlock();


        DEBUG_LOG( "Engine::run : ------ EVENTS   ------" );
        // Handle events globally and context-specific actions using the contexts input handler
        _inputManager.handleEvents( _contextStack.front()->inputHandler() );


        // Stop updating things while we're paused
        // Doesn't block the rendering thread so we can still load stuff and draw the window.
        while ( _pause )
        {
          // Put it to sleep. No point doing nothing really fast!
          std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );

          // Handler global events without a context. Required to be able to leave the pause state.
          _inputManager.handleEvents( nullptr );

          // Reset the timer while paused
          _frameTimer.lap();
        }


        // Lock access to the context stack for updating
#ifdef REGOLITH_VALGRIND_BUILD
        std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
        renderLock.lock();
#else
        while( ! renderLock.try_lock() );
#endif

        DEBUG_LOG( "Engine::run : ------ CONTEXTS ------" );
        float time = _frameTimer.lap(); // Only conversion from int to float happens here.

        // Iterate through all the visible contexts and update as necessary
        for ( ContextStack::reverse_iterator context_it = _visibleStackStart; context_it != _visibleStackEnd; ++context_it )
        {
          Context* this_context = (*context_it);
          if ( ! this_context->isPaused() )
          {
            this_context->update( time );
//            this_context->step( time );
//            this_context->resolveCollisions();
          }
        }
      }

      // Release the context stack
      renderLock.unlock();
    }
    catch ( Exception& ex )
    {
      Manager::getInstance()->getThreadManager().error();
      if ( renderLock.owns_lock() )
      {
        renderLock.unlock();
      }
      FAILURE_LOG( "Engine::run : Regolith Exception thrown from Engine Rendering Thread." );
      std::cerr << ex.elucidate();
    }
    catch ( std::exception& ex )
    {
      Manager::getInstance()->getThreadManager().error();
      if ( renderLock.owns_lock() )
      {
        renderLock.unlock();
      }
      FAILURE_LOG( "Engine::run : Standard Exception thrown from Engine Processing Thread." );
      std::cerr << ex.what();
    }
  }


  bool Engine::performStackOperations()
  {
    // Flag that we need to update the visible stack pointers
    bool modified = false;


    while ( ( ! _contextStack.empty() ) && _contextStack.front()->closed() )
    {
      modified = true;

      DEBUG_LOG( "Engine::performStackOperations : Popping closed context" );
      // Pop the context
      _contextStack.pop_front();
    }


    // If the stack is empty and a new group is queued, we load that.
    if ( _contextStack.empty() && _openContextGroup != nullptr )
    {
      modified = true;

      DEBUG_LOG( "Engine::performStackOperations : Push context from new context group" );
      // Push context pointer onto the empty stack
      _contextStack.push_front( _openContextGroup );
      _openContextGroup = nullptr;

      // Trigger the on start hooks
      _contextStack.front()->startContext();
    }


    // This is a loop because contexts can open children as soon as they are started.
    while ( _openContext != nullptr )
    {
      modified = true;

      DEBUG_LOG( "Engine::performStackOperations : Pushing new context" );
      // Push onto stack
      _contextStack.push_front( _openContext );
      _openContext = nullptr;
      // Trigger the start hooks
      _contextStack.front()->startContext();
    }


    if ( ! _contextStack.empty() )
    {
      if ( modified ) // Reset the visible stack pointers
      {
        DEBUG_LOG( "Engine::performStackOperations : Resetting visible stack iterators" );
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

        DEBUG_STREAM << "Engine::performStackOperations : FPS for previous context stack: AVG = " << _frameTimer.getAvgFPS() << " MIN = " << _frameTimer.getMinFPS() << " MAX = " << _frameTimer.getMaxFPS();
        _frameTimer.resetFPSCount();
      }

      return true;
    }
    else // Stack is empty! Time to abandon ship
    {
      DEBUG_LOG( "Engine::perfornStackOperations : Context stack is empty. Closing engine." );
      DEBUG_STREAM << "Engine::performStackOperations : FPS for previous context stack: AVG = " << _frameTimer.getAvgFPS() << " MIN = " << _frameTimer.getMinFPS() << " MAX = " << _frameTimer.getMaxFPS();
      _frameTimer.resetFPSCount();

      // Reset these anyway so the rendering thread doesnt fall over.
      _visibleStackStart = _contextStack.rbegin();
      _visibleStackEnd = _contextStack.rend();
      return false;
    }

  }


////////////////////////////////////////////////////////////////////////////////////////////////////V
  // Context and context group opreations

  // Tells the engine to push the context pointer to the top of the stack
  void Engine::openContext( Context* c )
  {
    _openContext = c;
  }


  // Tells the engine that this is the new context group entry point. Current stack MUST close itself!
  void Engine::openContextGroup( Context* c )
  {
    // Set the pointer
    _openContextGroup = c;

    // Tell everything that is running to stop
    for ( ContextStack::iterator it = _contextStack.begin(); it != _contextStack.end(); ++it )
    {
      (*it)->stopContext();
    }

    // Opening a context group takes priority so clear up this
    _openContext = nullptr;

  }


////////////////////////////////////////////////////////////////////////////////////////////////////V
  // Useful functions for external communication

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

    DEBUG_STREAM << "Engine::renderTextures : New data handler @ " << handler;
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
        _pause = false;
//        Manager::getInstance()->quit();
        for ( ContextStack::iterator it = _contextStack.begin(); it != _contextStack.end(); ++it )
        {
          (*it)->stopContext();
        }
        break;

      case REGOLITH_EVENT_ENGINE_PAUSE :
        INFO_LOG( "Engine::eventAction : Pausing Engine" );
        _pause = true;
        break;

      case REGOLITH_EVENT_ENGINE_RESUME :
        INFO_LOG( "Engine::eventAction : Resuming Engine" );
        _pause = false;
        break;

      default :
        break;
    }
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Engine processing thread definition

  void engineRenderingThread()
  {
    ThreadHandler threadHandler( "EngineRenderingThread", REGOLITH_THREAD_RENDERING );

    // Wait on the start condition
    threadHandler.start();

    // Get a reference to the engine
    Engine& engine = Manager::getInstance()->getEngine();

    // Get references to the required components
    Camera& camera = Manager::getInstance()->requestCamera();
    ContextStack::reverse_iterator& visibleStackStart = engine._visibleStackStart;
    ContextStack::reverse_iterator& visibleStackEnd = engine._visibleStackEnd;
    DataHandler*& currentDataHandler = engine._currentDataHandler;
    unsigned int& queueRenderRate = engine._queueRenderRate;

    // Control access to the contexts
    std::unique_lock<std::mutex> renderLock( Manager::getInstance()->getThreadManager().RenderMutex, std::defer_lock );

    // Control access to the DataHandler being loaded
    std::unique_lock<std::mutex> queueLock( engine._renderQueueMutex, std::defer_lock );


    // Update the thread status
    threadHandler.running();

    try
    {

      while ( threadHandler.isGood() )
      {
        // Acquire the render lock to stop other threads changing the context stack while it is being rendered.
#ifdef REGOLITH_VALGRIND_BUILD
        std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
        renderLock.lock();
#else
        while( ! renderLock.try_lock() );
#endif

        DEBUG_LOG( "engineRenderingThread : ------ RENDER ------" );

        // Setup the rendering process
        camera.resetRender();

        // Iterate through all the visible contexts and update as necessary
        for ( ContextStack::reverse_iterator context_it = visibleStackStart; context_it != visibleStackEnd; ++context_it )
        {
          // Draw everything to the back buffer
          (*context_it)->render( camera );
        }

        // Blits the back buffer to the front buffer synchronised with monitor VSYNC
        camera.draw();

        // Release acces to the context stack
        renderLock.unlock();


        DEBUG_LOG( "engineRenderingThread : ------ FRAME ------" );


        if ( queueLock.owns_lock() || queueLock.try_lock() )
        {
          if ( currentDataHandler != nullptr )
          {
            // Render some of the texture queue for the remaing frame time
            unsigned int counter = 0;
            while ( counter < queueRenderRate )
            {
              // Check if there are any surfaces to render
              DEBUG_STREAM << "engineRenderingThread : ------ TexRend @ " << currentDataHandler;
              RawTexture* rawTexture = currentDataHandler->popRenderTexture();
              if ( rawTexture != nullptr )
              {
                // Perform the rendering
                camera.renderRawTexture( rawTexture );
              }
              else
              {
                // Remove the DataHandler pointer
                currentDataHandler = nullptr;
                queueLock.unlock();
                break;
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
    catch( Exception& ex )
    {
      if ( renderLock.owns_lock() )
      {
        renderLock.unlock();
      }
      if ( queueLock.owns_lock() )
      {
        queueLock.unlock();
      }
      threadHandler.throwError( ex );
      return;
    }
    catch( std::exception& ex )
    {
      if ( renderLock.owns_lock() )
      {
        renderLock.unlock();
      }
      if ( queueLock.owns_lock() )
      {
        queueLock.unlock();
      }
      threadHandler.throwError( ex );
      return;
    }

    threadHandler.closing();


    // Can only delete the SDL_Renderer once other threads have finished using it
    threadHandler.waitStatus( REGOLITH_THREAD_DATA, THREAD_CLOSING );
    threadHandler.waitStatus( REGOLITH_THREAD_CONTEXT, THREAD_CLOSING );
    INFO_LOG( "EngineRenderingThread : Clearing camera!" );
    camera.clear();

    threadHandler.stop();
  }

}

