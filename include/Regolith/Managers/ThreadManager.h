
#ifndef REGOLITH_MANAGERS_THREAD_MANAGER_H_
#define REGOLITH_MANAGERS_THREAD_MANAGER_H_


#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>


namespace Regolith
{

  template < class DATA >
  struct Condition
  {
    std::condition_variable variable;
    std::mutex mutex;
    DATA data;

    Condition() {}
    explicit Condition( DATA d ) : data( d ) {}
  };


  class ThreadManager
  {
    private:

    public:
      // Con/Destructors
      ThreadManager();

      ~ThreadManager();

      // Signal everything!
      void quit();

////////////////////////////////////////////////////////////////////////////////
      // Conditions for threads to interact with

      // Signals the start of the engine - all threads now active
      static Condition<bool> StartCondition;

      // Every thread that sees this flag MUST end
      static std::atomic<bool> QuitFlag;


      // Signals data in the DataManager Queue
      Condition<bool> DataUpdate;

      // Signals a ContextGroup to be loaded in the ContextManager
      Condition<bool> ContextUpdate;

  };

}

#endif // REGOLITH_MANAGERS_THREAD_MANAGER_H_

