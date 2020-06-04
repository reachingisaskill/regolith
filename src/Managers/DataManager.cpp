
#include "Regolith/Managers/DataManager.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Utilities/JsonValidation.h"

#include "logtastic.h"

#include <mutex>
#include <atomic>


namespace Regolith
{

  void theLoadingThread();
  void loadFunction();
  void unloadFunction();


  DataManager::DataManager() :
    _loadingThread( theLoadingThread ),
    _textureFile(),
    _rawTextures(),
    _rawTextureCaches( "raw_texture_caches" ),
    _gameObjects( "game_objects" ),
    _globalData(),
    _loadQueue(),
    _unloadQueue(),
    _handlerQueue()
  {
  }


  DataManager::~DataManager()
  {
  }


  void DataManager::clear()
  {
    INFO_LOG( "Clearing Data Manager." );

    INFO_LOG( "Closing the loading thread." );
    _loadingThread.join();

    INFO_LOG( "Clearing texture caches." );
    _rawTextureCaches.clear();

    INFO_LOG( "Clearing raw texture data" );
    for ( RawTextureMap::iterator it = _rawTextures.begin(); it != _rawTextures.end(); ++it )
    {
      if ( it->second.texture != nullptr )
      {
        SDL_DestroyTexture( it->second.texture );
        it->second.texture = nullptr;
      }
    }
    _rawTextures.clear();

    INFO_LOG( "Data manager deleted." );
  }


  void DataManager::configureHandler( DataHandler& handler, std::string name )
  {
    IDNumber id;
    if ( ! _rawTextureCaches.exists( name ) )
    {
      id = _rawTextureCaches.addName( name );
      _loadedCaches[ id ] = false;
    }
    else
    {
      id = _rawTextureCaches.getID( name );
    }

    handler._requiredTextures = &_rawTextureCaches[ id ];
    handler._handlerID = id;
  }


  void DataManager::loadHandler( IDNumber i )
  {
    DEBUG_STREAM << "LOAD HANDLER: " << i;
    _handlerQueue.push( i );

    {
      std::lock_guard<std::mutex>( Manager::getInstance()->getThreadManager().DataUpdate.mutex );
      Manager::getInstance()->getThreadManager().DataUpdate.data = true;
    }
    Manager::getInstance()->getThreadManager().DataUpdate.variable.notify_all();
  }


  void DataManager::load( IDNumber i )
  {
    DEBUG_STREAM << "LOAD: " << i;
    _loadQueue.push( i );

    {
      std::lock_guard<std::mutex>( Manager::getInstance()->getThreadManager().DataUpdate.mutex );
      Manager::getInstance()->getThreadManager().DataUpdate.data = true;
    }
    Manager::getInstance()->getThreadManager().DataUpdate.variable.notify_all();
  }


