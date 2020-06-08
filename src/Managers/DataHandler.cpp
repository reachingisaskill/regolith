
#include "Regolith/Managers/DataHandler.h"


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
  }


  RawTexture* DataHandler::getRawTexture( std::string name )
  {
    RawTextureMap::iterator found = _rawTextures.find( name );
    if ( found == _rawTextures.end() )
    {
      found = _rawTextures.insert( std::make_pair( name, RawTexture() ) ).first;
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

