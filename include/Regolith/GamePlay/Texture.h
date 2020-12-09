
#ifndef REGOLITH_GAMEPLAY_TEXTURE_H_
#define REGOLITH_GAMEPLAY_TEXTURE_H_

#include "Regolith/Global/Global.h"

#include <iostream>
#include <string>


namespace Regolith
{
  // Forward declaration
  class Camera;
  class DataHandler;
  class RawTextureDetail;
  class RawStringDetail;

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Raw Texture Structure (Because SDL_Texture's don't store their dimensions).

  struct RawTexture
  {
    SDL_Texture* sdl_texture;
    SDL_Surface* surface;
    int width;
    int height;
    unsigned short int rows;
    unsigned short int columns;
    unsigned short cells;
    bool update;

    RawTexture();
    RawTexture( SDL_Texture*, int, int, unsigned short, unsigned short );
    RawTexture( SDL_Texture*, int, int, unsigned short, unsigned short, unsigned short );
  };

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Surface creation functions

  SDL_Surface* loadSurfaceFromFile( std::string, const SDL_Color& );
  SDL_Surface* loadSurfaceFromString( std::string, TTF_Font*, const SDL_Color& );


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Texture class. The simplest object of renderable data.

  class Texture
  {
    // Make the camera a friend
    friend class Camera;

    private:
      // Pointer to the raw sdl texture
      mutable RawTexture* _rawTexture;
      SDL_RendererFlip _flipFlag;
      SDL_Rect _clip;

      // If its a spritesheet
      unsigned int _currentSprite;

    protected:
      // Return a reference to the clip object
      SDL_Rect& clip() { return _clip; }

      // Manually set the clip around the texture (not compatible with spritesheet/animations)
      virtual void setClip( SDL_Rect c );

    public:
      Texture();

      virtual ~Texture();


      // Sets a new surface to the raw texture. Forces the renderer to re-draw the texture using the new surface
      void setNewSurface( SDL_Surface* );

      // Configures as a sprite sheet with optional animation. No. rows, No. Columns, and No. of used cells and update period
      // This function is optinal. Without it this acts as a single flat texture
      void configure( Json::Value&, DataHandler& );


      // Accessors
      int getWidth() const { return _clip.w; }
      int getHeight() const { return _clip.h; }


      // Colour Modulation
      void setColor( Uint8, Uint8, Uint8 );

      // Modulate the opacity
      void setAlpha( Uint8 );

      // Set the blend mode
      void setBlendMode( SDL_BlendMode );

      // Set the transformation info
      void setRotation( double );

      // Flip the image
      void setFlip( SDL_RendererFlip );

      // Flip vertically
      void flipVertical();

      // Flip horizontally
      void flipHorizontal();


      // Update which cell of the sprite sheet should be displayed
      void setFrameNumber( unsigned int );

      // Sprite details
      int getNumberSprites() const { return _rawTexture->cells; }
      const unsigned int& currentSpriteNumber() { return _currentSprite; }
  };
}

#endif // REGOLITH_GAMEPLAY_TEXTURE_H_

