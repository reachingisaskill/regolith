#ifndef REGOLITH_ASSETS_RAW_OBJECT_DETAILS_H_
#define REGOLITH_ASSETS_RAW_OBJECT_DETAILS_H_

#include "Regolith/Global/Global.h"

#include <string>
#include <map>
#include <set>


namespace Regolith
{
  // Forward declare DataHandler
  class DataHandler;

  // Enumeration of the possible assets that can be loaded
  enum AssetType { ASSET_IMAGE, ASSET_TEXT, ASSET_AUDIO, ASSET_FONT };
  static const char* const AssetTypeNames[] = { "ASSET_IMAGE", "ASSET_TEXT", "ASSET_AUDIO", "ASSET_FONT" };

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Structures for each asset type 

  struct ImageDetail
  {
    std::string filename;
    unsigned int width;
    unsigned int height;
    unsigned int rows;
    unsigned int columns;
    SDL_Color colourkey;
  };


  struct TextDetail
  {
    std::string filename;
  };


  struct AudioDetail
  {
    std::string filename;
  };


  struct FontDetail
  {
    std::string filename;
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Asset union class

  struct Asset
  {
    // Const asset type
    const AssetType type;

    // Union of all detail types. Asset type determines the active one
    union
    {
      ImageDetail imageDetail;
      TextDetail textDetail;
      AudioDetail audioDetail;
      FontDetail fontDetail;
    };

    // Contructors and destructors to handle union
    Asset( ImageDetail );
    Asset( TextDetail );
    Asset( AudioDetail );
    Asset( FontDetail );

    Asset( const Asset& );

    ~Asset();
  };

  typedef std::map< std::string, Asset > AssetMap;

}

#endif // REGOLITH_ASSETS_RAW_OBJECT_DETAILS_H_

