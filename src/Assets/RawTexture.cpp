
#include "Regolith/Assets/RawTexture.h"


namespace Regolith
{

  RawTexture::RawTexture() :
    sdl_texture( nullptr ),
    surface( nullptr ),
    width( 0 ),
    height( 0 ),
    rows( 0 ),
    columns( 0 ),
    cells( 0 )
  {
  }


  RawTexture::RawTexture( SDL_Texture* t, int w, int h, unsigned short r, unsigned short c ) :
    sdl_texture( t ),
    surface( nullptr ),
    width( w ),
    height( h ),
    rows( r ),
    columns( c ),
    cells( r*c )
  {
  }


  RawTexture::RawTexture( SDL_Texture* t, int w, int h, unsigned short r, unsigned short c, unsigned short n ) :
    sdl_texture( t ),
    surface( nullptr ),
    width( w ),
    height( h ),
    rows( r ),
    columns( c ),
    cells( n )
  {
  }


  RawTexture::~RawTexture()
  {
    if ( surface != nullptr )
    {
      SDL_FreeSurface( surface );
    }
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Texture creation functions

  SDL_Surface* loadSurfaceFromFile( std::string path, const SDL_Color& key )
  {
    // Load the image into a surface
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if ( loadedSurface == nullptr )
    {
      Exception ex( "loadSurfaceFromFile()", "Could not load image data", false );
      ex.addDetail( "Image path", path );
      ex.addDetail( "SDL_img error", IMG_GetError() );
      throw ex;
    }

    if ( key.a != 0 )
    {
      SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, key.r, key.g, key.b ) );
    }

    return loadedSurface;
  }

}

