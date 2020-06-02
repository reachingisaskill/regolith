
#include "Regolith/Managers/ThreadManager.h"


namespace Regolith
{

  Condition<bool> ThreadManager::StartCondition( false );
  std::atomic<bool> ThreadManager::QuitFlag( false );


  ThreadManager::ThreadManager() :
    StackUpdate( false )
  {
  }


  ThreadManager::~ThreadManager()
  {
    // Make sure this is true if the thread manager is being destroyed!
    QuitFlag = true;

    // Notify everything one last time to ensue all the threads see the quit flag
    StackUpdate.variable.notify_all();
  }


  void ThreadManager::quit()
  {
    QuitFlag = true;

    StackUpdate.variable.notify_all();
  }

}

