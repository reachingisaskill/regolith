
#include "Regolith/Managers/DataManager.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Utilities/JsonValidation.h"

#include "logtastic.h"


namespace Regolith
{

  DataManager::DataManager() :
    _textureFile(),
    _rawTextures( "raw_textures" ),
    _rawTextureLookup( "raw_texture_lookup" ),
    _gameObjects( "game_objects" ),
    _globalData()
  {
  }


  DataManager::~DataManager()
  {
    INFO_LOG( "Deleting Data Manager." );

    INFO_LOG( "Clearing texture caches." );
    _rawTextureLookup.clear();

    INFO_LOG( "Clearing raw texture data" );
    for ( NamedReferenceVector< RawTexture >::iterator it = _rawTextures.begin(); it != _rawTextures.end(); ++it )
    {
      if ( it->texture != nullptr )
        SDL_DestroyTexture( it->texture );
    }
    _rawTextures.clear();

    INFO_LOG( "Data manager deleted." );
  }


  void DataManager::configureHandler( DataHandler* handler, std::string name )
  {
    if ( ! _rawTextureLookup.exists( name ) )
    {
      _rawTextureLookup.addObject( std::vector< unsigned int >(), name );
    }
    handler->_requiredTextures = &_rawTextureLookup.get( name );
    handler->_handlerID = _rawTextureLookup.getID( name );
  }


  void DataManager::load( unsigned int i )
  {
    Json::Value temp_data;
    Utilities::loadJsonData( temp_data, _textureFile );
    Utilities::validateJson( temp_data, "textures", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJsonArray( temp_data["textures"], 0, Utilities::JSON_TYPE_ARRAY );
    Json::Value& texture_data = temp_data["textures"];

    std::vector< unsigned int >& textureVector = _rawTextureLookup[i];
    std::vector< unsigned int >::iterator end = textureVector.end();
    for ( std::vector< unsigned int >::iterator it = textureVector.begin(); it != end; ++it )
    {
      std::string name = _rawTextures.getName( (*it) );

      Json::ArrayIndex i;
      for ( i = 0; i < texture_data.size(); ++i )
      {
        Json::Value& datum = texture_data[i];
        Utilities::validateJson( datum, "name", Utilities::JSON_TYPE_STRING );
        if ( datum["name"] == name )
        {
          _rawTextures[(*it)] = makeTexture( datum );
          break;
        }
      }

      if ( i == texture_data.size() ) // Texture wasn't found
      {
        Exception ex( "DataManager::load()", "Could not find texture resource to load" );
        ex.addDetail( "Name", name );
        ex.addDetail( "ID", (*it) );
        throw ex;
      }
    }
  }


  void DataManager::unload( unsigned int i )
  {
    std::vector< unsigned int >& textureVector = _rawTextureLookup[i];
    std::vector< unsigned int >::iterator end = textureVector.end();
    for ( std::vector< unsigned int >::iterator it = textureVector.begin(); it != end; ++it )
    {
      SDL_DestroyTexture( _rawTextures[ (*it) ].texture );
    }
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Configure

  void DataManager::configure( Json::Value& json_data )
  {
    // Configure the global handler
    _globalData.configure( "global" );

    // Load the json data
    Utilities::validateJson( json_data, "global_objects", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJson( json_data, "texture_data_file", Utilities::JSON_TYPE_STRING );

    // Find the texture index file
    _textureFile = json_data["texture_data_file"].asString();

    // Find the global object list
    Json::Value& game_objects = json_data["global_objects"];

    // Iterate the global objects list
    Json::ArrayIndex game_objects_size = game_objects.size();
    for ( Json::ArrayIndex i = 0; i != game_objects_size; ++i )
    {
      Utilities::validateJson( game_objects[i], "name", Utilities::JSON_TYPE_STRING );
      std::string name = game_objects[i]["name"].asString();

      INFO_STREAM << "Building global object : " << name;

      GameObject* obj = Manager::getInstance()->getObjectFactory().build( game_objects[i], _globalData );

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
    load( _rawTextureLookup.getID( "global" ) );

  }

}

