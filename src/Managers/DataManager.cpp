
#include "Regolith/Managers/DataManager.h"
#include "Regolith/Utilities/JsonValidation.h"

#include "logtastic.h"


namespace Regolith
{

  DataManager::DataManager() :
    _textureFile(),
    _rawTextures( "raw_textures" ),
    _gameObjects( "game_objects" ),
    _globalData(),
    _dataHandlers( "data_handlers" )
  {
  }


  DataManager::~DataManager()
  {
    INFO_LOG( "Deleting Data Manager." );

    INFO_LOG( "Clearing data handlers." )
    _dataHandlers.clear();

    INFO_LOG( "Clearing raw texture data" );
    for ( NamedVector< RawTexture, false >::iterator it = _rawTextures.begin(); it != _rawTextures.end(); ++it )
    {
      if ( it->texture != nullptr )
        SDL_DestroyTexture( it->texture );
    }
    _rawTextures.clear();

    INFO_LOG( "Data manager deleted." );
  }


  unsigned int DataManager::requestHandler( std::string name )
  {
    if ( ! _dataHandlers.exists( name ) )
    {
      _dataHandlers.addObject( new DataHandler( _gameObjects ), name );
    }
    return _dataHandlers.get( name );
  }


  void DataManager::load( unsigned int i )
  {
    DataBuilder builder( *this, _textureFile );

    _dataHandlers[i].load( builder );
  }


  void DataManager::unload( unsigned int i )
  {
    DataBuilder builder( *this, _textureFile );

    _dataHandlers[i].unload( builder );
  }


  RawTexture* DataManager::getRawTexture( unsigned int i )
  {
    return _rawTextures[i];
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Data Builder member functions

  DataManager::DataBuilder::DataBuilder( DataManager& manager, std::string data_file ) :
    _manager( manager ),
    _data()
  {
    Json::Value temp_data;
    Utilities::loadJsonData( temp_data, data_file );
    Utilities::validateJson( temp_data, "textures", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJsonArray( temp_data["textures"], 0, Utilities::JSON_TYPE_ARRAY );
    _data = temp_data["textures"];
  }


  DataManager::DataBuilder::loadTexture( unsigned int id )
  {
    std::string name = _manager._rawTextures.getName( id );

    for ( Json::ArrayIndex i = 0; i < _data.size(); ++i )
    {
      Json::Value& datum = _data[i];
      Utilities::validateJson( datum, "name", Utilities::JSON_TYPE_STRING );
      if ( datum["name"] == name )
      {
        _rawTextures.set( id, temp = makeTexture( datum ) );
        return;
      }
    }
    Exception ex( "DataManager::DataBuilder::loadTexture()", "Could not find texture resource to load" );
    ex.addDetail( "Name", name );
    ex.addDetail( "ID", id );
    throw ex;
  }


  DataManager::DataBuilder::unloadTexture( unsigned int id )
  {
    SDL_DestroyTexture( _manager._rawTextures[id] );
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Configure

  void DataManager::configure( Json::Value& json_data )
  {
    Utilities::validateJson( json_data, "global_objects", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJson( json_data, "texture_data_file", Utilities::JSON_TYPE_STRING );

    _textureFile = json_data["texture_data_file"].asString();

    Json::Value& game_objects = json_data["global_objects"];

    Json::ArrayIndex game_objects_size = game_objects.size();
    for ( Json::ArrayIndex i = 0; i != game_objects_size; ++i )
    {
      Utilities::validateJson( game_objects[i], "name", Utilities::JSON_TYPE_STRING );
      std::string name = game_objects[i]["name"].asString();

      INFO_STREAM << "Building global object : " << name;

      GameObject* obj = Manager::getInstance()->getObjectFactory().build( game_objects[i] );

      if ( obj->isPhysical() ) // If the object claims to be physical
      {
        PhysicalObject* new_obj = dynamic_cast<PhysicalObject*>( obj );
        if ( new_obj == nullptr ) // Check that it's really physical!
        {
          Exception ex( "DataManager::configure()", "Could not cast game object to a physical object");
          ex.addDetail( "Name", name );
          throw ex;
        }
      }
      _gameObjects.addObject( obj, name );
    }

    // Load the global data textures into memory
    DataBuilder builder( *this, _textureFile );
    _globalData.load( builder );

  }

}

