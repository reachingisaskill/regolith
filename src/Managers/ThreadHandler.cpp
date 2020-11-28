
#include "Regolith/Managers/ThreadHandler.h"

#include "Regolith/Managers/Manager.h"


namespace Regolith
{

  ThreadHandler::ThreadHandler( std::string name ) :
    _threadName( name ),
    _quitFlag( ThreadManager::QuitFlag ),
    _errorFlag( ThreadManager::ErrorFlag ),
    _startCondition( ThreadManager::StartCondition ),
    _stopCondition( ThreadManager::StopCondition )
  {
  }


  ThreadHandler::~ThreadHandler()
  {
    Manager::getInstance()->getThreadManager().removeThreadHandler( this );
  }


  void ThreadHandler::updateStatus( ThreadStatus new_status )
  {
    INFO_STREAM << "ThreadHandler< " << _threadName << " > : Updating status: " << ThreadStatusStrings.at( new_status );
    std::lock_guard< std::mutex > lk( _status.mutex );
    _status.data = new_status;
    _status.variable.notify_all();
  }


  ThreadStatus ThreadHandler::getStatus() const
  {
    std::lock_guard< std::mutex > lk( _status.mutex );
    return _status.data;
  }


  void ThreadHandler::waitStart()
  {
    updateStatus( ThreadStatus::Waiting );
    INFO_STREAM << "ThreadHandler< " << _threadName << " > : Waiting on start condition.";
    std::unique_lock<std::mutex> lk( _startCondition.mutex );
    if ( _quitFlag || _errorFlag || _startCondition.data )
    {
      lk.unlock();
      return;
    }
    _startCondition.variable.wait( lk, [&]()->bool{ return _quitFlag || _errorFlag || _startCondition.data; } );
    lk.unlock();

    INFO_STREAM << "ThreadHandler< " << _threadName << " > : Registering with manager.";
    Manager::getInstance()->getThreadManager().registerThreadHandler( this );

    updateStatus( ThreadStatus::Initialising );
    INFO_STREAM << "ThreadHandler< " << _threadName << " > : Initialising.";
  }


  void ThreadHandler::running()
  {
    updateStatus( ThreadStatus::Running );
    INFO_STREAM << "ThreadHandler< " << _threadName << " > : Thread running.";
  }


  void ThreadHandler::waitStop()
  {
    updateStatus( ThreadStatus::Closing );
    INFO_STREAM << "ThreadHandler< " << _threadName << " > : Waiting on stop condition.";
    std::unique_lock<std::mutex> lk( _stopCondition.mutex );
    if ( _quitFlag || _errorFlag || _stopCondition.data )
    {
      lk.unlock();
      return;
    }
    _stopCondition.variable.wait( lk, [&]()->bool{ return _quitFlag || _errorFlag || _stopCondition.data; } );
    lk.unlock();
    updateStatus( ThreadStatus::Stop );
    INFO_STREAM << "ThreadHandler< " << _threadName << " > : Stopping.";
  }


  void ThreadHandler::waitStatus( ThreadStatus expected ) const
  {
    INFO_STREAM << "ThreadHandler< " << _threadName << " > : Waiting on status: " << ThreadStatusStrings.at( expected );
    std::unique_lock<std::mutex> lk( _status.mutex );
    if ( _quitFlag || _errorFlag || (_status.data == expected) )
    {
      lk.unlock();
      return;
    }
    _status.variable.wait( lk, [&]()->bool{ return _quitFlag || _errorFlag || ( _status.data == expected ); } );
    lk.unlock();
    INFO_STREAM << "ThreadHandler< " << _threadName << " > : Status acheived: " << ThreadStatusStrings.at( expected );
  }


  void ThreadHandler::throwError( Exception& ex )
  {
    Manager::getInstance()->getThreadManager().error();

    FAILURE_STREAM << "ThreadHandler< " << _threadName << " > : Regolith Exception thrown.";
    FAILURE_STREAM << "ThreadHandler< " << _threadName << " > : " << ex.elucidate();
    std::cerr << ex.elucidate() << std::endl;

    updateStatus( ThreadStatus::Stop );
  }


  void ThreadHandler::throwError( std::exception& ex )
  {
    Manager::getInstance()->getThreadManager().error();

    FAILURE_STREAM << "ThreadHandler< " << _threadName << " > : Standard Exception thrown.";
    FAILURE_STREAM << "ThreadHandler< " << _threadName << " > : " << ex.what();
    std::cerr << ex.what() << std::endl;

    updateStatus( ThreadStatus::Stop );
  }

}

