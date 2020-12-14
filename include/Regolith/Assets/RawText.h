
#ifndef REGOLITH_ASSETS_RAW_TEXT_H_
#define REGOLITH_ASSETS_RAW_TEXT_H_

#include "Regolith/Global/Global.h"


namespace Regolith
{

  // Function that loads the text from the text file
  std::string* loadText( std::string );


  // Owns the pointer to the text string that is loaded
  struct RawText
  {
    std::string* text;

    RawText() : text( nullptr ) {}
  };

}

#endif // REGOLITH_ASSETS_RAW_TEXT_H_

