
#include "Regolith/Assets/RawFont.h"


namespace Regolith
{

  TTF_Font* loadFont( std::string filename, int fontsize )
  {
    TTF_Font* theFont = TTF_OpenFont( filename.c_str(), fontsize );

    if ( theFont == nullptr ) // Failed to open
    {
      ERROR_STREAM << "loadFont()" << filename;
      Exception ex( "Manager::_loadFonts()", "Failed to load font" );
      ex.addDetail( "Font Name", filename );
      ex.addDetail( "Font Size", fontsize );
      ex.addDetail( "TTF Error", TTF_GetError() );
      throw ex;
    }

    return theFont;
  }

}

