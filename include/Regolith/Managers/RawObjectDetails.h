#ifndef REGOLITH_MANAGERS_RAW_OBJECT_DETAILS_H_
#define REGOLITH_MANAGERS_RAW_OBJECT_DETAILS_H_

#include "Regolith/Global/Global.h"

#include <string>
#include <map>


namespace Regolith
{

  // Enumeration of the possible assets that can be loaded
  enum AssetType { ASSET_TEXTURE, ASSET_STRING, ASSET_MUSIC, ASSET_SOUND };
  static const char* const AssetTypeNames[] = { "ASSET_TEXTURE", "ASSET_STRING", "ASSET_MUSIC", "ASSET_SOUND" };

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
    TTF_Font* font;
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

    Asset( TextureDetail );
    Asset( StringDetail );
    Asset( MusicDetail );
    Asset( SoundDetail );
//    Asset( FontDetail );

    Asset( const Asset& );

    ~Asset();
  };

  typedef std::map< std::string, Asset > AssetMap;

}

#endif // REGOLITH_MANAGERS_RAW_OBJECT_DETAILS_H_
