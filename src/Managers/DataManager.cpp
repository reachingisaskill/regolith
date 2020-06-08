
#include "Regolith/Managers/DataManager.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Utilities/JsonValidation.h"

#include "logtastic.h"

#include <mutex>
#include <atomic>


namespace Regolith
{

  void dataLoadingThread();
  void dataLoadFunction();
  void dataUnloadFunction();


  DataManager::DataManager() :
    _loadingThread( dataLoadingThread ),
    _indexFile(),
    _loadQueue(),
    _unloadQueue()
  {
  }


  DataManager::~DataManager()
  {
    INFO_LOG( "Destroying Data Manager." );
    clear();
  }


  void DataManager::clear()
  {
    INFO_LOG( "Clearing Data Manager." );

    _loadQueue.clear();
    _unloadQueue.clear();
  }


  void DataManager::load( DataHandler* handler )
  {
    DEBUG_STREAM << "LOAD";
    _loadQueue.push( handler );

    {
      std::lock_guard<std::mutex>( Manager::getInstance()->getThreadManager().DataUpdate.mutex );
      Manager::getInstance()->getThreadManager().DataUpdate.data = true;
    }
    Manager::getInstance()->getThreadManager().DataUpdate.variable.notify_all();
  }


  void DataManager::unload( DataHandler* handler )
  {
    DEBUG_STREAM << "UNLOAD";
    _unloadQueue.push( handler );

    {
      std::lock_guard<std::mutex>( Manager::getInstance()->getThreadManager().DataUpdate.mutex );
      Manager::getInstance()->getThreadManager().DataUpdate.data = true;
    }
    Manager::getInstance()->getThreadManager().DataUpdate.variable.notify_all();
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Configure

  void DataManager::configure( Json::Value& json_data )
  {
    INFO_LOG( "Configuring the Data Manager." );

    // Load the json data
    Utilities::validateJson( json_data, "resource_index_file", Utilities::JSON_TYPE_STRING );

    // Find the texture index file
    _indexFile = json_data["resource_index_file"].asString();

    // Load and validate the index file
    Json::Value temp_data;
    Utilities::loadJsonData( temp_data, _indexFile );
    Utilities::validateJson( temp_data, "textures", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJson( temp_data, "sounds", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJson( temp_data, "music", Utilities::JSON_TYPE_ARRAY );

    Utilities::validateJsonArray( temp_data["textures"], 0, Utilities::JSON_TYPE_ARRAY );
    Json::Value texture_data = temp_data["textures"];

//    Json::ArrayIndex i;
//    for ( i = 0; i < texture_data.size(); ++i )
//    {
//      Json::Value& datum = texture_data[i];
//      Utilities::validateJson( datum, "name", Utilities::JSON_TYPE_STRING );
//      std::string name = datum["name"].asString();
//
//      // Throw an error if there are duplicates.
//      if ( _rawTextures.find( name ) != _rawTextures.end() )
//      {
//        Exception ex( "DataManager::configure()", "Duplicate names found in texture list" );
//        ex.addDetail( "Name", name );
//        throw ex;
//      }
//
//      _rawTextures[name] = makeTexture( datum ); // Fill the info
//      SDL_DestroyTexture( _rawTextures[name].texture ); // Delete the texture data
//      _rawTextures[name].texture = nullptr; // Null
//    }

    temp_data.clear();
  }


  void DataManager::validate() const
  {
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Loading/unloading thread

  void dataLoadingThread()
  {
    INFO_LOG( "Data Manager loading thread start." );

    DataManager& manager = Manager::getInstance()->getDataManager();
    std::atomic<bool>& quitFlag = Manager::getInstance()->getThreadManager().QuitFlag;

    INFO_LOG( "Data Manager loading thread initialised and waiting to start" );
    {
      Condition<bool>& startCondition = Manager::getInstance()->getThreadManager().StartCondition;
      std::unique_lock<std::mutex> lk( startCondition.mutex );
      startCondition.variable.wait( lk, [&]()->bool{ return quitFlag || startCondition.data; } );
      lk.unlock();
    }
    INFO_LOG( "Data Manager loading thread go." );


    Condition<bool>& dataUpdate = Manager::getInstance()->getThreadManager().DataUpdate;
    std::unique_lock<std::mutex> dataLock( dataUpdate.mutex );

    INFO_LOG( "Loading thread waiting for first command" );
    while( ! quitFlag )
    {
      manager._loading = false;
      dataUpdate.variable.wait( dataLock, [&]()->bool{ return quitFlag || dataUpdate.data; } );
      manager._loading = true;
      dataUpdate.data = false;

      DEBUG_STREAM << "DATA LOADING THREAD WORKING";
      do
      {
        dataLock.unlock();

        dataUnloadFunction();
        dataLoadFunction();

        dataLock.lock();

      } while( dataUpdate.data == true );

      Manager::getInstance()->raiseEvent( REGOLITH_EVENT_DATA_LOADED );
    }

    dataLock.unlock();
    INFO_LOG( "Data Manager loading thread stopped." );
  }


  void dataLoadFunction()
  {
    DataManager& manager = Manager::getInstance()->getDataManager();
    DataHandler* temp_handler;

    if ( ! manager._loadQueue.empty() )
    {
      Json::Value temp_data;
      Utilities::loadJsonData( temp_data, manager._indexFile );
      Json::Value& texture_data = temp_data["textures"];

      while ( manager._loadQueue.pop( temp_handler ) )
      {
        if ( temp_handler->isLoaded() ) continue;

        RawTextureMap& textureCache = temp_handler->_rawTextures;
        RawTextureMap::iterator end = textureCache.end();
        for ( RawTextureMap::iterator it = textureCache.begin(); it != end; ++it )
        {
          std::string name = it->first;

          if ( ! texture_data.isMember( name ) )
          {
            ERROR_STREAM << "Could not find texture resource to load : " << name;
          }
          else
          {
            it->second = makeTexture( texture_data[ name ] );
            DEBUG_STREAM << "Loaded Texture: " << name;
          }
        }

        temp_handler->_isLoaded = true;
      }
    }
  }


  void dataUnloadFunction()
  {
    DataManager& manager = Manager::getInstance()->getDataManager();
    DataHandler* temp_handler;

    while ( manager._unloadQueue.pop( temp_handler ) )
    {
      if ( ! temp_handler->isLoaded() ) continue;

      RawTextureMap& textureCache = temp_handler->_rawTextures;
      RawTextureMap::iterator end = textureCache.end();
      for ( RawTextureMap::iterator it = textureCache.begin(); it != end; ++it )
      {
        std::string name = it->first;

        if ( it->second.texture != nullptr )
        {
          SDL_DestroyTexture( it->second.texture );
          it->second.texture = nullptr;
        }

        DEBUG_STREAM << "Unloaded texture: " << name;
      }

      temp_handler->_isLoaded = false;
    }
  }

}

