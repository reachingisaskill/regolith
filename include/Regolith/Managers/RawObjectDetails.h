#ifndef REGOLITH_MANAGERS_RAW_OBJECT_DETAILS_H_
#define REGOLITH_MANAGERS_RAW_OBJECT_DETAILS_H_

#include "Regolith/Global/Global.h"

#include <string>
#include <map>


namespace Regolith
{

  struct RawTextureDetail
  {
    std::string filename;
    unsigned int width;
    unsigned int height;
    unsigned int rows;
    unsigned int columns;
    SDL_Color colourkey;
  };

  typedef std::map<std::string, RawTextureDetail> RawTextureDetailMap;


  struct RawStringDetail
  {
    std::string text;
    std::string font;
    unsigned int width;
    unsigned int height;
    SDL_Color colour;
  };

  typedef std::map<std::string, RawStringDetail> RawStringDetailMap;


  struct RawMusicDetail
  {
    std::string filename;
  };

  typedef std::map<std::string, RawMusicDetail> RawMusicDetailMap;


  struct RawSoundDetail
  {
    std::string filename;
  };

  typedef std::map<std::string, RawSoundDetail> RawSoundDetailMap;


  struct RawFontDetail
  {
    std::string filename;
    unsigned int size;
  };

  typedef std::map<std::string, RawFontDetail> RawFontDetailMap;


  struct RawAsset
  {
    enum AssetType { TEXTURE, STRING, MUSIC, SOUND } type;

    union
    {
      RawTextureDetail textureDetail;
      RawStringDetail stringDetail;
      RawMusicDetail musicDetail;
      RawSoundDetail soundDetail;
    };
  };

}

#endif // REGOLITH_MANAGERS_RAW_OBJECT_DETAILS_H_

