
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
    _entryPoint( nullptr ),
    _loadContextGroup( nullptr ),
    _unloadContextGroup( nullptr ),
    _currentContextGroup( nullptr ),
    _renderContextGroup( nullptr ),
    _contextUpdate( false )
  {
  }


  ContextManager::~ContextManager()
  {
    INFO_LOG( "ContextManager::~ContextManager : Destroying Context Manager" );
  }


  void ContextManager::clear()
  {
    // Hold this mutex in case the loading thread is active
    std::unique_lock<std::mutex> lock( _contextUpdate.mutex );

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


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Configuration

  void ContextManager::configure( Json::Value& json_data )
  {
    INFO_LOG( "ContextManager::configure : Configuring" );

    // Register condition variables that can block thread execution
    Manager::getInstance()->getThreadManager().registerCondition( &_contextUpdate.variable );
    Manager::getInstance()->getThreadManager().registerCondition( &_renderContextGroup.variable );

    // Validate expected json values
    validateJson( json_data, "global", JsonType::STRING );
    validateJson( json_data, "context_groups", JsonType::OBJECT );

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
      validateJson( *it, JsonType::STRING );

      std::string name = it.key().asString();
      std::string file = it->asString();

      ContextGroup* cg = new ContextGroup();
      cg->configure( file, false );
      _contextGroups[ name ] = cg;
    }
    INFO_LOG( "ContextManager::configure : Context Groups Configured" );


    INFO_LOG( "ContextManager::configure : Locating entry point" );
    if ( validateJson( json_data, "entry_point", JsonType::STRING, false ) )
    {
      // Find the starting context group and load it
      std::string entry_point = json_data["entry_point"].asString();
      _entryPoint = this->getContextGroup( entry_point );
      INFO_LOG( "ContextManager::configure : Entry Point Located" );
    }
    else // Start with the global context group
    {
      _entryPoint = &_globalContextGroup;
      INFO_LOG( "ContextManager::configure : Entry point default to global context group" );
    }
  }


  ContextGroup* ContextManager::loadEntryPoint()
  {
    INFO_LOG( "ContextManager::loadEntryPoint : Loading global context group" );
    _globalContextGroup.load();
    INFO_LOG( "ContextManager::loadEntryPoint : Loading first context group" );
    _entryPoint->load();
    DEBUG_LOG( "ContextManager::loadEntryPoint : Completed" );
    return _entryPoint;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Accessors

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


  bool ContextManager::isLoading() const
  {
    GuardLock lg( _currentGroupMutex );
    return ( _currentContextGroup != nullptr );
  }


//  bool ContextManager::isLoaded() const
//  {
//    GuardLock lg( _currentGroupMutex );
//
//    if ( _currentContextGroup != nullptr )
//    {
//      return _currentContextGroup->isLoaded();
//    }
//    else
//    {
//      return false;
//    }
//  }
//
//
//  float ContextManager::loadingProgress() const
//  {
//    GuardLock lg( _currentGroupMutex );
//
//    if ( _currentContextGroup != nullptr )
//    {
//      return _currentContextGroup->getLoadProgress();
//    }
//    else
//    {
//      return 0.0;
//    }
//  }
//
//
//  std::string ContextManager::loadingStatus() const
//  {
//    GuardLock lg( _currentGroupMutex );
//
//    if ( _currentContextGroup != nullptr )
//    {
//      return _currentContextGroup->getLoadStatus();
//    }
//    else
//    {
//      return std::string( "" );
//    }
//  }


  void ContextManager::loadContextGroup( ContextGroup* next )
  {
    // Lock the pointers for the next action
    GuardLock lg( _loadGroupMutex );

    DEBUG_LOG( "ContextManager::setNextContextGroup : Setting next context group." );
    _loadContextGroup = next;

    std::unique_lock<std::mutex> lock( _contextUpdate.mutex );
    _contextUpdate.data = true;
    lock.unlock();

    DEBUG_LOG( "ContextManager::loadNextContextGroup : Triggering condition variable." );
    _contextUpdate.variable.notify_all();
  }


  void ContextManager::unloadContextGroup( ContextGroup* previous )
  {
    // Lock the pointers for the next action
    GuardLock lg( _loadGroupMutex );

    DEBUG_LOG( "ContextManager::setNextContextGroup : Setting next context group." );
    _unloadContextGroup = previous;

    std::unique_lock<std::mutex> lock( _contextUpdate.mutex );
    _contextUpdate.data = true;
    lock.unlock();

    DEBUG_LOG( "ContextManager::loadNextContextGroup : Triggering condition variable." );
    _contextUpdate.variable.notify_all();
  }


  void ContextManager::requestRenderContextGroup( ContextGroup* context_group )
  {
    // Only one may be rendered at a time
    GuardLock render_lock( _renderGroupMutex );

    // Lock the pointer mutex while we edit it
    UniqueLock pointer_lock( _renderContextGroup.mutex );

    // Set the pointer
    _renderContextGroup.data = context_group;

    // Wait for group to be come rendered.
    _renderContextGroup.variable.wait( pointer_lock, [&]()->bool{ return _renderContextGroup.data->isRendered() || ThreadManager::ErrorFlag; } );

    // Clear the pointer
    _renderContextGroup.data = nullptr;

    // Clean up
    pointer_lock.unlock();
  }


  void ContextManager::renderContextGroup( Camera& camera )
  {
    GuardLock lg( _renderContextGroup.mutex );
    if ( _renderContextGroup.data != nullptr )
    {
      if ( _renderContextGroup.data->engineRenderLoadedObjects( camera ) )
      {
        _renderContextGroup.variable.notify_all();
      }
    }
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
    Condition<bool>& contextUpdate = manager._contextUpdate;
    UniqueLock contextLock( contextUpdate.mutex, std::defer_lock );
    UniqueLock loadPointerLock( manager._loadGroupMutex, std::defer_lock );
    UniqueLock currentPointerLock( manager._currentGroupMutex, std::defer_lock );


    // Update the thread status
    threadHandler.running();

    try
    {
      while( threadHandler.isGood() )
      {
        // Signal that the context group has been loaded
        contextLock.lock();
        if ( ! contextUpdate.data )
        {
          contextUpdate.variable.wait( contextLock, [&]()->bool{ return (! threadHandler.isGood() ) || contextUpdate.data; } );
          contextUpdate.data = false;
        }
        contextLock.unlock();

        if ( ! threadHandler.isGood() ) break;

        DEBUG_STREAM << "ContextManagerLoadingThread : WORKING";

        // Copy the request into the thread and clear them
        loadPointerLock.lock();
        ContextGroup* loadGroup = manager._loadContextGroup;
        manager._loadContextGroup = nullptr;
        ContextGroup* unloadGroup = manager._unloadContextGroup;
        manager._unloadContextGroup = nullptr;
        loadPointerLock.unlock();

        // Update the current group pointer
        currentPointerLock.lock();
        manager._currentContextGroup = unloadGroup;
        currentPointerLock.unlock();

        // Unload the unload group
        if ( ( unloadGroup != nullptr ) && ( ! unloadGroup->isGlobal() ) )
        {
          unloadGroup->unload();
        }

        // Update the current group pointer
        currentPointerLock.lock();
        manager._currentContextGroup = unloadGroup;
        currentPointerLock.unlock();

        // Load the load group
        if ( ( loadGroup != nullptr ) && ( ! loadGroup->isGlobal() ) )
        {
          loadGroup->load();
        }
      }

//      contextLock.unlock();
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

