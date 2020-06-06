
#include "Regolith/Managers/ContextManager.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  // Forward declare the loading thread
  void contextManagerLoadingThread();


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Con/Destruction

  ContextManager::ContextManager() :
    _loadingThread( contextManagerLoadingThread ),
    _globalContextGroup(),
    _contextGroups( "manager_context_handlers" ),
    _currentContextGroup( nullptr ),
    _nextContextGroup( nullptr ),
    _isLoaded( false ),
    _loadedProgress( 1.0 )
  {
    _contextGroups.addObject( nullptr, "null" );
  }


  ContextManager::~ContextManager()
  {
    INFO_LOG( "Destroying Context Manager" );
    _contextGroups.clear();
    _globalContextGroup.unload();
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Configuration

  void ContextManager::configure( Json::Value& json_data )
  {
    Utilities::validateJson( json_data, "global", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "context_groups", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJson( json_data, "entry_point", Utilities::JSON_TYPE_ARRAY );

    // Load the global contexts and data first
    std::string global_file = json_data["global"].asString();
    _globalContextGroup.setFile( global_file );
    _globalContextGroup.load();

    // Create all the context groups but don't load any information
    Json::Value& groups = json_data["context_groups"];
    Json::ArrayIndex groups_size = groups.size();
    for ( Json::ArrayIndex i = 0; i != groups_size; ++i )
    {
      Utilities::validateJson( groups[i], "name", Utilities::JSON_TYPE_STRING );
      Utilities::validateJson( groups[i], "file", Utilities::JSON_TYPE_STRING );

      std::string name = groups[i]["name"].asString();
      std::string file = groups[i]["file"].asString();

      _contextGroups.addObject( new ContextGroup( file ), name );
    }

    // Find the starting context group and load it
    std::string entry_point = json_data["entry_point"].asString();
    _currentContextGroup = &_contextGroups.get( entry_point );
    _currentContextGroup.load();
  }


  void ContextManager::validate() const
  {
  }


  void ContextManager::loadContextGroup( ContextGroup* cg )
  {
    _isLoaded = false;
    _loadedProgress = 0.0;

    _nextContextGroup = cg;

    Context* load_screen = _nextContextGroup->getLoadScreen();

    Manager::getInstance()->setContextStack( load_screen );

  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Loading thread

  void contextManagerLoadingThread()
  {
    INFO_LOG( "Context Manager loading thread start." );

    ContextManager& manager = Manager::getInstance()->getContextManager();
    std::atomic<bool>& quitFlag = Manager::getInstance()->getThreadManager().QuitFlag;

    INFO_LOG( "Context Manager loading thread initialised and waiting to start" );
    {
      Condition<bool>& startCondition = Manager::getInstance()->getThreadManager().StartCondition;
      std::unique_lock<std::mutex> lk( startCondition.mutex );
      startCondition.variable.wait( lk, [&]()->bool{ return quitFlag || startCondition.data; } );
      lk.unlock();
    }
    INFO_LOG( "Context Manager loading thread go." );


    Condition<bool>& contextUpdate = Manager::getInstance()->getThreadManager().ContextUpdate;
    Condition<bool>& dataUpdate = Manager::getInstance()->getThreadManager().DataUpdate;
    IDNumber temp_number;

    std::unique_lock<std::mutex> contextLock( contextUpdate.mutex );
    std::unique_lock<std::mutex> dataLock( dataUpdate.mutex, std::defer_lock );

    INFO_LOG( "Loading thread waiting for first command" );
    while( ! quitFlag )
    {
      contextUpdate.variable.wait( contextLock, [&]()->bool{ return quitFlag || contextUpdate.data; } );

      DEBUG_STREAM << "CONTEXT LOADING THREAD WORKING";

      if ( manager._nextContextGroup != nullptr )
      {
        // Halt the data thread first. No interfering.
        dataLock.lock();
        dataUpdate.data = true;

        // Update and load the current context group pointer
        manager._currentContextGroup->unload();
        manager._currentContextGroup = manager._nextContextGroup;
        manager._nextContextGroup = nullptr;
        manager._currentContextGroup->load();

        // Wait for the data thread to load all the data objects
        dataUpdate.variable.wait( dataLock, [&]()->bool { return quitFlag || ! dataUpdate.data; } );
        dataLock.unlock();
      }

      // Signal that the context group has been loaded
      contextUpdate.data = false;
      manager._isLoaded = true;
      manager._loadedProgress = 1.0;
    }

    contextLock.unlock();
    INFO_LOG( "Context Manager loading thread stopped." );
  }

}

