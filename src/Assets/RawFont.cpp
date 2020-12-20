
#include "Regolith/Assets/RawFont.h"


namespace Regolith
{

  RawFont loadRawFont( FontDetail details )
  {
    RawFont raw_font;
    raw_font.colour = details.colour;

    raw_font.ttf_font = TTF_OpenFont( details.filename.c_str(), details.size );

    if ( raw_font.ttf_font == nullptr ) // Failed to open
    {
      Exception ex( "loadRawFont()", "Failed to load font" );
      ex.addDetail( "Font Name", details.filename );
      ex.addDetail( "Font Size", details.size );
      ex.addDetail( "TTF Error", TTF_GetError() );
      throw ex;
    }

    return raw_font;
  }

}

