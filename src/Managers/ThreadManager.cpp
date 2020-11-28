
#include "Regolith/Managers/ThreadManager.h"
#include "Regolith/Managers/ThreadHandler.h"


namespace Regolith
{
////////////////////////////////////////////////////////////////////////////////////////////////////

  // Static thread communication variables
  Condition<bool> ThreadManager::StartCondition( false );
  Condition<bool> ThreadManager::StopCondition( false );
  std::atomic<bool> ThreadManager::QuitFlag( false );
  std::atomic<bool> ThreadManager::ErrorFlag( false );


//  // Static thread status variables
//  Condition<ThreadStatus> ThreadManager::DataManagerStatus( ThreadStatus::Null );
//  Condition<ThreadStatus> ThreadManager::ContextManagerStatus( ThreadStatus::Null );
//  Condition<ThreadStatus> ThreadManager::EngineRenderingStatus( ThreadStatus::Null );


////////////////////////////////////////////////////////////////////////////////////////////////////

  ThreadManager::ThreadManager() :
    _dataManagerThread( dataManagerLoadingThread ),
    _contextManagerThread( contextManagerLoadingThread ),
    _engineRenderingThread( engineRenderingThread ),
    DataUpdate( false ),
    ContextUpdate( false ),
    MusicUpdate( nullptr ),
    RenderMutex()
  {
  }


  ThreadManager::~ThreadManager()
  {
    // All executing threads must have been stopped by the owner of this object!
    // Don't fuck this up or everything will explode!
  }


  void ThreadManager::registerThreadHandler( ThreadHandler* handler )
  {
    std::lock_guard<std::mutex> lk( _handlerSetMutex );
    _threadHandlers.insert( handler );
  }


  void ThreadManager::removeThreadHandler( ThreadHandler* handler )
  {
    std::lock_guard<std::mutex> lk( _handlerSetMutex );
    _threadHandlers.erase( handler );
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Functions to signal the changes of states

//  void ThreadManager::quit()
//  {
//    INFO_LOG( "Thread Manager Calling Quit" );
//    QuitFlag = true;
//
//    StartCondition.variable.notify_all();
//    DataUpdate.variable.notify_all();
//    ContextUpdate.variable.notify_all();
//    MusicUpdate.variable.notify_all();
//  }


  void ThreadManager::error()
  {
    ERROR_LOG( "ThreadManager::error : Calling Quit. An error has occured." );
    // Set the atomic error and quit flags
    ErrorFlag = true;
    QuitFlag = true;

    // Notify all the conditions so that the threads see the flags
    StartCondition.variable.notify_all();
    DataUpdate.variable.notify_all();
    ContextUpdate.variable.notify_all();
    MusicUpdate.variable.notify_all();
  }


  void ThreadManager::startAll()
  {
    // Wait for all the threads to be in a Waiting state
    INFO_LOG( "ThreadManager::startAll : Waiting for threads to be intialised." );
    waitThreadStatus( ThreadStatus::Waiting );

    if ( ErrorFlag )
    {
      Exception ex( "ThreadManager::startAll()", "Could not start threads. Error flag raised." );
      throw ex;
    }

    // Set the start condition under lock
    INFO_LOG( "ThreadManager::startAll : Setting the start condition variable" );
    {
      std::lock_guard<std::mutex> lk( StartCondition.mutex );
      StartCondition.data = true;
    }

    // Notify the start and pause briefly. Seems to help the locks become aquired in a reliable way
    StartCondition.variable.notify_all();
    std::this_thread::sleep_for( std::chrono::milliseconds( 5 ) );

    INFO_LOG( "ThreadManager::startAll : Waiting for all threads to be in the running state" );
    // Only return once all the threads are running (or broken!)
    waitThreadStatus( ThreadStatus::Running );
  }


  void ThreadManager::stopAll()
  {
    // Set the atomic quit flag
    QuitFlag = true;

    // Notify all the conditions so that the threads see the quit flag
    StartCondition.variable.notify_all();
    DataUpdate.variable.notify_all();
    ContextUpdate.variable.notify_all();
    MusicUpdate.variable.notify_all();
  }


  void ThreadManager::closeAll()
  {
    // Wait until the threads have finished their post-execution operations
    waitThreadStatus( ThreadStatus::Stop );

    // Set the stop condition under lock
    INFO_LOG( "ThreadManager::closeAll : Setting the stop condition variable" );
    {
      std::lock_guard<std::mutex> lk( StopCondition.mutex );
      StopCondition.data = true;
    }
    // Notify all the waiting threads
    StopCondition.variable.notify_all();
    // Threads can now be safely joined
  }


  void ThreadManager::join()
  {
    if ( ! QuitFlag )
    {
      this->stopAll();
      this->closeAll();
    }

    // Wait for all the threads to re-join
    INFO_LOG( "ThreadManager::~ThreadManager : Joining data manager thread" );
    _dataManagerThread.join();
    INFO_LOG( "ThreadManager::~ThreadManager : Joining context manager thread" );
    _contextManagerThread.join();
    INFO_LOG( "ThreadManager::~ThreadManager : Joining engine rendering thread" );
    _engineRenderingThread.join();
  }


  void ThreadManager::waitThreadStatus( ThreadStatus status )
  {
    // Aquire the handler mutex
    std::unique_lock<std::mutex> lock( _handlerSetMutex );

    // Iterate through the handlers check them individually
    for ( HandlerSet::iterator it = _threadHandlers.begin(); it != _threadHandlers.end(); ++it )
    {
      (*it)->waitStatus( status );
    }
  }

}