  void DataManager::unload( IDNumber i )
  {
    DEBUG_STREAM << "UNLOAD: " << i;
    _unloadQueue.push( i );

    {
      std::lock_guard<std::mutex>( Manager::getInstance()->getThreadManager().DataUpdate.mutex );
      Manager::getInstance()->getThreadManager().DataUpdate.data = true;
    }
    Manager::getInstance()->getThreadManager().DataUpdate.variable.notify_all();
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
    configureHandler( _globalData, "global" );

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
      _rawTextures[name].texture = nullptr; // Null
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

    DEBUG_LOG( "Loading the global objects" );

    // Push the global cache ID onto the queue
    _loadQueue.push( _rawTextureCaches.getID( "global" ) );

    // Run the load function in this thread - load thread only active once engine starts
    loadFunction();
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


  void DataManager::loadEntryPoint( IDNumber id )
  {
    const ContextHandler* handler = Manager::getInstance()->getContextManager().getContextHandler( id );
    const ContextHandler::DataCacheList& caches = handler->getDataCache();

    ContextHandler::DataCacheList::const_iterator end = caches.end();
    for ( ContextHandler::DataCacheList::const_iterator it = caches.begin(); it != end; ++it )
    {
      // Push the data cache
      _loadQueue.push( (*it) );
    }

    // Run the load function in this thread - load thread only active once engine starts
    loadFunction();
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Loading/unloading thread

  void theLoadingThread()
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
    IDNumber temp_number;

    std::unique_lock<std::mutex> dataLock( dataUpdate.mutex );
    INFO_LOG( "Loading thread waiting for first command" );
    while( ! quitFlag )
    {
      dataUpdate.variable.wait( dataLock, [&]()->bool{ return quitFlag || dataUpdate.data; } );

      DEBUG_STREAM << "LOADING THREAD WORKING";

      do
      {
        if ( manager._handlerQueue.pop( temp_number ) )
        {
          const ContextHandler* handler = Manager::getInstance()->getContextManager().getContextHandler( temp_number );

          const ContextHandler::DataCacheList& caches = handler->getDataCache();

          std::map<IDNumber, bool>::iterator end = manager._loadedCaches.end();
          for ( std::map<IDNumber, bool>::iterator it = manager._loadedCaches.begin(); it != end; ++it )
          {
            ContextHandler::DataCacheList::const_iterator found = caches.find( it->first );
            if ( found == caches.end() ) 
            {
              if ( it->second )
              {
                manager._unloadQueue.push( it->first );
              }
            }
            else
            {
              if ( ! it->second )
              {
                manager._loadQueue.push( it->first );
              }
            }
          }
        }

        unloadFunction();

        loadFunction();

      }
      while ( ! manager._handlerQueue.empty() );

      dataUpdate.data = false;
      Manager::getInstance()->raiseEvent( REGOLITH_EVENT_DATA_LOADED );
    }

    dataLock.unlock();
    INFO_LOG( "Data Manager loading thread stopped." );
  }


  void loadFunction()
  {
    DataManager& manager = Manager::getInstance()->getDataManager();
    IDNumber temp_number;

    if ( ! manager._loadQueue.empty() )
    {
      Json::Value temp_data;
      Utilities::loadJsonData( temp_data, manager._textureFile );
      Json::Value& texture_data = temp_data["textures"];

      while ( manager._loadQueue.pop( temp_number ) )
      {
        if ( manager._loadedCaches[temp_number] ) continue;
        manager._loadedCaches[temp_number] = true;
        DEBUG_STREAM << "Loading Data Cache: " << temp_number;


        RawTextureCache& textureCache = manager._rawTextureCaches[temp_number];
        RawTextureCache::iterator end = textureCache.end();
        for ( RawTextureCache::iterator it = textureCache.begin(); it != end; ++it )
        {
          std::string name = (*it)->first;

          Json::ArrayIndex i;
          for ( i = 0; i < texture_data.size(); ++i )
          {
            Json::Value& datum = texture_data[i];
            if ( datum["name"].asString() == name )
            {
              (*it)->second = makeTexture( datum );
              DEBUG_STREAM << "TEXTURE: " << name << ", " << (*it);
              break;
            }
          }

          if ( temp_number == texture_data.size() ) // Texture wasn't found
          {
            ERROR_STREAM << "Could not find texture resource to load : " << name << " - " << (*it);
//              Exception ex( "DataManager::load()", "Could not find texture resource to load" );
//              ex.addDetail( "Name", name );
//              ex.addDetail( "ID", (*it) );
//              throw ex;
          }
        }
      }
    }
  }


  void unloadFunction()
  {
    DataManager& manager = Manager::getInstance()->getDataManager();
    IDNumber temp_number;

    while ( manager._unloadQueue.pop( temp_number ) )
    {
      if ( temp_number == manager._globalData.getID() ) continue;
      if ( ! manager._loadedCaches[temp_number] ) continue;

      manager._loadedCaches[temp_number] = false;
      DEBUG_STREAM << "Unloading Data Cache: " << temp_number;

      RawTextureCache& textureCache = manager._rawTextureCaches[temp_number];
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
  }

}

