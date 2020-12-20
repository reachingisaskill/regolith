
#include "Regolith/Managers/DataManager.h"
#include "Regolith/Managers/ThreadManager.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Managers/ThreadHandler.h"
#include "Regolith/Utilities/JsonValidation.h"
#include "Regolith/Assets/RawFont.h"
#include "Regolith/Assets/RawText.h"
#include "Regolith/Assets/RawTexture.h"
#include "Regolith/Assets/RawMusic.h"
#include "Regolith/Assets/RawSound.h"

#include <mutex>
#include <atomic>


namespace Regolith
{

  DataManager::DataManager() :
    _indexFile(),
    _assets()
  {
  }


  DataManager::~DataManager()
  {
    INFO_LOG( "DataManager::~DataManager : Destructing" );
  }


  void DataManager::clear()
  {
    INFO_LOG( "DataManager::clear : Clearing Data Manager." );
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
        return loadRawTexture( asset_found->second.imageDetail );
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
        return loadRawMusic( asset_found->second.audioDetail );
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
        return loadRawSound( asset_found->second.audioDetail );
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
        return loadRawFont( asset_found->second.fontDetail );
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


  RawText DataManager::buildRawText( std::string name ) const
  {
    AssetMap::const_iterator asset_found = _assets.find( name );
    if ( asset_found == _assets.end() )
    {
      Exception ex( "DataManager::buildRawText()", "Asset does not exist" );
      ex.addDetail( "Asset Name", name );
      throw ex;
    }

    switch ( asset_found->second.type )
    {
      case ASSET_TEXT :
        return loadRawText( asset_found->second.textDetail );
        break;

      default :
        Exception ex( "DataManager::buildRawText()", "Asset is not a text" );
        ex.addDetail( "Asset Name", name );
        ex.addDetail( "Expected", "ASSET_TEXT" );
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
      DEBUG_STREAM << "DataManager::configure : Asset Font: " << name;
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
      detail.filename = data["filename"].asString();

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

    DEBUG_STREAM << "DataManager::configure : Built asset map. Size = " << _assets.size();
  }

}

