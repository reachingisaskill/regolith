//#define LOGTASTIC_DEBUG_OFF

#include "Regolith/Managers/DataManager.h"
#include "Regolith/Managers/ThreadManager.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Utilities/JsonValidation.h"

#include "logtastic.h"

#include <mutex>
#include <atomic>


namespace Regolith
{

  void dataLoadFunction();
  void dataUnloadFunction();


  DataManager::DataManager() :
    _loading( false ),
    _loadFlagMutex(),
    _indexFile(),
    _assets(),
    _loadQueue(),
    _unloadQueue()
  {
  }


  DataManager::~DataManager()
  {
    INFO_LOG( "Destroying Data Manager." );
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
    DEBUG_STREAM << "DATA MANAGER LOAD";
    _loadQueue.push( handler );

    {
      std::lock_guard<std::mutex>( Manager::getInstance()->getThreadManager().DataUpdate.mutex );
      Manager::getInstance()->getThreadManager().DataUpdate.data = true;
    }
    Manager::getInstance()->getThreadManager().DataUpdate.variable.notify_all();
  }


  void DataManager::unload( DataHandler* handler )
  {
    DEBUG_STREAM << "DATA MANAGER UNLOAD";
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


  const Asset& DataManager::getAsset( std::string name ) const
  {
    AssetMap::const_iterator found = _assets.find( name );
    if ( found == _assets.end() )
    {
      Exception ex( "DataManager::getAsset()", "Asset does not exist" );
      ex.addDetail( "Asset Name", name );
      throw ex;
    }
    return found->second;
  }


  RawTexture DataManager::buildRawTexture( std::string name ) const
  {
    AssetMap::const_iterator asset_found = _assets.find( name );
    if ( asset_found == _assets.end() )
    {
      Exception ex( "DataManager::buildRawTexture()", "Asset does not exist" );
      ex.addDetail( "Asset Name", name );
      throw ex;
    }

    switch ( asset_found->second.type )
    {
      case ASSET_TEXTURE :
        return RawTexture( nullptr, asset_found->second.textureDetail.width, asset_found->second.textureDetail.height, asset_found->second.textureDetail.rows, asset_found->second.textureDetail.columns );
        break;

      case ASSET_STRING :
        return RawTexture( nullptr, asset_found->second.stringDetail.width, asset_found->second.stringDetail.height, 1, 1 );
        break;

      default :
        Exception ex( "DataManager::buildRawTexture()", "Asset is not a texture" );
        ex.addDetail( "Asset Name", name );
        ex.addDetail( "Expected", "ASSET_TEXTURE or ASSET_STRING" );
        ex.addDetail( "Found", AssetTypeNames[asset_found->second.type] );
        throw ex;
        break;
    }
  }


  RawMusic DataManager::buildRawMusic( std::string name ) const
  {
    AssetMap::const_iterator asset_found = _assets.find( name );
    if ( asset_found == _assets.end() )
    {
      Exception ex( "DataManager::buildRawMusic()", "Asset does not exist" );
      ex.addDetail( "Asset Name", name );
      throw ex;
    }

    switch ( asset_found->second.type )
    {
      case ASSET_MUSIC :
        return RawMusic();
        break;

      case ASSET_SOUND :
        return RawMusic();
        break;

      default :
        Exception ex( "DataManager::buildRawMusic()", "Asset is not a music track" );
        ex.addDetail( "Asset Name", name );
        ex.addDetail( "Expected", "ASSET_MUSIC or ASSET_SOUND" );
        ex.addDetail( "Found", AssetTypeNames[asset_found->second.type] );
        throw ex;
        break;
    }
  }


  RawSound DataManager::buildRawSound( std::string name ) const
  {
    AssetMap::const_iterator asset_found = _assets.find( name );
    if ( asset_found == _assets.end() )
    {
      Exception ex( "DataManager::buildRawSound()", "Asset does not exist" );
      ex.addDetail( "Asset Name", name );
      throw ex;
    }

    switch ( asset_found->second.type )
    {
      case ASSET_SOUND :
        return RawSound();
        break;

      default :
        Exception ex( "DataManager::buildRawMusic()", "Asset is not a music track" );
        ex.addDetail( "Asset Name", name );
        ex.addDetail( "Expected", "ASSET_SOUND" );
        ex.addDetail( "Found", AssetTypeNames[asset_found->second.type] );
        throw ex;
        break;
    }
  }


  void DataManager::loadRawTexture( std::string name, RawTexture& texture ) const
  {
    AssetMap::const_iterator asset_found = _assets.find( name );
    if ( asset_found == _assets.end() )
    {
      Exception ex( "DataManager::loadRawTexture()", "Asset does not exist" );
      ex.addDetail( "Asset Name", name );
      throw ex;
    }

    switch ( asset_found->second.type )
    {
      case ASSET_TEXTURE :
        texture.surface = loadSurfaceFromFile( asset_found->second.textureDetail.filename, asset_found->second.textureDetail.colourkey );
        break;

      case ASSET_STRING :
        texture.surface = loadSurfaceFromString( asset_found->second.stringDetail.text, asset_found->second.stringDetail.font, asset_found->second.stringDetail.colour );
        break;

      default :
        Exception ex( "DataManager::loadRawTexture()", "Asset is not a texture" );
        ex.addDetail( "Asset Name", name );
        ex.addDetail( "Expected", "ASSET_TEXTURE or ASSET_STRING" );
        ex.addDetail( "Found", AssetTypeNames[asset_found->second.type] );
        throw ex;
        break;
    }
  }


  void DataManager::loadRawMusic( std::string name, RawMusic& music ) const
  {
    AssetMap::const_iterator asset_found = _assets.find( name );
    if ( asset_found == _assets.end() )
    {
      Exception ex( "DataManager::buildRawMusic()", "Asset does not exist" );
      ex.addDetail( "Asset Name", name );
      throw ex;
    }

    switch ( asset_found->second.type )
    {
      case ASSET_MUSIC :
        music.music = loadMusic( asset_found->second.musicDetail.filename );
        break;

      case ASSET_SOUND :
        music.music = loadMusic( asset_found->second.soundDetail.filename );
        break;

      default :
        Exception ex( "DataManager::loadRawMusic()", "Asset is not a music track" );
        ex.addDetail( "Asset Name", name );
        ex.addDetail( "Expected", "ASSET_MUSIC or ASSET_SOUND" );
        ex.addDetail( "Found", AssetTypeNames[asset_found->second.type] );
        throw ex;
        break;
    }
  }


  void DataManager::loadRawSound( std::string name, RawSound& sound ) const
  {
    AssetMap::const_iterator asset_found = _assets.find( name );
    if ( asset_found == _assets.end() )
    {
      Exception ex( "DataManager::loadRawSound()", "Asset does not exist" );
      ex.addDetail( "Asset Name", name );
      throw ex;
    }

    switch ( asset_found->second.type )
    {
      case ASSET_SOUND :
        sound.sound = loadSound( asset_found->second.soundDetail.filename );
        break;

      default :
        Exception ex( "DataManager::loadRawSound()", "Asset is not a sound file" );
        ex.addDetail( "Asset Name", name );
        ex.addDetail( "Expected", "ASSET_SOUND" );
        ex.addDetail( "Found", AssetTypeNames[asset_found->second.type] );
        throw ex;
        break;
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
      TextureDetail detail;

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
      _assets.insert( { name, Asset( detail ) } );
//      _assets[name] = Asset( detail );
      DEBUG_STREAM << "ASSET Texture: " << name;
    }

    for ( Json::Value::iterator it = index_data["strings"].begin(); it != index_data["strings"].end(); ++it )
    {
      Json::Value& data = *it;
      StringDetail detail;

      std::string font_name = data["font"].asString();

      std::string name = it.key().asString();
      detail.text = data["text"].asString();
      detail.font = Manager::getInstance()->getFontPointer( font_name );
      detail.colour.r = data["colour"][0].asInt();
      detail.colour.g = data["colour"][1].asInt();
      detail.colour.b = data["colour"][2].asInt();
      detail.colour.a = data["colour"][3].asInt();

      SDL_Surface* textSurface = TTF_RenderText_Solid( detail.font, detail.text.c_str(), detail.colour );
      if ( textSurface == nullptr )
      {
        Exception ex( "DataManager::configure()", "Could not render text" );
        ex.addDetail( "Text string", detail.text.c_str() );
        ex.addDetail( "Font", detail.font );
        ex.addDetail( "SDL_TTF Error", TTF_GetError() );
        throw ex;
      }
      detail.width = textSurface->w;
      detail.height = textSurface->h;
      SDL_FreeSurface( textSurface );

      _assets.insert( std::make_pair( name, Asset( detail ) ) );
      DEBUG_STREAM << "ASSET String: " << name;
    }

    for ( Json::Value::iterator it = index_data["music"].begin(); it != index_data["music"].end(); ++it )
    {
      Json::Value& data = *it;
      MusicDetail detail;

      std::string name = it.key().asString();
      detail.filename = data["path"].asString();

      _assets.insert( std::make_pair( name, Asset( detail ) ) );
      DEBUG_STREAM << "ASSET Music: " << name;
    }


    for ( Json::Value::iterator it = index_data["sounds"].begin(); it != index_data["sounds"].end(); ++it )
    {
      Json::Value& data = *it;
      SoundDetail detail;

      std::string name = it.key().asString();
      detail.filename = data["path"].asString();

      _assets.insert( std::make_pair( name, Asset( detail ) ) );
      DEBUG_STREAM << "ASSET Sound: " << name;
    }


//    for ( Json::Value::iterator it = index_data["font"].begin(); it != index_data["font"].end(); ++it )
//    {
//      Json::Value& data = *it;
//      FontDetail detail;
//
//      std::string name = it.key().asString();
//      detail.filename = data["filename"].asString();
//
//      _assets.insert( std::make_pair( name, Asset( detail ) ) );
//    }

  }


  void DataManager::validate() const
  {
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Loading/unloading thread

  void dataManagerLoadingThread()
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

    try
    {

      INFO_LOG( "Loading thread waiting for first command" );
      while( ! quitFlag )
      {
        dataUpdate.variable.wait( dataLock, [&]()->bool{ return quitFlag || dataUpdate.data; } );
        if ( quitFlag ) break;

        manager.setLoading( true );

        DEBUG_LOG( "DATA LOADING THREAD WORKING" );
        do
        {
          dataUpdate.data = false;

          dataLock.unlock();

          dataUnloadFunction();
          dataLoadFunction();

          dataLock.lock();

        } while( dataUpdate.data == true );
        DEBUG_LOG( "DATA LOADING THREAD FINISHED" );

        dataUpdate.variable.notify_all();
      }

      dataLock.unlock();
    }
    catch ( const Exception& ex )
    {
      Manager::getInstance()->getThreadManager().error();
      if ( dataLock.owns_lock() )
      {
        dataUpdate.variable.notify_all();
        dataLock.unlock();
      }
      FAILURE_LOG( "Regolith Exception thrown from Data Manager Thread." );
      std::cerr << ex.elucidate();
    }
    catch( const std::exception& ex )
    {
      Manager::getInstance()->getThreadManager().error();
      if ( dataLock.owns_lock() )
      {
        dataUpdate.variable.notify_all();
        dataLock.unlock();
      }
      FAILURE_LOG( "Standard Exception thrown from Data Manager Thread." );
      std::cerr << ex.what();
    }

    INFO_LOG( "Data Manager loading thread stopped." );
  }


  void dataLoadFunction()
  {
    DataManager& manager = Manager::getInstance()->getDataManager();
    DataHandler* temp_handler;

    while ( manager._loadQueue.pop( temp_handler ) )
    {
      try
      {
        temp_handler->load();
      }
      catch( Exception& ex )
      {
        std::cerr << ex.elucidate();
        Manager::getInstance()->getThreadManager().error();
      }
    }
  }


  void dataUnloadFunction()
  {
    DataManager& manager = Manager::getInstance()->getDataManager();
    DataHandler* temp_handler;

    while ( manager._unloadQueue.pop( temp_handler ) )
    {
      try
      {
        temp_handler->unload();
      }
      catch( Exception& ex )
      {
        std::cerr << ex.elucidate();
        Manager::getInstance()->getThreadManager().error();
      }
    }
  }

}
