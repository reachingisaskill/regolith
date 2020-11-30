
#include "Regolith/Managers/ThreadManager.h"
#include "Regolith/Managers/ThreadHandler.h"


namespace Regolith
{
////////////////////////////////////////////////////////////////////////////////////////////////////

  // Static thread communication variables
  Condition<bool> ThreadManager::StartCondition( false );
  std::atomic<bool> ThreadManager::QuitFlag( false );
  std::atomic<bool> ThreadManager::ErrorFlag( false );


////////////////////////////////////////////////////////////////////////////////////////////////////

  ThreadManager::ThreadManager() :
    _dataManagerThread(),
    _contextManagerThread(),
    _engineRenderingThread(),
    _threadStatus(),
    DataUpdate( false ),
    ContextUpdate( false ),
    MusicUpdate( nullptr ),
    RenderMutex()
  {
    GuardLock lk( _threadStatus.mutex );
    for ( char n = 0; n < REGOLITH_THREAD_TOTAL; ++n )
    {
      _threadStatus.data[ (ThreadName) n ] = THREAD_NULL;
    }
  }


  ThreadManager::~ThreadManager()
  {
    // All executing threads must have been stopped by the owner of this object!
    // Don't fuck this up or everything will explode!
  }


  void ThreadManager::setThreadStatus( ThreadName name, ThreadStatus status )
  {
    UniqueLock lk( _threadStatus.mutex );
    _threadStatus.data[name] = status;
    lk.unlock();

    _threadStatus.variable.notify_all();
  }


  void ThreadManager::waitThreadStatus( ThreadName name, ThreadStatus status )
  {
    UniqueLock lk( _threadStatus.mutex );

    if ( ErrorFlag || ( _threadStatus.data[name] >= status ) )
    {
      lk.unlock();
      return;
    }

    _threadStatus.variable.wait( lk, [&]()->bool{ return ErrorFlag || ( _threadStatus.data[name] >= status ); } );

    lk.unlock();
  }


  void ThreadManager::waitThreadStatus( ThreadStatus status )
  {
    UniqueLock lk( _threadStatus.mutex );

    for ( char n = 0; n < REGOLITH_THREAD_TOTAL; ++n )
    {
      if ( ErrorFlag )
      {
        lk.unlock();
        return;
      }

      if ( _threadStatus.data[ (ThreadName) n ] >= status )
      {
        continue;
      }

      _threadStatus.variable.wait( lk, [&]()->bool{ return ErrorFlag || ( _threadStatus.data[ (ThreadName) n ] >= status ); } );
    }

    lk.unlock();
  }


////////////////////////////////////////////////////////////////////////////////////////////////////

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


////////////////////////////////////////////////////////////////////////////////////////////////////

  void ThreadManager::startAll()
  {
    _dataManagerThread = std::thread( dataManagerLoadingThread );
    _contextManagerThread = std::thread( contextManagerLoadingThread );
    _engineRenderingThread = std::thread( engineRenderingThread );


    // Wait for all the threads to be in a Waiting state
    INFO_LOG( "ThreadManager::startAll : Waiting for threads to be intialised." );
    waitThreadStatus( THREAD_WAITING );

    if ( ErrorFlag )
    {
      Exception ex( "ThreadManager::startAll()", "Could not start threads. Error flag raised." );
      throw ex;
    }

    // Set the start condition under lock
    INFO_LOG( "ThreadManager::startAll : Setting the start condition variable" );
    {
      GuardLock lk( StartCondition.mutex );
      StartCondition.data = true;
    }

    // Notify the start and pause briefly. Seems to help the locks become aquired in a reliable way
    StartCondition.variable.notify_all();
    std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );

    INFO_LOG( "ThreadManager::startAll : Waiting for all threads to be in the running state" );
    // Only return once all the threads are running (or broken!)
    waitThreadStatus( THREAD_RUNNING );

    if ( ErrorFlag )
    {
      Exception ex( "ThreadManager::startAll()", "Threads not running. Error flag raised." );
      throw ex;
    }
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

    waitThreadStatus( THREAD_STOP );
  }


  void ThreadManager::join()
  {
    // Wait for all the threads to re-join
    INFO_LOG( "ThreadManager::~ThreadManager : Joining data manager thread" );
    _dataManagerThread.join();
    INFO_LOG( "ThreadManager::~ThreadManager : Joining context manager thread" );
    _contextManagerThread.join();
    INFO_LOG( "ThreadManager::~ThreadManager : Joining engine rendering thread" );
    _engineRenderingThread.join();
  }

}

