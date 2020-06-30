#ifndef REGOLITH_MANAGERS_RAW_OBJECT_DETAILS_H_
#define REGOLITH_MANAGERS_RAW_OBJECT_DETAILS_H_

#include "Regolith/Global/Global.h"

#include <string>
#include <map>


namespace Regolith
{

  // Enumeration of the possible assets that can be loaded
  enum AssetType { TEXTURE, STRING, MUSIC, SOUND };

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Structures for each asset type 

  struct TextureDetail
  {
    std::string filename;
    unsigned int width;
    unsigned int height;
    unsigned int rows;
    unsigned int columns;
    SDL_Color colourkey;
  };


  struct StringDetail
  {
    std::string text;
    std::string font;
    unsigned int width;
    unsigned int height;
    SDL_Color colour;
  };


  struct MusicDetail
  {
    std::string filename;
  };


  struct SoundDetail
  {
    std::string filename;
  };


//  struct FontDetail
//  {
//    std::string filename;
//    unsigned int size;
//  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Asset union class

  struct Asset
  {
    const AssetType type;

    union
    {
      TextureDetail textureDetail;
      StringDetail stringDetail;
      MusicDetail musicDetail;
      SoundDetail soundDetail;
//      FontDetail fontDetail;
    };

    Asset( TextureDetail d ) : type ( ASSET_TEXTURE ), textureDetail( d ) {}
    Asset( StringDetail d ) : type ( ASSET_STRING ), stringDetail( d ) {}
    Asset( MusicDetail d ) : type ( ASSET_MUSIC ), musicDetail( d ) {}
    Asset( SoundDetail d ) : type ( ASSET_SOUND ), soundDetail( d ) {}
//    Asset( FontDetail d ) : type ( ASSET_FONT ), fontDetail( d ) {}
  };

  typedef std::map< std::string, Asset > AssetMap;

}

#endif // REGOLITH_MANAGERS_RAW_OBJECT_DETAILS_H_

