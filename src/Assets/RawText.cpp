
#include "Regolith/Assets/RawText.h"

namespace Regolith
{

  RawText loadRawText( TextDetail details )
  {
    RawText raw_text;
    raw_text.text = new std::string();

    std::ifstream reader( filename );
    raw_text.text->assign( std::istreambuf_iterator<char>( reader ), std::istreambuf_iterator<char>() );

    return raw_text;
  }

}

