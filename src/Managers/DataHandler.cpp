//#define LOGTASTIC_DEBUG_OFF

#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Managers/DataManager.h"
#include "Regolith/Managers/Manager.h"

#include <thread>
#include <chrono>


namespace Regolith
{
  DataHandler::DataHandler() :
    _rawTextures(),
    _rawSounds(),
    _rawMusic(),
    _rawFonts(),
    _rawTexts()
  {
  }


  DataHandler::~DataHandler()
  {
    INFO_LOG( "Destroying Data Handler" );

    _rawTextures.clear();
    _rawSounds.clear();
    _rawMusic.clear();
    _rawFonts.clear();
    _rawTexts.clear();
  }


  void DataHandler::clear()
  {
    INFO_LOG( "DataHandler::clear : Clearing the Data Handler" );

    RawTextureMap::iterator textures_end = _rawTextures.end();
    for ( RawTextureMap::iterator it = _rawTextures.begin(); it != textures_end; ++it )
    {
      std::string name = it->first;

      if ( it->second.surface != nullptr )
      {
        DEBUG_STREAM << "DataHandler::clear : Unloaded texture: " << name << " @ " << it->second.surface;
        SDL_FreeSurface( it->second.surface );
        it->second.surface = nullptr;
      }
    }

    // Nothing to do - font memory is handled by the FontHandler.
    /*
    RawFontMap::iterator font_end = _rawFonts.end();
    for ( RawFontMap::iterator it = _rawFonts.begin(); it != font_end; ++it )
    {
      std::string name = it->first;

      if ( it->second.ttf_font != nullptr )
      {

        DEBUG_STREAM << "Unloaded font: " << name << " @ " << it->second.ttf_font;
        TTF_CloseFont( it->second.ttf_font );
        it->second.ttf_font = nullptr;
      }
    }
    */

    RawTextMap::iterator texts_end = _rawTexts.end();
    for ( RawTextMap::iterator it = _rawTexts.begin(); it != texts_end; ++it )
    {
      std::string name = it->first;

      if ( it->second.text != nullptr )
      {
        DEBUG_STREAM << "DataHandler::clear : Unloaded text: " << name;
        delete it->second.text;
        it->second.text = nullptr;
      }
    }

    RawMusicMap::iterator music_end = _rawMusic.end();
    for ( RawMusicMap::iterator it = _rawMusic.begin(); it != music_end; ++it )
    {
      std::string name = it->first;

      if ( it->second.music != nullptr )
      {
        DEBUG_STREAM << "DataHandler::clear : Unloaded music: " << name << " @ " << it->second.music;
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
        DEBUG_STREAM << "DataHandler::clear : Unloaded sound: " << name << " @ " << it->second.sound;
        Mix_FreeChunk( it->second.sound );
        it->second.sound = nullptr;
      }
    }
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


  RawFont* DataHandler::getRawFont( std::string name )
  {
    RawFontMap::iterator found = _rawFonts.find( name );
    if ( found == _rawFonts.end() )
    {
      RawFont new_font = Manager::getInstance()->getDataManager().buildRawFont( name );
      found = _rawFonts.insert( std::make_pair( name, new_font ) ).first;
    }

    return &(found->second);
  }


  RawText* DataHandler::getRawText( std::string name )
  {
    RawTextMap::iterator found = _rawTexts.find( name );
    if ( found == _rawTexts.end() )
    {
      RawText new_text = Manager::getInstance()->getDataManager().buildRawText( name );
      found = _rawTexts.insert( std::make_pair( name, new_text ) ).first;
    }

    return &(found->second);
  }

}

