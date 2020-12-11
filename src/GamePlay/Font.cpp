
#include "Regolith/GamePlay/Font.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Font related functions

  TTF_Font* loadFont( std::string filename, int fontsize )
  {
    TTF_Font* theFont = TTF_OpenFont( filename.c_str(), fontsize );
    if ( theFont == nullptr ) // Failed to open
    {
      FAILURE_STREAM << "Manager::_loadFonts() : Failed to load font : " << filename;
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


  void Font::configure( Json::Value& json_data, DataHandler& handler )
  {
    Utilities::validateJson( json_data, "font", Utilities::JSON_TYPE_STRING );

    _rawFont = handler.getRawFont( json_data["font"].asString() );
  }


  SDL_Surface* Font::write( Text& text )
  {
    SDL_Surface* textSurface = TTF_RenderText_Solid( _rawFont->ttf_font, text.getRawText()->c_str(), _rawFont->colour );
    if ( textSurface == nullptr )
    {
      Exception ex( "Font::write()", "Could not render text", true );
      ex.addDetail( "Text string", text.getRawText() );
      ex.addDetail( "SDL_ttf error", TTF_GetError() );
      throw ex;
    }

    return textSurface;
  }


  SDL_Surface* Font::writeParagraph( Text& /*text*/, float /*width*/, float /*height*/ )
  {
    Exception ex( "Font::writeParagraph()", "Functionality not yet available" );
    throw ex;
  }

}

