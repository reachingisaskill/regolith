
#ifndef REGOLITH_ASSETS_RAW_FONT_H_
#define REGOLITH_ASSETS_RAW_FONT_H_

#include "Regolith/Global/Global.h"


namespace Regolith
{

  // Function that creates the TTF_Font object on the heap
  TTF_Font* loadFont( std::string, int );


  // Proxy object for the SDL TTF_Font pointer
  struct RawFont
  {
    TTF_Font* ttf_font;
    SDL_Color colour;

    RawFont( TTF_Font* f = nullptr ) : ttf_font( f ) {}
  };

}

#endif // REGOLITH_ASSETS_RAW_FONT_H_

