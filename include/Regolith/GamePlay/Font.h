
#ifndef REGOLITH_GAMEPLAY_FONT_H_
#define REGOLITH_GAMEPLAY_FONT_H_

#include "Regolith/Global/Global.h"


namespace Regolith
{

  // Function that creates the TTF_Font object on the heap
  TTF_Font* loadFont( std::string, int );


////////////////////////////////////////////////////////////////////////////////O
  // Raw font struct

  // Proxy object for the SDL TTF_Font pointer
  struct RawFont
  {
    TTF_Font* ttf_font;
    SDL_Color colour;

    RawFont( TTF_Font* f = nullptr ) : ttf_font( f ) {}
  };


////////////////////////////////////////////////////////////////////////////////O
  // Font class definition

  // Class that contains a pointer to the font proxy. Can be used to write text into surfaces at runtime
  class Font
  {
    private:
      RawFont* _rawFont;

    public:
      // Empty con/destructor
      Font();
      ~Font();


      // Configure the font object
      void configure( std::string, DataHandler& );


      // Using the loaded font, turn a string of text into a simple surface
      SDL_Surface* write( std::string );

      // More advanced, turn a long string of text in to a surface, contained and aligned within the provided limits
      SDL_Surface* writeParagraph( std::string, float, float, AlignmentMode, AlignmentMode );
  };

}

#endif // REGOLITH_GAMEPLAY_FONT_H_

