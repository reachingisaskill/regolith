
#include "Regolith/Managers/ThreadManager.h"


namespace Regolith
{

  // Static thread communication variables
  Condition<bool> ThreadManager::StartCondition( false );
  std::atomic<bool> ThreadManager::QuitFlag( false );
  std::atomic<bool> ThreadManager::ErrorFlag( false );


  // Static thread status variables
  Condition<ThreadStatus> ThreadManager::DataManagerStatus( ThreadStatus::Null );
  Condition<ThreadStatus> ThreadManager::ContextManagerStatus( ThreadStatus::Null );
  Condition<ThreadStatus> ThreadManager::EngineRenderingStatus( ThreadStatus::Null );


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
    // Make sure this is true if the thread manager is being destroyed!
    if ( QuitFlag == false )
    {
      this->stopAll();
    }

    // Wait for all the threads to re-join
    _dataManagerThread.join();
    _contextManagerThread.join();
    _engineRenderingThread.join();
  }


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
    ERROR_LOG( "Thread Manager Calling Quit. An error has occured." );
    // Set the atomic error and quit flags
    ErrorFlag = true;
    QuitFlag = true;

    // Notify all the conditions so that the threads see the flags
    DataManagerStatus.variable.notify_all();
    ContextManagerStatus.variable.notify_all();
    EngineRenderingStatus.variable.notify_all();

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


  void ThreadManager::waitThreadStatus( ThreadStatus status )
  {
    // Wait on the DataManager thread
    std::unique_lock<std::mutex> dataManagerLock( DataManagerStatus.mutex );
    DataManagerStatus.variable.wait( dataManagerLock, [&]()->bool{ return ErrorFlag || QuitFlag || (DataManagerStatus.data == status); } );
    dataManagerLock.unlock();

    // Wait on the ContextManager thread
    std::unique_lock<std::mutex> contextManagerLock( ContextManagerStatus.mutex );
    DataManagerStatus.variable.wait( contextManagerLock, [&]()->bool{ return ErrorFlag || QuitFlag || (ContextManagerStatus.data == status); } );
    contextManagerLock.unlock();

    // Wait on the Engine Rendering thread
    std::unique_lock<std::mutex> engineRenderingLock( EngineRenderingStatus.mutex );
    EngineRenderingStatus.variable.wait( engineRenderingLock, [&]()->bool{ return ErrorFlag || QuitFlag || (EngineRenderingStatus.data == status); } );
    engineRenderingLock.unlock();
  }

}

