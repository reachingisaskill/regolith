
#include "Regolith/Managers/ThreadHandler.h"

#include "Regolith/Managers/Manager.h"
#include "Regolith/Managers/ThreadManager.h"


namespace Regolith
{

  ThreadHandler::ThreadHandler( std::string name, ThreadName id ) :
    _threadName( name ),
    _identifier( id ),
    _owner( Manager::getInstance()->getThreadManager() ),
    _quitFlag( ThreadManager::QuitFlag ),
    _errorFlag( ThreadManager::ErrorFlag ),
    _startCondition( ThreadManager::StartCondition )
  {
  }


  ThreadHandler::~ThreadHandler()
  {
  }


  void ThreadHandler::start()
  {
    INFO_STREAM << "ThreadHandler< " << _threadName << " > : Waiting on start condition.";
    _owner.setThreadStatus( _identifier, THREAD_WAITING );

    UniqueLock lk( _startCondition.mutex );
    if ( ! (_quitFlag || _errorFlag || _startCondition.data) )
    {
      _startCondition.variable.wait( lk, [&]()->bool{ return _quitFlag || _errorFlag || _startCondition.data; } );
      lk.unlock();
    }

    INFO_STREAM << "ThreadHandler< " << _threadName << " > : Initialising.";
    _owner.setThreadStatus( _identifier, THREAD_INITIALISING );
  }


  void ThreadHandler::running()
  {
    INFO_STREAM << "ThreadHandler< " << _threadName << " > : Thread running.";
    _owner.setThreadStatus( _identifier, THREAD_RUNNING );
  }


  void ThreadHandler::closing()
  {
    INFO_STREAM << "ThreadHandler< " << _threadName << " > : Thread closing.";
    _owner.setThreadStatus( _identifier, THREAD_CLOSING );
  }


  void ThreadHandler::stop()
  {
    INFO_STREAM << "ThreadHandler< " << _threadName << " > : Thread stopped.";
    _owner.setThreadStatus( _identifier, THREAD_STOP );
  }


  void ThreadHandler::waitStatus( ThreadName name, ThreadStatus status )
  {
    _owner.waitThreadStatus( name, status );
  }


  void ThreadHandler::throwError( Exception& ex )
  {
    Manager::getInstance()->getThreadManager().error();

    FAILURE_STREAM << "ThreadHandler< " << _threadName << " > : Regolith Exception thrown.";
    FAILURE_STREAM << "ThreadHandler< " << _threadName << " > : " << ex.elucidate();
    std::cerr << ex.elucidate() << std::endl;
  }


  void ThreadHandler::throwError( std::exception& ex )
  {
    Manager::getInstance()->getThreadManager().error();

    FAILURE_STREAM << "ThreadHandler< " << _threadName << " > : Standard Exception thrown.";
    FAILURE_STREAM << "ThreadHandler< " << _threadName << " > : " << ex.what();
    std::cerr << ex.what() << std::endl;
  }

}

