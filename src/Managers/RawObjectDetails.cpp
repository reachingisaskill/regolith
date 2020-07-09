
#include "Regolith/Managers/RawObjectDetails.h"


namespace Regolith
{

  Asset::Asset( TextureDetail d ) :
    type( ASSET_TEXTURE )
  {
    new (&textureDetail) TextureDetail( d );
  }


  Asset::Asset( StringDetail d ) :
    type( ASSET_STRING )
  {
    new (&stringDetail) StringDetail( d );
  }


  Asset::Asset( MusicDetail d ) :
    type( ASSET_MUSIC )
  {
    new (&musicDetail) MusicDetail( d );
  }


  Asset::Asset( SoundDetail d ) :
    type( ASSET_SOUND )
  {
    new (&soundDetail) SoundDetail( d );
  }


  Asset::Asset( const Asset& ass ) :
    type( ass.type )
  {
    switch ( type )
    {
      case ASSET_TEXTURE :
        new (&textureDetail) TextureDetail( ass.textureDetail );
        break;

      case ASSET_STRING :
        new (&stringDetail) StringDetail( ass.stringDetail );
        break;

      case ASSET_MUSIC :
        new (&musicDetail) MusicDetail( ass.musicDetail );
        break;

      case ASSET_SOUND :
        new (&soundDetail) SoundDetail( ass.soundDetail );
        break;

//      case ASSET_FONT :
//        new (&fontDetail) FontDetail( ass.fontDetail );
//        break;
    }
  }



  Asset::~Asset()
  {
    switch( type )
    {
      case ASSET_TEXTURE :
        textureDetail.~TextureDetail();
        break;

      case ASSET_STRING :
        stringDetail.~StringDetail();
        break;

      case ASSET_MUSIC :
        musicDetail.~MusicDetail();
        break;

      case ASSET_SOUND :
        soundDetail.~SoundDetail();
        break;

//      case ASSET_FONT :
//        fontDetail.~FontDetail();
//        break;
    }
  }

}

