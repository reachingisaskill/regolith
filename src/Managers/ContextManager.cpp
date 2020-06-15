
#include "Regolith/Managers/ContextManager.h"
#include "Regolith/Managers/ThreadManager.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  // Forward declare the loading thread
  void contextManagerLoadingThread();


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Con/Destruction

  ContextManager::ContextManager() :
    _loaded( false ),
    _progress( 0.0 ),
    _globalContextGroup(),
    _contextGroups( "Context Groups" ),
    _currentContextGroup( nullptr ),
    _nextContextGroup( nullptr )
  {
  }


  ContextManager::~ContextManager()
  {
    INFO_LOG( "Destroying Context Manager" );
  }


  void ContextManager::clear()
  {
    // Hold this mutex in case the loading thread is active
    Condition<bool>& contextUpdate = Manager::getInstance()->getThreadManager().ContextUpdate;
    std::unique_lock<std::mutex> lock( contextUpdate.mutex );

    // Unload and then delete all the context groups
    for ( ProxyMap< ContextGroup* >::iterator it = _contextGroups.begin(); it != _contextGroups.end(); ++it )
    {
      if ( it->second->isLoaded() )
      {
        it->second->unload();
      }
      delete it->second;
    }
    _contextGroups.clear();

    // Unload the global context group
    if ( _globalContextGroup.isLoaded() )
    {
      _globalContextGroup.unload();
    }

    lock.unlock();
  }


  void ContextManager::setLoaded( bool value )
  {
    std::lock_guard<std::mutex> lg( _loadedMutex );

    _loaded = value;
  }


  bool ContextManager::isLoaded() const
  {
    std::lock_guard<std::mutex> lg( _loadedMutex );

    return _loaded;
  }


  void ContextManager::setProgress( float value )
  {
    std::lock_guard<std::mutex> lg( _progressMutex );

    _progress = value;
  }


  float ContextManager::loadingProgress() const
  {
    std::lock_guard<std::mutex> lg( _progressMutex );

    return _progress;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Configuration

  void ContextManager::configure( Json::Value& json_data )
  {
    Utilities::validateJson( json_data, "global", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "context_groups", Utilities::JSON_TYPE_OBJECT );
    Utilities::validateJson( json_data, "entry_point", Utilities::JSON_TYPE_STRING );

    // Load the global contexts and data first
    std::string global_file = json_data["global"].asString();
    _globalContextGroup.configure( global_file, true );

    // Create all the context groups but don't load any information
    Json::Value& groups = json_data["context_groups"];
    for ( Json::Value::iterator it = groups.begin(); it != groups.end(); ++it )
    {
      Utilities::validateJson( *it, Utilities::JSON_TYPE_STRING );

      std::string name = it.key().asString();
      std::string file = it->asString();

      ContextGroup* cg = new ContextGroup();
      cg->configure( file, false );
      _contextGroups.set( name, cg );
    }

    // Find the starting context group and load it
    std::string entry_point = json_data["entry_point"].asString();
    _nextContextGroup = _contextGroups.get( entry_point );
  }


  void ContextManager::validate() const
  {
  }


  void ContextManager::loadEntryPoint()
  {
    _currentContextGroup = _nextContextGroup;
    _nextContextGroup = nullptr;

    INFO_LOG( "Loading global context group" );
    _globalContextGroup.load();
    INFO_LOG( "Loading first context group" );
    _currentContextGroup->load();
    DEBUG_LOG( "Completed" );
  }


  void ContextManager::setNextContextGroup( ContextGroup* cg )
  {
    setLoaded( false );
    setProgress( 0.0 );

    _nextContextGroup = cg;

    Context* load_screen = (Context*)_nextContextGroup->getLoadScreen();

    DEBUG_STREAM << "Load scree start @ " << load_screen;
    Manager::getInstance()->setContextStack( load_screen );
  }


  void ContextManager::loadNextContextGroup()
  {
    Condition<bool>& contextUpdate = Manager::getInstance()->getThreadManager().ContextUpdate;
    std::unique_lock<std::mutex> lock( contextUpdate.mutex );

    contextUpdate.data = true;

    lock.unlock();

    contextUpdate.variable.notify_all();
  }


  void ContextManager::startContextGroup()
  {
    Manager::getInstance()->setContextStack( _currentContextGroup->getEntryPoint() );
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Loading thread

  void contextManagerLoadingThread()
  {
    INFO_LOG( "Context Manager loading thread start." );

    std::atomic<bool>& quitFlag = Manager::getInstance()->getThreadManager().QuitFlag;

    INFO_LOG( "Context Manager loading thread initialised and waiting to start" );
    {
      Condition<bool>& startCondition = Manager::getInstance()->getThreadManager().StartCondition;
      std::unique_lock<std::mutex> lk( startCondition.mutex );
      startCondition.variable.wait( lk, [&]()->bool{ return quitFlag || startCondition.data; } );
      lk.unlock();
    }
    INFO_LOG( "Context Manager loading thread go." );


    ContextManager& manager = Manager::getInstance()->getContextManager();
    Condition<bool>& contextUpdate = Manager::getInstance()->getThreadManager().ContextUpdate;
    std::unique_lock<std::mutex> contextLock( contextUpdate.mutex );

    try
    {

      INFO_LOG( "Loading thread waiting for first command" );
      while( ! quitFlag )
      {
        contextUpdate.variable.wait( contextLock, [&]()->bool{ return quitFlag || contextUpdate.data; } );
        if ( quitFlag ) break;

        DEBUG_STREAM << "CONTEXT LOADING THREAD WORKING";

        if ( manager._nextContextGroup != nullptr )
        {
          // Update and load the current context group pointer
          manager._currentContextGroup->unload();
          manager._currentContextGroup = manager._nextContextGroup;
          manager._nextContextGroup = nullptr;
          manager._currentContextGroup->load();
        }

        // Signal that the context group has been loaded
        contextUpdate.data = false;
        manager.setProgress( 1.0 );
        manager.setLoaded( true );

        contextUpdate.variable.notify_all();
      }

      contextLock.unlock();
    }
    catch( Exception& ex )
    {
      Manager::getInstance()->getThreadManager().error();
      if ( contextLock.owns_lock() )
      {
        contextUpdate.variable.notify_all();
        contextLock.unlock();
      }
      FAILURE_LOG( "Regolith Exception thrown from Context Manager Thread." );
      std::cerr << ex.elucidate();
    }
    catch( std::exception& ex )
    {
      Manager::getInstance()->getThreadManager().error();
      if ( contextLock.owns_lock() )
      {
        contextUpdate.variable.notify_all();
        contextLock.unlock();
      }
      FAILURE_LOG( "Standard Exception thrown from Context Manager Thread." );
      std::cerr << ex.what();
    }

    INFO_LOG( "Context Manager loading thread stopped." );
  }

}

