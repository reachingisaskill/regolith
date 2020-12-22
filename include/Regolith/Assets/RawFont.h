
#ifndef REGOLITH_ASSETS_RAW_FONT_H_
#define REGOLITH_ASSETS_RAW_FONT_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Assets/RawObjectDetails.h"


namespace Regolith
{

  // Proxy object for the SDL TTF_Font pointer
  struct RawFont
  {
    std::string filename;
  };


}

#endif // REGOLITH_ASSETS_RAW_FONT_H_

