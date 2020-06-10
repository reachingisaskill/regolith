
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
    _loading( false ),
    _loadFlagMutex(),
    _loadingThread( dataLoadingThread ),
    _indexFile(),
    _loadQueue(),
    _unloadQueue()
  {
  }


  DataManager::~DataManager()
  {
    INFO_LOG( "Destroying Data Manager." );
    _loadingThread.join();
  }


  void DataManager::clear()
  {
    INFO_LOG( "Clearing Data Manager." );

    _loadQueue.clear();
    _unloadQueue.clear();
  }


  void DataManager::setLoading( bool value )
  {
    std::lock_guard<std::mutex> gl( _loadFlagMutex );

    _loading = value;
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


  bool DataManager::isLoading() const
  {
    std::lock_guard<std::mutex> gl( _loadFlagMutex );

    return _loading;
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
    Utilities::validateJson( temp_data, "textures", Utilities::JSON_TYPE_OBJECT );
    Utilities::validateJson( temp_data, "sounds", Utilities::JSON_TYPE_OBJECT );
    Utilities::validateJson( temp_data, "music", Utilities::JSON_TYPE_OBJECT );

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

    std::atomic<bool>& quitFlag = Manager::getInstance()->getThreadManager().QuitFlag;

    INFO_LOG( "Data Manager loading thread initialised and waiting to start" );
    {
      Condition<bool>& startCondition = Manager::getInstance()->getThreadManager().StartCondition;
      std::unique_lock<std::mutex> lk( startCondition.mutex );
      startCondition.variable.wait( lk, [&]()->bool{ return quitFlag || startCondition.data; } );
      lk.unlock();
    }
    INFO_LOG( "Data Manager loading thread go." );


    DataManager& manager = Manager::getInstance()->getDataManager();
    Condition<bool>& dataUpdate = Manager::getInstance()->getThreadManager().DataUpdate;
    std::unique_lock<std::mutex> dataLock( dataUpdate.mutex );

    INFO_LOG( "Loading thread waiting for first command" );
    while( ! quitFlag )
    {
      if ( ! dataUpdate.data )
      {
        manager.setLoading( false );
        dataUpdate.variable.wait( dataLock, [&]()->bool{ return quitFlag || dataUpdate.data; } );
        if ( quitFlag ) break;
      }

      manager.setLoading( true );

      DEBUG_STREAM << "DATA LOADING THREAD WORKING";
      do
      {
        dataUpdate.data = false;

        dataLock.unlock();

        dataUnloadFunction();
        dataLoadFunction();

        dataLock.lock();

      } while( dataUpdate.data == true );

      dataUpdate.variable.notify_all();
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
      Json::Value& music_data = temp_data["music"];
      Json::Value& sound_data = temp_data["sounds"];

      while ( manager._loadQueue.pop( temp_handler ) )
      {
        if ( temp_handler->isLoaded() ) continue;

        RawTextureMap& textureCache = temp_handler->_rawTextures;
        RawTextureMap::iterator texture_end = textureCache.end();
        for ( RawTextureMap::iterator it = textureCache.begin(); it != texture_end; ++it )
        {
          std::string name = it->first;

          if ( ! texture_data.isMember( name ) )
          {
            ERROR_STREAM << "Could not find texture resource to load : " << name;
          }
          else
          {
            DEBUG_STREAM << "Loaded Texture: " << name << " - " << it->second.width << ", " << it->second.height << " @ " << it->second.texture;
            it->second = makeTexture( texture_data[ name ] );
            DEBUG_STREAM << "Loaded Texture: " << name << " - " << it->second.width << ", " << it->second.height << " @ " << it->second.texture;
          }
        }


        RawMusicMap& musicCache = temp_handler->_rawMusic;
        RawMusicMap::iterator music_end = musicCache.end();
        for ( RawMusicMap::iterator it = musicCache.begin(); it != music_end; ++it )
        {
          std::string name = it->first;

          if ( ! music_data.isMember( name ) )
          {
            ERROR_STREAM << "Could not find music resource to load : " << name;
          }
          else
          {
            it->second = makeMusic( music_data[ name ] );
            DEBUG_STREAM << "Loaded Music: " << name;
          }
        }


        RawSoundMap& soundCache = temp_handler->_rawSounds;
        RawSoundMap::iterator sound_end = soundCache.end();
        for ( RawSoundMap::iterator it = soundCache.begin(); it != sound_end; ++it )
        {
          std::string name = it->first;

          if ( ! sound_data.isMember( name ) )
          {
            ERROR_STREAM << "Could not find sound resource to load : " << name;
          }
          else
          {
            it->second = makeSound( sound_data[ name ] );
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
      RawTextureMap::iterator textures_end = textureCache.end();
      for ( RawTextureMap::iterator it = textureCache.begin(); it != textures_end; ++it )
      {
        std::string name = it->first;

        if ( it->second.texture != nullptr )
        {
          SDL_DestroyTexture( it->second.texture );
          it->second.texture = nullptr;
        }

        DEBUG_STREAM << "Unloaded texture: " << name;
      }

      RawMusicMap& musicCache = temp_handler->_rawMusic;
      RawMusicMap::iterator music_end = musicCache.end();
      for ( RawMusicMap::iterator it = musicCache.begin(); it != music_end; ++it )
      {
        std::string name = it->first;

        if ( it->second.music != nullptr )
        {
          Mix_FreeMusic( it->second.music );
          it->second.music = nullptr;
        }

        DEBUG_STREAM << "Unloaded music: " << name;
      }

      RawSoundMap& soundCache = temp_handler->_rawSounds;
      RawSoundMap::iterator sounds_end = soundCache.end();
      for ( RawSoundMap::iterator it = soundCache.begin(); it != sounds_end; ++it )
      {
        std::string name = it->first;

        if ( it->second.sound != nullptr )
        {
          Mix_FreeChunk( it->second.sound );
          it->second.sound = nullptr;
        }

        DEBUG_STREAM << "Unloaded sound: " << name;
      }

      temp_handler->_isLoaded = false;
    }
  }

}

