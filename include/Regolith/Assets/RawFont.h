
#ifndef REGOLITH_ASSETS_RAW_FONT_H_
#define REGOLITH_ASSETS_RAW_FONT_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Assets/RawObjectDetails.h"


namespace Regolith
{

  // Proxy object for the SDL TTF_Font pointer
  struct RawFont
  {
    TTF_Font* ttf_font;
    SDL_Color colour;
  };


  // Function that creates the raw font proxy
  RawFont loadRawFont( FontDetail );

}

#endif // REGOLITH_ASSETS_RAW_FONT_H_

