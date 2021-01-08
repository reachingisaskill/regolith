
#ifndef REGOLITH_MANAGERS_THREAD_HANDLER_H_
#define REGOLITH_MANAGERS_THREAD_HANDLER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Links/LinkThreadManager.h"
#include "Regolith/Utilities/Condition.h"

#include <string>


namespace Regolith
{

  class ThreadHandler
  {
    private:
      std::string _threadName;
      ThreadName _identifier;
      Link<ThreadManager, ThreadHandler> _owner;

      std::atomic<bool>& _quitFlag;
      std::atomic<bool>& _errorFlag;
      Condition<bool>& _startCondition;
      Condition<bool>& _stopCondition;

    public:

      // Aquire references to the static thread flags
      ThreadHandler( std::string, ThreadName );

      // Do nothing here
      ~ThreadHandler();

      // Non-copyable and non-moveable
      ThreadHandler( const ThreadHandler& ) = delete;
      ThreadHandler( ThreadHandler&& ) = delete;
      ThreadHandler& operator=( const ThreadHandler& ) = delete;
      ThreadHandler& operator=( ThreadHandler&& ) = delete;


      // Wait for the status of this handler to change
      void waitStatus( ThreadName, ThreadStatus );
      

      // Halt execution and wait for the start signal
      void start();

      // Signal that the main loop is now starting
      void running();

      // Halt execution and wait for the stop signal
      void closing();

      // Halt execution and wait for the stop signal
      void stop();


      // Return the status of the static quit flag
      bool quit() const { return _quitFlag; }

      // Return the status of the static error flag
      bool error() const { return _errorFlag; }

      // Return an or-ed list of flags inorder to break out of the threads main loop
      bool isGood() const { return ! ( _quitFlag || _errorFlag ); }


      // When the owning thread finds an error signal the main thread via the thread manager
      void throwError( Exception& );
      void throwError( std::exception& );
  };

}

#endif // REGOLITH_MANAGERS_THREAD_HANDLER_H_

