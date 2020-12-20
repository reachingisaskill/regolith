
#include "Regolith/Assets/RawTexture.h"


namespace Regolith
{

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Raw Texture creation function

  RawTexture loadRawTexture( ImageDetail details )
  {
    RawTexture raw_texture;

    raw_texture.sdl_texture = nullptr;
    raw_texture.width = details.width;
    raw_texture.height = details.height;
    raw_texture.rows = details.rows;
    raw_texture.columns = details.columns;
    raw_texture.cells = details.rows * details.columns;

    // Load the image into a surface
    raw_texture.surface = IMG_Load( details.filename.c_str() );
    if ( raw_texture.surface == nullptr )
    {
      Exception ex( "loadRawTexture()", "Could not load image data", false );
      ex.addDetail( "Image path", details.filename );
      ex.addDetail( "SDL IMG error", IMG_GetError() );
      throw ex;
    }

    if ( details.colourkey.a != 0 )
    {
      SDL_SetColorKey( raw_texture.surface, SDL_TRUE, SDL_MapRGB( raw_texture.surface->format, details.colourkey.r, details.colourkey.g, details.colourkey.b ) );
    }

    return raw_texture;
  }

}

