
#ifndef REGOLITH_GAME_PLAY_PEN_H_
#define REGOLITH_GAME_PLAY_PEN_H_

#include "Regolith/Global/Global.h"


namespace Regolith
{

  /*
   * Serves as a builder class for SDL_Surfaces based on text
   */
  class Pen
  {
    private:
      TTF_Font* _ttfFont;
      SDL_Color _colour;


    public:
      Pen();

      Pen( TTF_Font*, SDL_Color );

      // No cleanup required. Pens do no own any data
      ~Pen();

      SDL_Surface* shortWrite( std::string& ) const;

      SDL_Surface* paragraphWrite( std::string& ) const;
  };

}

#endif // REGOLITH_GAME_PLAY_PEN_H_

