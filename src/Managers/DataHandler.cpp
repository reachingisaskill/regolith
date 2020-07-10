//#define LOGTASTIC_DEBUG_OFF

#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Managers/DataManager.h"
#include "Regolith/Managers/Manager.h"


namespace Regolith
{
  DataHandler::DataHandler() :
    _rawTextures(),
    _rawSounds(),
    _rawMusic(),
    _surfaceRenderQueue(),
    _isLoaded( false ),
    _isRendered( true ),
    _loadingMutex()
  {
  }


  DataHandler::~DataHandler()
  {
    INFO_LOG( "Destroying Data Handler" );

    if ( _isLoaded )
    {
      this->unload();
    }

    _rawTextures.clear();
    _rawMusic.clear();
    _rawSounds.clear();
  }


  bool DataHandler::isLoaded() const
  {
    GuardLock lg( _loadingMutex );
    return _isLoaded;
  }


  void DataHandler::load()
  {
    INFO_LOG( "Loading Data Handler" );

    GuardLock lg( _loadingMutex );
    if ( _isLoaded ) return;

    _isRendered = false;

    DataManager& manager = Manager::getInstance()->getDataManager();

    RawTextureMap::iterator texture_end = _rawTextures.end();
    for ( RawTextureMap::iterator it = _rawTextures.begin(); it != texture_end; ++it )
    {
      std::string name = it->first;
      try
      {
        manager.loadRawTexture( name, it->second );
        _surfaceRenderQueue.push_back( &it->second );
        DEBUG_STREAM << "Loaded Surface: " << name << " - " << it->second.width << ", " << it->second.height << ", " << it->second.cells << " @ " << it->second.texture;
      }
      catch( Exception& ex )
      {
        ex.addDetail( "Texture Name", name );
        throw ex;
      }
    }

    // Tell the engine to render the surfaces into textures on the rendering thread
    Manager::getInstance()->renderSurfaces( this );


    RawMusicMap::iterator music_end = _rawMusic.end();
    for ( RawMusicMap::iterator it = _rawMusic.begin(); it != music_end; ++it )
    {
      std::string name = it->first;
      try
      {
        manager.loadRawMusic( name, it->second );
        DEBUG_STREAM << "Loaded Music: " << name << " @ " << it->second.music;
      }
      catch( Exception& ex )
      {
        ex.addDetail( "Music Name", name );
        throw ex;
      }
    }

    RawSoundMap::iterator sound_end = _rawSounds.end();
    for ( RawSoundMap::iterator it = _rawSounds.begin(); it != sound_end; ++it )
    {
      std::string name = it->first;
      try
      {
        manager.loadRawSound( name, it->second );
        DEBUG_STREAM << "Loaded Sound: " << name << " @ " << it->second.sound;
      }
      catch( Exception& ex )
      {
        ex.addDetail( "Sound Name", name );
        throw ex;
      }
    }


    // Block the current thread while we wait for the engine to render all the textures
    while ( ! _isRendered )
    {
      std::this_thread::sleep( std::chrono::milliseconds( 10 ) )
    }

    _isLoaded = true;
  }


  void DataHandler::unload()
  {
    INFO_LOG( "Unloading Data Handler" );

    GuardLock lg( _loadingMutex );
    if ( ! _isLoaded ) return;

    RawTextureMap::iterator textures_end = _rawTextures.end();
    for ( RawTextureMap::iterator it = _rawTextures.begin(); it != textures_end; ++it )
    {
      std::string name = it->first;

      if ( it->second.texture != nullptr )
      {
        DEBUG_STREAM << "Unloaded texture: " << name << " @ " << it->second.texture;
        SDL_DestroyTexture( it->second.texture );
        it->second.texture = nullptr;
      }
    }

    RawMusicMap::iterator music_end = _rawMusic.end();
    for ( RawMusicMap::iterator it = _rawMusic.begin(); it != music_end; ++it )
    {
      std::string name = it->first;

      if ( it->second.music != nullptr )
      {
        DEBUG_STREAM << "Unloaded music: " << name << " @ " << it->second.music;
        Mix_FreeMusic( it->second.music );
        it->second.music = nullptr;
      }

    }

    RawSoundMap::iterator sounds_end = _rawSounds.end();
    for ( RawSoundMap::iterator it = _rawSounds.begin(); it != sounds_end; ++it )
    {
      std::string name = it->first;

      if ( it->second.sound != nullptr )
      {
        DEBUG_STREAM << "Unloaded sound: " << name << " @ " << it->second.sound;
        Mix_FreeChunk( it->second.sound );
        it->second.sound = nullptr;
      }

    }

    _isLoaded = false;
  }


  RawTexture* DataHandler::getRawTexture( std::string name )
  {
    RawTextureMap::iterator found = _rawTextures.find( name );
    if ( found == _rawTextures.end() )
    {
      RawTexture new_texture = Manager::getInstance()->getDataManager().buildRawTexture( name );
      found = _rawTextures.insert( std::make_pair( name, new_texture ) ).first;
    }

    return &(found->second);
  }


  RawSound* DataHandler::getRawSound( std::string name )
  {
    RawSoundMap::iterator found = _rawSounds.find( name );
    if ( found == _rawSounds.end() )
    {
      RawSound new_sound = Manager::getInstance()->getDataManager().buildRawSound( name );
      found = _rawSounds.insert( std::make_pair( name, new_sound ) ).first;
    }

    return &(found->second);
  }


  RawMusic* DataHandler::getRawMusic( std::string name )
  {
    RawMusicMap::iterator found = _rawMusic.find( name );
    if ( found == _rawMusic.end() )
    {
      RawMusic new_music = Manager::getInstance()->getDataManager().buildRawMusic( name );
      found = _rawMusic.insert( std::make_pair( name, new_music ) ).first;
    }

    return &(found->second);
  }


  RawTexture* DataHandler::popRenderTexture() const
  {
    RawTexture* temp = nullptr;

    if ( ! _surfaceRenderQueue.empty() )
    {
      temp = _surfaceRenderQueue.front();
      _surfaceRenderQueue.pop();
    }
    else
    {
      _isRendered = true;
    }

    return temp;
  }

}

