
#ifndef REGOLITH_MANAGERS_THREAD_MANAGER_H_
#define REGOLITH_MANAGERS_THREAD_MANAGER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/Component.h"
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
  class ThreadManager : public Component
  {
    // Allow links to access the private members
    template < class T, class R > friend class Link;

    private:
      typedef std::map< ThreadName, ThreadStatus > StatusMap;
      typedef std::list< std::condition_variable* > ConditionList;

    private:
      // Context manager thread container
      std::thread _contextManagerThread;

      // Context manager thread container
      std::thread _engineRenderingThread;


      // Set of thread handlers that are current active
      Condition< StatusMap > _threadStatus;


      // List of all the condition variables that can be blocking threads.
      // If an error is called we must be able to trigger all of them!
      ConditionList _conditionVariables;


    protected:

      // Used by the thread handlers to update their respective status' status
      void setThreadStatus( ThreadName, ThreadStatus );

      // Wait on the status of one or all the the threads
      void waitThreadStatus( ThreadStatus );
      void waitThreadStatus( ThreadName, ThreadStatus );

      // Register a condition variable so that it can be triggered in the event of an error
      void registerCondition( std::condition_variable* );


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


////////////////////////////////////////////////////////////////////////////////
      // Component Interface
      // Register game-wide events with the manager
      virtual void registerEvents( InputManager& ) override {}

      // Regolith events
      virtual void eventAction( const RegolithEvent&, const SDL_Event& ) override {}

  };

}

#endif // REGOLITH_MANAGERS_THREAD_MANAGER_H_

