
#include "Regolith/Managers/ContextManager.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Managers/ThreadManager.h"
#include "Regolith/Managers/ThreadHandler.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  // Forward declare the loading thread
  void contextManagerLoadingThread();


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Con/Destruction

  ContextManager::ContextManager() :
    _globalContextGroup(),
    _contextGroups(),
    _currentContextGroup( nullptr ),
    _nextContextGroup( nullptr )
  {
  }


  ContextManager::~ContextManager()
  {
    INFO_LOG( "ContextManager::~ContextManager : Destroying Context Manager" );
  }


  void ContextManager::clear()
  {
    // Hold this mutex in case the loading thread is active
    Condition<bool>& contextUpdate = Manager::getInstance()->getThreadManager().ContextUpdate;
    std::unique_lock<std::mutex> lock( contextUpdate.mutex );

    // Unload and then delete all the context groups
    for ( ContextGroupMap::iterator it = _contextGroups.begin(); it != _contextGroups.end(); ++it )
    {
      if ( it->second->isLoaded() )
      {
        DEBUG_LOG( "ContextManager::clear : Unloading context group" );
        it->second->unload();
      }
      delete it->second;
    }
    _contextGroups.clear();

    DEBUG_LOG( "ContextManager::clear : Context groups unloaded." );

    // Unload the global context group
    if ( _globalContextGroup.isLoaded() )
    {
      DEBUG_LOG( "ContextManager::clear : Unloading global context group." );
      _globalContextGroup.unload();
    }

    DEBUG_LOG( "ContextManager::clear : Complete." );

    lock.unlock();
  }


  bool ContextManager::isLoaded() const
  {
    GuardLock lg( _currentGroupMutex );
    return _currentContextGroup->isLoaded();
  }


  float ContextManager::loadingProgress() const
  {
    GuardLock lg( _currentGroupMutex );
    return _currentContextGroup->getLoadProgress();
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Configuration

  void ContextManager::configure( Json::Value& json_data )
  {
    INFO_LOG( "ContextManager::configure : Configuring" );

    Utilities::validateJson( json_data, "global", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "context_groups", Utilities::JSON_TYPE_OBJECT );

    // Load the global contexts and data first
    INFO_LOG( "ContextManager::configure : Configuring Global Context Group" );
    std::string global_file = json_data["global"].asString();
    _globalContextGroup.configure( global_file, true );
    INFO_LOG( "ContextManager::configure : Global Context Group Configured" );

    // Create all the context groups but don't load any information
    INFO_LOG( "ContextManager::configure : Configuring Context Groups" );
    Json::Value& groups = json_data["context_groups"];
    for ( Json::Value::iterator it = groups.begin(); it != groups.end(); ++it )
    {
      Utilities::validateJson( *it, Utilities::JSON_TYPE_STRING );

      std::string name = it.key().asString();
      std::string file = it->asString();

      ContextGroup* cg = new ContextGroup();
      cg->configure( file, false );
      _contextGroups[ name ] = cg;
    }
    INFO_LOG( "ContextManager::configure : Context Groups Configured" );


    INFO_LOG( "ContextManager::configure : Locating entry point" );
    if ( Utilities::validateJson( json_data, "entry_point", Utilities::JSON_TYPE_STRING, false ) )
    {
      // Find the starting context group and load it
      std::string entry_point = json_data["entry_point"].asString();
      _nextContextGroup = this->getContextGroup( entry_point );
      INFO_LOG( "ContextManager::configure : Entry Point Located" );
    }
    else // Start with the global context group
    {
      _nextContextGroup = &_globalContextGroup;
      INFO_LOG( "ContextManager::configure : Entry point default to global context group" );
    }
  }


  void ContextManager::loadEntryPoint()
  {
    UniqueLock lock1( _currentGroupMutex );
    UniqueLock lock2( _nextGroupMutex );

    _currentContextGroup = _nextContextGroup;
    _nextContextGroup = nullptr;

    lock2.unlock();
    lock1.unlock();

    INFO_LOG( "ContextManager::loadEntryPoint : Loading global context group" );
    _globalContextGroup.load();
    INFO_LOG( "ContextManager::loadEntryPoint : Loading first context group" );
    _currentContextGroup->load();
    DEBUG_LOG( "ContextManager::loadEntryPoint : Completed" );
  }


  ContextGroup* ContextManager::getContextGroup( std::string name )
  {
    ContextGroupMap::iterator found = _contextGroups.find( name );

    if ( found == _contextGroups.end() )
    {
      Exception ex( "ContextManager::getContextGroup()", "Could not find context group name in map" );
      ex.addDetail( "Name", name );
      throw ex;
    }

    return found->second;
  }


  ContextGroup* ContextManager::getCurrentContextGroup()
  {
    GuardLock lg( _currentGroupMutex );

    return _currentContextGroup;
  }


  void ContextManager::setNextContextGroup( ContextGroup* cg )
  {
    GuardLock lg( _nextGroupMutex );

    DEBUG_LOG( "ContextManager::setNextContextGroup : Setting next context group." );
    _nextContextGroup = cg;
  }


  void ContextManager::loadNextContextGroup()
  {
    GuardLock lg( _nextGroupMutex );

    Condition<bool>& contextUpdate = Manager::getInstance()->getThreadManager().ContextUpdate;
    std::unique_lock<std::mutex> lock( contextUpdate.mutex );

    contextUpdate.data = true;

    lock.unlock();
    DEBUG_LOG( "ContextManager::loadNextContextGroup : Triggering condition variable." );

    contextUpdate.variable.notify_all();
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Loading thread


  void contextManagerLoadingThread()
  {
    ThreadHandler threadHandler( "ContextManagerThread", REGOLITH_THREAD_CONTEXT );

    // Wait on the start condition
    threadHandler.start();


    // Set up references
    ContextManager& manager = Manager::getInstance()->getContextManager();
    Condition<bool>& contextUpdate = Manager::getInstance()->getThreadManager().ContextUpdate;
    UniqueLock contextLock( contextUpdate.mutex );
    UniqueLock currentPointerLock( manager._currentGroupMutex, std::defer_lock );
    UniqueLock nextPointerLock( manager._nextGroupMutex, std::defer_lock );


    // Update the thread status
    threadHandler.running();

    try
    {
      while( threadHandler.isGood() )
      {
        contextUpdate.variable.wait( contextLock, [&]()->bool{ return (! threadHandler.isGood() ) || contextUpdate.data; } );

        if ( ! threadHandler.isGood() ) break;

        DEBUG_STREAM << "ContextManagerLoadingThread : WORKING";

        if ( manager._nextContextGroup != nullptr )
        {
          // Update and load the current context group pointer
          if ( ( manager._currentContextGroup != nullptr ) && ( ! manager._currentContextGroup->isGlobal() ) )
          {
            manager._currentContextGroup->unload();
          }

          // Lock the loading mutex while where swap the current context group
          currentPointerLock.lock();
          nextPointerLock.lock();
          manager._currentContextGroup = manager._nextContextGroup;
          manager._nextContextGroup = nullptr;
          nextPointerLock.unlock();
          currentPointerLock.unlock();

          manager._currentContextGroup->load();
        }

        // Signal that the context group has been loaded
        contextUpdate.data = false;

        contextUpdate.variable.notify_all();
      }

      contextLock.unlock();
    }
    catch( Exception& ex )
    {
      if ( contextLock.owns_lock() )
      {
        contextUpdate.variable.notify_all();
        contextLock.unlock();
      }
      threadHandler.throwError( ex );
      return;
    }
    catch( std::exception& ex )
    {
      if ( contextLock.owns_lock() )
      {
        contextUpdate.variable.notify_all();
        contextLock.unlock();
      }
      threadHandler.throwError( ex );
      return;
    }

    threadHandler.closing();

    // Do any closing operations here

    threadHandler.stop();
  }

}

