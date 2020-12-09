
#include "Regolith/GamePlay/Font.h"


namespace Regolith
{

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Font related functions

  TTF_Font* loadFont( std::string filename, int fontsize )
  {
    TTF_Font* theFont = TTF_OpenFont( filename.c_str(), fontsize );
    if ( theFont == nullptr ) // Failed to open
    {
      FAILURE_STREAM << "Manager::_loadFonts() : Failed to load font : " << font_name;
      Exception ex( "Manager::_loadFonts()", "Can not load font" );
      ex.addDetail( "Font Name", filename );
      ex.addDetail( "Font Path", fontsize );
      ex.addDetail( "TTF Error", TTF_GetError() );
      throw ex;
    }

    return theFont;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Font class function definitions

  Font::Font() :
    _rawFont( nullptr )
  {
  }


  Font::~Font()
  {
  }


  void Font::configure( std::string font_name, DataHandler& handler )
  {
    _rawFont = handler.getRawFont( font_name );
  }


  SDL_Surface* Font::write( std::string text )
  {
  }


  SDL_Surface* Font::writeParagraph( std::string /*text*/, float /*width*/, float /*height*/, AlignmentMode /*hor*/, AlignmentMode /*ver*/ )
  {
    Exception ex( "Font::writeParagraph()", "Functionality not yet available" );
    throw ex;
  }

}

