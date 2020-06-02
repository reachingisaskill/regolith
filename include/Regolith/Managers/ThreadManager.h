
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

      static Condition<bool> StartCondition;

      static std::atomic<bool> QuitFlag;

      Condition<bool> StackUpdate;
  };

}

#endif // REGOLITH_MANAGERS_THREAD_MANAGER_H_

