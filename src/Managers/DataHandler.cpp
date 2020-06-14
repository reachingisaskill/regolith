
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Managers/Manager.h"


namespace Regolith
{
  DataHandler::DataHandler() :
    _rawTextures(),
    _rawSounds(),
    _rawMusic(),
    _isLoaded( false )
  {
  }


  DataHandler::~DataHandler()
  {
    INFO_LOG( "Destroying Data Handler" );

    if ( _isLoaded )
    {
      RawTextureMap::iterator textures_end = _rawTextures.end();
      for ( RawTextureMap::iterator it = _rawTextures.begin(); it != textures_end; ++it )
      {
        std::string name = it->first;

        if ( it->second.texture != nullptr )
        {
          SDL_DestroyTexture( it->second.texture );
          it->second.texture = nullptr;
        }

        DEBUG_STREAM << "Unloaded texture: " << name;
      }

      RawMusicMap::iterator music_end = _rawMusic.end();
      for ( RawMusicMap::iterator it = _rawMusic.begin(); it != music_end; ++it )
      {
        std::string name = it->first;

        if ( it->second.music != nullptr )
        {
          Mix_FreeMusic( it->second.music );
          it->second.music = nullptr;
        }

        DEBUG_STREAM << "Unloaded music: " << name;
      }

      RawSoundMap::iterator sounds_end = _rawSounds.end();
      for ( RawSoundMap::iterator it = _rawSounds.begin(); it != sounds_end; ++it )
      {
        std::string name = it->first;

        if ( it->second.sound != nullptr )
        {
          Mix_FreeChunk( it->second.sound );
          it->second.sound = nullptr;
        }

        DEBUG_STREAM << "Unloaded sound: " << name;
      }

      _isLoaded = false;
    }
    _rawTextures.clear();
    _rawMusic.clear();
    _rawSounds.clear();
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
      found = _rawSounds.insert( std::make_pair( name, RawSound() ) ).first;
    }

    return &(found->second);
  }


  RawMusic* DataHandler::getRawMusic( std::string name )
  {
    RawMusicMap::iterator found = _rawMusic.find( name );
    if ( found == _rawMusic.end() )
    {
      found = _rawMusic.insert( std::make_pair( name, RawMusic() ) ).first;
    }

    return &(found->second);
  }

}

