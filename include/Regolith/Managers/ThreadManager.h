
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

  // Context Manager loading thread
  void contextManagerLoadingThread();

  // Context Manager loading thread
  void engineRenderingThread();


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Manager Class
  class ThreadManager
  {
    friend class ThreadHandler;

    private:
      typedef std::map< ThreadName, ThreadStatus > StatusMap;

    private:
      // Context manager thread container
      std::thread _contextManagerThread;

      // Context manager thread container
      std::thread _engineRenderingThread;


      // Set of thread handlers that are current active
      Condition< StatusMap > _threadStatus;


    protected:

      // Used by the thread handlers to update their respective status' status
      void setThreadStatus( ThreadName, ThreadStatus );


    public:
      // Con/Destructors
      ThreadManager();

      ~ThreadManager();


      // Stop the threads with error signals
      void error();


      // Send the start signals
      void startAll();

      // Tell the threads to stop the primary loops and perform closing operations
      void stopAll();

      // Closes the thread handlers
      void join();


      // Wait on the status of one or all the the threads
      void waitThreadStatus( ThreadStatus );
      void waitThreadStatus( ThreadName, ThreadStatus );


////////////////////////////////////////////////////////////////////////////////
      // Conditions for threads to interact with

      // Signals the start of the engine - all threads now active
      static Condition<bool> StartCondition;

      // Signals the start of the engine - all threads now active
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

