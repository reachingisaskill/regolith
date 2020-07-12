
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
    _engineRenderingThread( engineRenderingThread ),
    DataUpdate( false ),
    ContextUpdate( false ),
    MusicUpdate( nullptr ),
    FrameSynchronisation( false ),
    RenderSynchronisation( false )
  {
  }


  ThreadManager::~ThreadManager()
  {
    // Make sure this is true if the thread manager is being destroyed!
    if ( QuitFlag == false )
    {
      this->stopAll();
    }

    // Notify everything one last time to ensue all the threads see the quit flag
    StartCondition.variable.notify_all();
    DataUpdate.variable.notify_all();
    ContextUpdate.variable.notify_all();
    MusicUpdate.variable.notify_all();
    FrameSynchronisation.variable.notify_all();
    RenderSynchronisation.variable.notify_all();
  }


  void ThreadManager::quit()
  {
    INFO_LOG( "Thread Manager Calling Quit" );
    QuitFlag = true;

    StartCondition.variable.notify_all();
    DataUpdate.variable.notify_all();
    ContextUpdate.variable.notify_all();
    MusicUpdate.variable.notify_all();
    FrameSynchronisation.variable.notify_all();
    RenderSynchronisation.variable.notify_all();
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
    FrameSynchronisation.variable.notify_all();
    RenderSynchronisation.variable.notify_all();
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
    // Call quit first to make sure everything is going to stop.
    this->quit();

    // Wait for all the threads to re-join
    _dataManagerThread.join();
    _contextManagerThread.join();
    _engineRenderingThread.join();
  }

}

