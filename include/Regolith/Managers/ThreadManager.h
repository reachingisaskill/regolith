
#ifndef REGOLITH_MANAGERS_THREAD_MANAGER_H_
#define REGOLITH_MANAGERS_THREAD_MANAGER_H_

#include "Regolith/Global/Global.h"

#include "Regolith/Utilities/Condition.h"

#include <thread>
#include <mutex>
#include <atomic>


namespace Regolith
{
////////////////////////////////////////////////////////////////////////////////////////////////////
  // Forward declaration
  class ThreadHandler;


////////////////////////////////////////////////////////////////////////////////////////////////////
  // List of all threads

  // Data Manager loading thread
  void dataManagerLoadingThread();

  // Context Manager loading thread
  void contextManagerLoadingThread();

  // Context Manager loading thread
  void engineRenderingThread();


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Manager Class
  class ThreadManager
  {
    private:
      typedef std::set< ThreadHandler* > HandlerSet;

    private:
      // Data manager thread container
      std::thread _dataManagerThread;

      // Context manager thread container
      std::thread _contextManagerThread;

      // Context manager thread container
      std::thread _engineRenderingThread;


      // Set of thread handlers that are current active
      HandlerSet _threadHandlers;
      std::mutex _handlerSetMutex;


      // Private function to wait on the status of the thread handlers
      void waitThreadStatus( ThreadStatus );

    public:
      // Con/Destructors
      ThreadManager();

      ~ThreadManager();


      // Function for a thread handler to register itself with the manager
      void registerThreadHandler( ThreadHandler* );

      // Function for a thread handler to remove itself from the manager before destruction
      void removeThreadHandler( ThreadHandler* );


//      // Set the global quit flag. Threads will stop when they are ready to do so
//      void quit() { QuitFlag = true; }

      // Stop the threads with error signals
      void error();

      // Send the start signals
      void startAll();

      // Tell the threads to stop the primary loops and perform closing operations
      void stopAll();

      // Tell the threads to stop completely
      void closeAll();

      // Closes the thread handlers
      void join();



////////////////////////////////////////////////////////////////////////////////
      // Conditions for threads to interact with

      // Signals the start of the engine - all threads now active
      static Condition<bool> StartCondition;

      // Signals the start of the engine - all threads now be joined
      static Condition<bool> StopCondition;

      // Every thread that sees this flag MUST end
      static std::atomic<bool> QuitFlag;

      // Every thread that sees this flag MUST end
      static std::atomic<bool> ErrorFlag;



      // Signals data in the DataManager Queue
      Condition<bool> DataUpdate;

      // Signals a ContextGroup to be loaded in the ContextManager
      Condition<bool> ContextUpdate;

      // Signals a change of song in the AudioManager
      Condition<Mix_Music*> MusicUpdate;

      // Mutex to control access to contexts
      std::mutex RenderMutex;

  };

}

#endif // REGOLITH_MANAGERS_THREAD_MANAGER_H_

