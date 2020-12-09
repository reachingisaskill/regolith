
#include "Regolith/Managers/RawObjectDetails.h"


namespace Regolith
{

  Asset::Asset( ImageDetail d ) :
    type( ASSET_IMAGE )
  {
    new (&imageDetail) ImageDetail( d );
  }


  Asset::Asset( TextDetail d ) :
    type( ASSET_TEXT )
  {
    new (&textDetail) TextDetail( d );
  }


  Asset::Asset( AudioDetail d ) :
    type( ASSET_AUDIO )
  {
    new (&audioDetail) AudioDetail( d );
  }


  Asset::Asset( FontDetail d ) :
    type( ASSET_FONT )
  {
    new (&fontDetail) FontDetail( d );
  }


  Asset::Asset( const Asset& ass ) :
    type( ass.type )
  {
    switch ( type )
    {
      case ASSET_IMAGE :
        new (&imageDetail) ImageDetail( ass.imageDetail );
        break;

      case ASSET_TEXT :
        new (&textDetail) StringDetail( ass.textDetail );
        break;

      case ASSET_AUDIO :
        new (&audioDetail) AudioDetail( ass.audioDetail );
        break;

      case ASSET_FONT :
        new (&fontDetail) FontDetail( ass.fontDetail );
        break;
    }
  }



  Asset::~Asset()
  {
    switch( type )
    {
      case ASSET_IMAGE :
        imageDetail.~ImageDetail();
        break;

      case ASSET_TEXT :
        textDetail.~TextDetail();
        break;

      case ASSET_AUDIO :
        audioDetail.~AudioDetail();
        break;

      case ASSET_FONT :
        fontDetail.~FontDetail();
        break;
    }
  }

}

