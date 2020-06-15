
#include "Regolith/Managers/ThreadManager.h"

#include "logtastic.h"


namespace Regolith
{

  Condition<bool> ThreadManager::StartCondition( false );
  std::atomic<bool> ThreadManager::QuitFlag( false );
  std::atomic<bool> ThreadManager::ErrorFlag( false );


  ThreadManager::ThreadManager() :
    _dataManagerThread( dataManagerLoadingThread ),
    _contextManagerThread( contextManagerLoadingThread ),
    DataUpdate( false ),
    ContextUpdate( false ),
    MusicUpdate( nullptr )
  {
  }


  ThreadManager::~ThreadManager()
  {
    // Make sure this is true if the thread manager is being destroyed!
    QuitFlag = true;

    // Notify everything one last time to ensue all the threads see the quit flag
    StartCondition.variable.notify_all();
    DataUpdate.variable.notify_all();
    ContextUpdate.variable.notify_all();
    MusicUpdate.variable.notify_all();
  }


  void ThreadManager::quit()
  {
    INFO_LOG( "Thread Manager Calling Quit" );
    QuitFlag = true;

    StartCondition.variable.notify_all();
    DataUpdate.variable.notify_all();
    ContextUpdate.variable.notify_all();
    MusicUpdate.variable.notify_all();
  }


  void ThreadManager::error()
  {
    ERROR_LOG( "Thread Manager Calling Quit. An error has occured." );
    ErrorFlag = true;
    QuitFlag = true;

    StartCondition.variable.notify_all();
    DataUpdate.variable.notify_all();
    ContextUpdate.variable.notify_all();
    MusicUpdate.variable.notify_all();
  }


  void ThreadManager::startAll()
  {
    // Set the start condition under lock
    {
      std::lock_guard<std::mutex> lk( StartCondition.mutex );
      StartCondition.data = true;
    }

    // Notify the start
    StartCondition.variable.notify_all();
  }


  void ThreadManager::stopAll()
  {
    // Quit MUST have been called already by this point
    // Wait for all the threads to re-join
    _dataManagerThread.join();
    _contextManagerThread.join();
  }

}

