
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
  // List of all threads

  // Data Manager loading thread
  void dataManagerLoadingThread();

  // Context Manager loading thread
  void contextManagerLoadingThread();

  // Context Manager loading thread
  void engineRenderingThread();


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Status enum for thread behvaiour

  enum class ThreadStatus { Null, Waiting, Initialising, Running, Closing, Stop };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Manager Class
  class ThreadManager
  {
    private:
      // Data manager thread container
      std::thread _dataManagerThread;

      // Context manager thread container
      std::thread _contextManagerThread;

      // Context manager thread container
      std::thread _engineRenderingThread;


      void waitThreadStatus( ThreadStatus );

    public:
      // Con/Destructors
      ThreadManager();

      ~ThreadManager();

      // Signal everything!
      void quit() { QuitFlag = true; }

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


      // Current thread status variables to observe thread behaviour
      static Condition<ThreadStatus> DataManagerStatus;
      static Condition<ThreadStatus> ContextManagerStatus;
      static Condition<ThreadStatus> EngineRenderingStatus;


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

