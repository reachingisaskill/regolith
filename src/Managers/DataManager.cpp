
#include "Regolith/Managers/DataManager.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Utilities/JsonValidation.h"

#include "logtastic.h"


namespace Regolith
{

  DataManager::DataManager() :
    _textureFile(),
    _rawTextures(),
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
    for ( RawTextureMap::iterator it = _rawTextures.begin(); it != _rawTextures.end(); ++it )
    {
      if ( it->second.texture != nullptr )
        SDL_DestroyTexture( it->second.texture );
    }
    _rawTextures.clear();

    INFO_LOG( "Data manager deleted." );
  }


  void DataManager::configureHandler( DataHandler* handler, std::string name )
  {
    IDNumber id;
    if ( ! _rawTextureLookup.exists( name ) )
    {
      id = _rawTextureLookup.addName( name );
      _loadedCaches[ id ] = false;
    }
    else
    {
      id = _rawTextureLookup.getID( name );
    }

    handler->_requiredTextures = &_rawTextureLookup[ id ];
    handler->_handlerID = id;
  }


  void DataManager::load( IDNumber i )
  {
    if ( _loadedCaches[i] ) return;
    DEBUG_STREAM << "Loading Data Cache: " << i;

    _loadedCaches[i] = true;

    Json::Value temp_data;
    Utilities::loadJsonData( temp_data, _textureFile );
    Json::Value& texture_data = temp_data["textures"];

    RawTextureCache& textureCache = _rawTextureLookup[i];
    RawTextureCache::iterator end = textureCache.end();
    for ( RawTextureCache::iterator it = textureCache.begin(); it != end; ++it )
    {
      std::string name = (*it)->first;

      Json::ArrayIndex i;
      for ( i = 0; i < texture_data.size(); ++i )
      {
        Json::Value& datum = texture_data[i];
        Utilities::validateJson( datum, "name", Utilities::JSON_TYPE_STRING );

        if ( datum["name"].asString() == name )
        {
          (*it)->second = makeTexture( datum );
          DEBUG_STREAM << "TEXTURE: " << name << ", " << (*it);
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


  void DataManager::unload( IDNumber i )
  {
    if ( ! _loadedCaches[i] ) return;
    DEBUG_STREAM << "Unloading Data Cache: " << i;

    _loadedCaches[i] = false;

    RawTextureCache& textureCache = _rawTextureLookup[i];
    RawTextureCache::iterator end = textureCache.end();
    for ( RawTextureCache::iterator it = textureCache.begin(); it != end; ++it )
    {
      if ( (*it)->second.texture != nullptr )
      {
        SDL_DestroyTexture( (*it)->second.texture );
        (*it)->second.texture = nullptr;
      }
    }
  }


  void DataManager::loadAll()
  {
    for ( size_t i = 0; i < _rawTextureLookup.size(); ++i )
    {
      load( i );
    }
  }


  RawTexturePointer DataManager::requestRawTexture( std::string name )
  {
    RawTextureMap::iterator it = _rawTextures.find( name );
    if ( it == _rawTextures.end() )
    {
      it = _rawTextures.insert( std::make_pair( name, RawTexture() ) ).first;
    }

    return  &(*it);
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Configure

  void DataManager::configure( Json::Value& json_data )
  {
    INFO_LOG( "Configuring the Data Manager." );
    // Configure the global handler
    _globalData.configure( "global" );
//    print();

    // Load the json data
    Utilities::validateJson( json_data, "global_objects", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJson( json_data, "texture_data_file", Utilities::JSON_TYPE_STRING );

    // Find the texture index file
    _textureFile = json_data["texture_data_file"].asString();

    // Load and validate the index file
    Json::Value temp_data;
    Utilities::loadJsonData( temp_data, _textureFile );
    Utilities::validateJson( temp_data, "textures", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJsonArray( temp_data["textures"], 0, Utilities::JSON_TYPE_ARRAY );
    Json::Value texture_data = temp_data["textures"];

    Json::ArrayIndex i;
    for ( i = 0; i < texture_data.size(); ++i )
    {
      Json::Value& datum = texture_data[i];
      Utilities::validateJson( datum, "name", Utilities::JSON_TYPE_STRING );
      std::string name = datum["name"].asString();

      // Throw an error if there are duplicates.
      if ( _rawTextures.find( name ) != _rawTextures.end() )
      {
        Exception ex( "DataManager::configure()", "Duplicate names found in texture list" );
        ex.addDetail( "Name", name );
        throw ex;
      }

      _rawTextures[name] = makeTexture( datum ); // Fill the info
      SDL_DestroyTexture( _rawTextures[name].texture ); // Delete the texture data
    }

    temp_data.clear();


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

//    print();
//    std::cout << "\n\n\n  LOADING GLOBAL \n\n\n";
//    DEBUG_STREAM << "  LOADING GLOBAL";
    // Load the global data textures into memory
    load( _rawTextureLookup.getID( "global" ) );
  }


  void DataManager::validate() const
  {
    NamedVector<GameObject, true>::const_iterator obj_end = _gameObjects.end();
    for ( NamedVector<GameObject, true>::const_iterator it = _gameObjects.begin(); it != obj_end; ++it )
    {
      if ( (*it) != nullptr )
        (*it)->validate();
    }
  }


//  void DataManager::print()
//  {
//    std::cout << "Loaded Status: \n";
//  
//    for ( std::map< IDNumber, bool >::iterator it = _loadedCaches.begin(); it != _loadedCaches.end(); ++it )
//    {
//      std::cout << "ID : " << it->first << " - Loaded : " << it->second << '\n';
//    }
//
//    std::cout << "\nTextures Lookup:\n";
//
//    for ( size_t i = 0; i < _rawTextureLookup.size(); ++i )
//    {
//      IDNumberSet& textures = _rawTextureLookup[i];
//      std::cout << "  Lookup: " << i << " - " << _rawTextureLookup.getName( i ) << '\n';
//
//      for ( IDNumberSet::iterator it = textures.begin(); it != textures.end(); ++it )
//      {
//        std::cout << "    Texture : " << (*it) << " @ " << &_rawTextures[(*it)] << " -> " << _rawTextures[(*it)].texture << '\n';
//      }
//    }
//
//    std::cout << "\nRaw Textures:\n";
//
//    for ( size_t i = 0; i < _rawTextures.size(); ++i )
//    {
//        std::cout << "  Texture : " << i << " @ " << &_rawTextures[i] << " -> " << _rawTextures[i].texture << '\n';
//    }
//  }

}

