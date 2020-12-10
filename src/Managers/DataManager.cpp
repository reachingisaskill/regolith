
#include "Regolith/Managers/DataManager.h"
#include "Regolith/Managers/ThreadManager.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Managers/ThreadHandler.h"
#include "Regolith/Utilities/JsonValidation.h"
#include "Regolith/GamePlay/Font.h"
#include "Regolith/GamePlay/Texture.h"
#include "Regolith/GamePlay/Noises.h"

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
    INFO_LOG( "DataManager::~DataManager : Destructing" );
  }


  void DataManager::clear()
  {
    INFO_LOG( "DataManager::clear : Clearing Data Manager." );

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
    DEBUG_LOG( "DataManager::load : Loading handler" );
    _loadQueue.push( handler );

    {
      std::lock_guard<std::mutex>( Manager::getInstance()->getThreadManager().DataUpdate.mutex );
      Manager::getInstance()->getThreadManager().DataUpdate.data = true;
    }
    Manager::getInstance()->getThreadManager().DataUpdate.variable.notify_all();
  }


  void DataManager::unload( DataHandler* handler )
  {
    DEBUG_LOG( "DataManager::unload : Unloading Handler" );
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
      case ASSET_IMAGE :
        return RawTexture( nullptr, asset_found->second.imageDetail.width, asset_found->second.imageDetail.height, asset_found->second.imageDetail.rows, asset_found->second.imageDetail.columns );
        break;

      case ASSET_TEXT :
        return RawTexture( nullptr, asset_found->second.textDetail.width, asset_found->second.textDetail.height, 1, 1 );
        break;

      default :
        Exception ex( "DataManager::buildRawTexture()", "Asset is not a texture" );
        ex.addDetail( "Asset Name", name );
        ex.addDetail( "Expected", "ASSET_IMAGE or ASSET_TEXT" );
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
      case ASSET_AUDIO :
        return RawMusic();
        break;

      default :
        Exception ex( "DataManager::buildRawMusic()", "Asset is not a music track" );
        ex.addDetail( "Asset Name", name );
        ex.addDetail( "Expected", "ASSET_AUDIO" );
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
      case ASSET_AUDIO :
        return RawSound();
        break;

      default :
        Exception ex( "DataManager::buildRawMusic()", "Asset is not a sound" );
        ex.addDetail( "Asset Name", name );
        ex.addDetail( "Expected", "ASSET_AUDIO" );
        ex.addDetail( "Found", AssetTypeNames[asset_found->second.type] );
        throw ex;
        break;
    }
  }


  RawFont DataManager::buildRawFont( std::string name ) const
  {
    AssetMap::const_iterator asset_found = _assets.find( name );
    if ( asset_found == _assets.end() )
    {
      Exception ex( "DataManager::buildRawFont()", "Asset does not exist" );
      ex.addDetail( "Asset Name", name );
      throw ex;
    }

    switch ( asset_found->second.type )
    {
      case ASSET_FONT :
        return RawFont();
        break;

      default :
        Exception ex( "DataManager::buildRawFont()", "Asset is not a font" );
        ex.addDetail( "Asset Name", name );
        ex.addDetail( "Expected", "ASSET_FONT" );
        ex.addDetail( "Found", AssetTypeNames[asset_found->second.type] );
        throw ex;
        break;
    }
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Load raw asset functions

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
      case ASSET_IMAGE :
        texture.surface = loadSurfaceFromFile( asset_found->second.imageDetail.filename, asset_found->second.imageDetail.colourkey );
        break;

//      case ASSET_TEXT :
//        {
//          AssetMap::const_iterator font_found = _assets.find( asset_found->second.textDetail.font );
//          if ( font_found == _assets.end() || font_found->second.type != ASSET_FONT )
//          {
//            Exception ex( "DataManager::loadRawTexture()", "Requested font not found" );
//            ex.addDetail( "Asset Name", name );
//            ex.addDetail( "Font Name", asset_found->second.textDetail.font );
//            throw ex;
//          }
//
//          texture.surface = loadSurfaceFromString( asset_found->second.textDetail.text, asset_found->second.textDetail.font, asset_found->second.textDetail.colour );
//        }
//        break;

      default :
        Exception ex( "DataManager::loadRawTexture()", "Asset is not a texture" );
        ex.addDetail( "Asset Name", name );
        ex.addDetail( "Expected", "ASSET_IMAGE or ASSET_TEXT" );
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
      case ASSET_AUDIO :
        music.music = loadMusic( asset_found->second.audioDetail.filename );
        break;

      default :
        Exception ex( "DataManager::loadRawMusic()", "Asset is not a music track" );
        ex.addDetail( "Asset Name", name );
        ex.addDetail( "Expected", "ASSET_AUDIO" );
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
      case ASSET_AUDIO :
        sound.sound = loadSound( asset_found->second.audioDetail.filename );
        break;

      default :
        Exception ex( "DataManager::loadRawSound()", "Asset is not a sound file" );
        ex.addDetail( "Asset Name", name );
        ex.addDetail( "Expected", "ASSET_AUDIO" );
        ex.addDetail( "Found", AssetTypeNames[asset_found->second.type] );
        throw ex;
        break;
    }
  }


  void DataManager::loadRawFont( std::string name, RawFont& font ) const
  {
    AssetMap::const_iterator asset_found = _assets.find( name );
    if ( asset_found == _assets.end() )
    {
      Exception ex( "DataManager::loadRawFont()", "Asset does not exist" );
      ex.addDetail( "Asset Name", name );
      throw ex;
    }

    switch ( asset_found->second.type )
    {
      case ASSET_FONT :
        font.ttf_font = loadFont( asset_found->second.fontDetail.filename, asset_found->second.fontDetail.size );
        break;

      default :
        Exception ex( "DataManager::loadRawFont()", "Asset is not a font file" );
        ex.addDetail( "Asset Name", name );
        ex.addDetail( "Expected", "ASSET_FONT" );
        ex.addDetail( "Found", AssetTypeNames[asset_found->second.type] );
        throw ex;
        break;
    }
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Configure

  void DataManager::configure( Json::Value& json_data )
  {
    INFO_LOG( "DataManager::configure : Configuring the Data Manager." );

    // Load the json data
    Utilities::validateJson( json_data, "resource_index_file", Utilities::JSON_TYPE_STRING );

    // Find the texture index file
    _indexFile = json_data["resource_index_file"].asString();

    // Load and validate the index file
    Json::Value index_data;
    Utilities::loadJsonData( index_data, _indexFile );
    Utilities::validateJson( index_data, "fonts", Utilities::JSON_TYPE_OBJECT );
    Utilities::validateJson( index_data, "images", Utilities::JSON_TYPE_OBJECT );
    Utilities::validateJson( index_data, "text", Utilities::JSON_TYPE_OBJECT );
    Utilities::validateJson( index_data, "audio", Utilities::JSON_TYPE_OBJECT );


    // Load details of all the font assets
    for ( Json::Value::iterator it = index_data["fonts"].begin(); it != index_data["fonts"].end(); ++it )
    {
      Json::Value& data = *it;
      FontDetail detail;

      std::string name = it.key().asString();
      detail.filename = data["path"].asString();
      detail.size = data["size"].asInt();
      detail.colour.r = data["colour"][0].asInt();
      detail.colour.g = data["colour"][1].asInt();
      detail.colour.b = data["colour"][2].asInt();
      detail.colour.a = data["colour"][3].asInt();

      _assets.insert( std::make_pair( name, Asset( detail ) ) );
    }

    // Load details of all the image assets
    for ( Json::Value::iterator it = index_data["images"].begin(); it != index_data["images"].end(); ++it )
    {
      Json::Value& data = *it;
      ImageDetail detail;

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
      _assets.insert( std::make_pair( name, Asset( detail ) ) );
      DEBUG_STREAM << "DataManager::configure : Asset Texture: " << name;
    }

    // Load details of all the text assets
    for ( Json::Value::iterator it = index_data["text"].begin(); it != index_data["text"].end(); ++it )
    {
      Json::Value& data = *it;
      TextDetail detail;

      std::string name = it.key().asString();
      detail.text = data["filename"].asString();
      detail.font = data["font"].asString();

      AssetMap::iterator font_it = _assets.find( detail.font );
      if ( font_it == _assets.end() || font_it->second.type != ASSET_FONT )
      {
        Exception ex( "DataManager::configure()", "Expected font not found" );
        ex.addDetail( "Font Name", detail.font );
        throw ex;
      }

      _assets.insert( std::make_pair( name, Asset( detail ) ) );
      DEBUG_STREAM << "DataManager::configure : Asset String: " << name;
    }

    // Load details of all the audio assets
    for ( Json::Value::iterator it = index_data["audio"].begin(); it != index_data["audio"].end(); ++it )
    {
      Json::Value& data = *it;
      AudioDetail detail;

      std::string name = it.key().asString();
      detail.filename = data["path"].asString();

      _assets.insert( std::make_pair( name, Asset( detail ) ) );
      DEBUG_STREAM << "DataManager::configure : Asset Sound: " << name;
    }
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Loading/unloading thread

  void dataManagerLoadingThread()
  {
    ThreadHandler threadHandler( "DataManagerThread", REGOLITH_THREAD_DATA );

    // Wait on the start condition
    threadHandler.start();


    // Get references to required data
    DataManager& manager = Manager::getInstance()->getDataManager();
    Condition<bool>& dataUpdate = Manager::getInstance()->getThreadManager().DataUpdate;
    std::unique_lock<std::mutex> dataLock( dataUpdate.mutex );


    // Update the thread status
    threadHandler.running();

    try
    {
      while( threadHandler.isGood() )
      {
        dataUpdate.variable.wait( dataLock, [&]()->bool{ return (! threadHandler.isGood()) || dataUpdate.data; } );

        manager.setLoading( true );

        DEBUG_LOG( "dataManagerLoadingThread : Working..." );
        do
        {
          dataUpdate.data = false;

          dataLock.unlock();

          dataUnloadFunction();
          dataLoadFunction();

          dataLock.lock();

        } while( dataUpdate.data == true );
        DEBUG_LOG( "dataManagerLoadingThread : Finished" );

        dataUpdate.variable.notify_all();
      }

      dataLock.unlock();
    }
    catch ( Exception& ex )
    {
      if ( dataLock.owns_lock() )
      {
        dataUpdate.variable.notify_all();
        dataLock.unlock();
      }
      threadHandler.throwError( ex );
      return;
    }
    catch( std::exception& ex )
    {
      if ( dataLock.owns_lock() )
      {
        dataUpdate.variable.notify_all();
        dataLock.unlock();
      }
      threadHandler.throwError( ex );
      return;
    }

    threadHandler.closing();

    // Do any closing operations here

    threadHandler.stop();
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

