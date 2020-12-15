
#ifndef REGOLITH_ASSETS_RAW_TEXTURE_H_
#define REGOLITH_ASSETS_RAW_TEXTURE_H_

#include "Regolith/Global/Global.h"


namespace Regolith
{

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Raw Texture Structure
  /*
   * Stores details are specfic to the sdl_texture it points to.
   * Remember that multiple textures may use the same SDL_Texture during rendering.
   * Hence all modifications - e.g. rotations, etc must be applied to the Texture objects, not the RawTexture.
   */
  struct RawTexture
  {
    SDL_Texture* sdl_texture;
    SDL_Surface* surface;
    int width;
    int height;
    unsigned short int rows;
    unsigned short int columns;
    unsigned short cells;

    RawTexture();
    RawTexture( SDL_Texture*, int, int, unsigned short, unsigned short );
    RawTexture( SDL_Texture*, int, int, unsigned short, unsigned short, unsigned short );

    ~RawTexture();
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Surface creation functions

  SDL_Surface* loadSurfaceFromFile( std::string, const SDL_Color& );
}

#endif // REGOLITH_ASSETS_RAW_TEXTURE_H_

