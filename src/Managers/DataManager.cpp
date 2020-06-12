
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
    _textureDetails(),
    _stringDetails(),
    _musicDetails(),
    _soundDetails(),
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


  RawTexture DataManager::buildRawTexture( std::string name ) const
  {
    RawTextureDetailMap::const_iterator texture_found = _textureDetails.find( name );
    if ( texture_found != _textureDetails.end() )
    {
      return RawTexture( nullptr, texture_found->second.width, texture_found->second.height, texture_found->second.rows, texture_found->second.columns );
    }
    else
    {
      RawStringDetailMap::const_iterator string_found = _stringDetails.find( name );
      if ( string_found != _stringDetails.end() )
      {
        return RawTexture( nullptr, string_found->second.width, string_found->second.height, 1, 1 );
      }
      else
      {
        ERROR_STREAM << "Could not find requested texture name: " << name;
        return RawTexture();
      }
    }
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
    Json::Value index_data;
    Utilities::loadJsonData( index_data, _indexFile );
    Utilities::validateJson( index_data, "textures", Utilities::JSON_TYPE_OBJECT );
    Utilities::validateJson( index_data, "strings", Utilities::JSON_TYPE_OBJECT );
    Utilities::validateJson( index_data, "sounds", Utilities::JSON_TYPE_OBJECT );
    Utilities::validateJson( index_data, "music", Utilities::JSON_TYPE_OBJECT );
//    Utilities::validateJson( index_data, "fonts", Utilities::JSON_TYPE_OBJECT );

    for ( Json::Value::iterator it = index_data["textures"].begin(); it != index_data["textures"].end(); ++it )
    {
      Json::Value& data = *it;
      RawTextureDetail detail;

      std::string name = it.key().asString();
      detail.filename = data["path"].asString();
      detail.width = data["width"].asInt();
      detail.height = data["height"].asInt();
      detail.rows = data["rows"].asInt();
      detail.columns = data["columns"].asInt();
      if ( data.isMember( "colour_key" ) )
      {
        detail.colourkey.r = data["colour_key"][0].asInt();
        detail.colourkey.g = data["colour_key"][1].asInt();
        detail.colourkey.b = data["colour_key"][2].asInt();
        detail.colourkey.a = 255;
      }
      else
      {
        detail.colourkey = { 0, 0, 0, 0 };
      }
      _textureDetails.insert( std::make_pair( name, detail ) );
      DEBUG_STREAM << "INDEX Texture: " << name;
    }

    for ( Json::Value::iterator it = index_data["strings"].begin(); it != index_data["strings"].end(); ++it )
    {
      Json::Value& data = *it;
      RawStringDetail detail;

      std::string name = it.key().asString();
      detail.text = data["text"].asString();
      detail.font = data["font"].asString();
      detail.colour.r = data["colour"][0].asInt();
      detail.colour.g = data["colour"][1].asInt();
      detail.colour.b = data["colour"][2].asInt();
      detail.colour.a = data["colour"][3].asInt();
      detail.width = 0;
      detail.height = 0;
      _stringDetails.insert( std::make_pair( name, detail ) );
      DEBUG_STREAM << "INDEX String: " << name;
    }

    for ( Json::Value::iterator it = index_data["music"].begin(); it != index_data["music"].end(); ++it )
    {
      Json::Value& data = *it;
      RawMusicDetail detail;

      std::string name = it.key().asString();
      detail.filename = data["path"].asString();

      _musicDetails.insert( std::make_pair( name, detail ) );
      DEBUG_STREAM << "INDEX Music: " << name;
    }


    for ( Json::Value::iterator it = index_data["sounds"].begin(); it != index_data["sounds"].end(); ++it )
    {
      Json::Value& data = *it;
      RawSoundDetail detail;

      std::string name = it.key().asString();
      detail.filename = data["path"].asString();

      _soundDetails.insert( std::make_pair( name, detail ) );
      DEBUG_STREAM << "INDEX Sound: " << name;
    }


//    for ( Json::Value::iterator it = index_data["font"].begin(); it != index_data["font"].end(); ++it )
//    {
//      Json::Value& data = *it;
//      RawFontDetail detail;
//
//      std::string name = it.key().asString();
//      detail.filename = data["filename"].asString();
//
//      _fontDetails.insert( std::make_pair( name, detail ) );
//    }

  }


  void DataManager::validate()
  {
    for ( RawStringDetailMap::iterator it = _stringDetails.begin(); it != _stringDetails.end(); ++it )
    {
      RawStringDetail& data = it->second;

      TTF_Font* font = Manager::getInstance()->getFontPointer( data.font );
      SDL_Surface* textSurface = TTF_RenderText_Solid( font, data.text.c_str(), data.colour );
      if ( textSurface == nullptr )
      {
        Exception ex( "DataManager::configure()", "Could not render text" );
        ex.addDetail( "Text string", data.text.c_str() );
        ex.addDetail( "Font", data.font );
        ex.addDetail( "SDL_TTF Error", TTF_GetError() );
        throw ex;
      }

      data.width = textSurface->w;
      data.height = textSurface->h;
      SDL_FreeSurface( textSurface );
    }
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
      while ( manager._loadQueue.pop( temp_handler ) )
      {
        if ( temp_handler->isLoaded() ) continue;

        RawTextureMap& textureCache = temp_handler->_rawTextures;
        RawTextureMap::iterator texture_end = textureCache.end();
        for ( RawTextureMap::iterator it = textureCache.begin(); it != texture_end; ++it )
        {
          std::string name = it->first;
          RawTextureDetailMap::iterator texture_found = manager._textureDetails.find( name );
          if ( texture_found != manager._textureDetails.end() )
          {
            DEBUG_STREAM << "Loaded Texture: " << name << " - " << it->second.width << ", " << it->second.height << ", " << it->second.cells << " @ " << it->second.texture;
            it->second = makeTextureFromFile( texture_found->second );
//            it->second = makeTextureFromFile( texture_data[ name ] );
            DEBUG_STREAM << "Loaded Texture: " << name << " - " << it->second.width << ", " << it->second.height << ", " << it->second.cells << " @ " << it->second.texture;
          }
          else
          {
            RawStringDetailMap::iterator string_found = manager._stringDetails.find( name );
            if ( string_found != manager._stringDetails.end() )
            {
              DEBUG_STREAM << "Loaded Texture: " << name << " - " << it->second.width << ", " << it->second.height << ", " << it->second.cells << " @ " << it->second.texture;
              it->second = makeTextureFromText( string_found->second );
  //            it->second = makeTextureFromText( string_data[ name ] );
              DEBUG_STREAM << "Loaded Texture: " << name << " - " << it->second.width << ", " << it->second.height << ", " << it->second.cells << " @ " << it->second.texture;
            }
            else
            {
              ERROR_STREAM << "Could not find texture resource to load : " << name;
            }
          }
        }


        RawMusicMap& musicCache = temp_handler->_rawMusic;
        RawMusicMap::iterator music_end = musicCache.end();
        for ( RawMusicMap::iterator it = musicCache.begin(); it != music_end; ++it )
        {
          std::string name = it->first;
          RawMusicDetailMap::iterator music_found = manager._musicDetails.find( name );

          if ( music_found == manager._musicDetails.end() )
          {
            ERROR_STREAM << "Could not find music resource to load : " << name;
          }
          else
          {
            it->second = makeMusic( music_found->second );
            DEBUG_STREAM << "Loaded Music: " << name;
          }
        }


        RawSoundMap& soundCache = temp_handler->_rawSounds;
        RawSoundMap::iterator sound_end = soundCache.end();
        for ( RawSoundMap::iterator it = soundCache.begin(); it != sound_end; ++it )
        {
          std::string name = it->first;
          RawSoundDetailMap::iterator sound_found = manager._soundDetails.find( name );

          if ( sound_found == manager._soundDetails.end() )
          {
            ERROR_STREAM << "Could not find sound resource to load : " << name;
          }
          else
          {
            it->second = makeSound( sound_found->second );
            DEBUG_STREAM << "Loaded Sound: " << name;
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

