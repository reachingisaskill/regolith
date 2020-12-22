
#include "Regolith/GamePlay/Pen.h"


namespace Regolith
{

  Pen::Pen() :
  _ttfFont( nullptr ),
  _colour( { 0, 0, 0, 0 } )
  {
  }


  Pen::Pen( TTF_Font* font, SDL_Color colour ) :
  _ttfFont( font ),
  _colour( colour )
  {
  }


  Pen::~Pen()
  {
  }


  SDL_Surface* Pen::shortWrite( std::string& text ) const
  {
    SDL_Surface* surface = TTF_RenderText_Solid( _ttfFont, text.c_str(), _colour );

    if ( surface == nullptr )
    {
      Exception ex( "Pen::shortWrite()", "Could not render text" );
      ex.addDetail( "Text string", text );
      ex.addDetail( "TTF error", TTF_GetError() );
      throw ex;
    }

    return surface;
  }


  SDL_Surface* Pen::paragraphWrite( std::string& text ) const
  {
    SDL_Surface* surface = TTF_RenderText_Solid( _ttfFont, text.c_str(), _colour );

    if ( surface == nullptr )
    {
      Exception ex( "Pen::shortWrite()", "Could not render text" );
      ex.addDetail( "Text string", text );
      ex.addDetail( "TTF error", TTF_GetError() );
      throw ex;
    }

    return surface;
  }

}

