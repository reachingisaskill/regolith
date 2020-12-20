
#ifndef REGOLITH_ASSETS_RAW_TEXT_H_
#define REGOLITH_ASSETS_RAW_TEXT_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Assets/RawObjectDetails.h"


namespace Regolith
{

  // Owns the pointer to the text string that is loaded
  struct RawText
  {
    std::string* text;
  };


  // Function that loads the text from the text file into the RawText proxy
  RawText loadRawText( TextDetail );

}

#endif // REGOLITH_ASSETS_RAW_TEXT_H_

