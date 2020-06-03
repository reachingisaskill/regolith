
#include "Regolith/Managers/ThreadManager.h"

#include "logtastic.h"


namespace Regolith
{

  Condition<bool> ThreadManager::StartCondition( false );
  std::atomic<bool> ThreadManager::QuitFlag( false );


  ThreadManager::ThreadManager() :
    DataUpdate( false ),
    StackUpdate( false )
  {
  }


  ThreadManager::~ThreadManager()
  {
    // Make sure this is true if the thread manager is being destroyed!
    QuitFlag = true;

    // Notify everything one last time to ensue all the threads see the quit flag
    StartCondition.variable.notify_all();
    DataUpdate.variable.notify_all();
    StackUpdate.variable.notify_all();
  }


  void ThreadManager::quit()
  {
    INFO_LOG( "Thread Manager Calling Quit" );
    QuitFlag = true;

    StartCondition.variable.notify_all();
    DataUpdate.variable.notify_all();
    StackUpdate.variable.notify_all();
  }

}

