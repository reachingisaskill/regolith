
#ifndef REGOLITH_MANAGERS_FONT_MANAGER_H_
#define REGOLITH_MANAGERS_FONT_MANAGER_H_

#include "Regolith/Assets/RawFont.h"
#include "Regolith/Architecture/Component.h"
#include "Regolith/GamePlay/Pen.h"

#include <map>


namespace Regolith
{

  /*
   * Fonts are slightly awkward to control due to the design of the TTF interface
   * So use a context group wide handler to obfuscate the details.
   *
   * Users must request a Pen object a combination builder and proxy object for the 
   * TTF font data stored within the handler.
   * 
   */
  class FontManager : public Component
  {
    // Allow links to access the private members
    template < class T, class R > friend class Link;

    // Useful typedefs
    typedef std::pair< TTF_Font*, SDL_Color > FontPair;
    typedef std::map< unsigned int, FontPair > FontSizeMap;
    typedef std::map< std::string, FontSizeMap > FontMap;

    typedef std::map< std::string, RawFont > FontFileMap;

    private:
      FontMap _fonts;
      FontFileMap _fontFiles;

    public:
      FontManager();

      virtual ~FontManager();

      // Clear the owned memory
      void clear();


      // Configure the list of fonts to be used
      void configure( Json::Value& );


      // Return an Pen to write text
      Pen requestPen( std::string, unsigned int, SDL_Color );


////////////////////////////////////////////////////////////////////////////////
      // Component Interface
      // Register game-wide events with the manager
      virtual void registerEvents( InputManager& ) override {}

      // Regolith events
      virtual void eventAction( const RegolithEvent&, const SDL_Event& ) override {}

  };

}

#endif // REGOLITH_MANAGERS_FONT_MANAGER_H_

