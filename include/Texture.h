
#ifndef __REGOLITH__TEXTURE_H__
#define __REGOLITH__TEXTURE_H__

#include "Definitions.h"
#include "Drawable.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <string>


namespace Regolith
{
  // Forward declaration
  class Camera;

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Raw Texture Structure (Because SDL_Texture's don't store their dimensions).

  struct RawTexture
  {
    SDL_Texture* texture;
    int width;
    int height;
  };

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Useful functions

  RawTexture makeTextureFromText( TTF_Font*, std::string, SDL_Color );

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Texture class. The simplest object of renderable data.

  class Texture
  {
    private:
      // Pointer to the raw sdl texture
      RawTexture _theTexture;
      float _angle;
      SDL_RendererFlip _flipFlag;
      SDL_Rect _clip;

      // Dimensions after clipping, etc
      int _spriteWidth;
      int _spriteHeight;

      // If its a spritesheet
      int _currentSprite;
      int _rows;
      int _columns;
      int _numSprites;

      // If its animated
      Uint32 _updatePeriod;
      Uint32 _count;

    protected:
      SDL_Rect& clip() { return _clip; }

    public:
      Texture();

      explicit Texture( RawTexture );

      virtual ~Texture();


      // Configures as a sprite sheet with optional animation. No. rows, No. Columns, and No. of used cells and update period
      // This function is optinal. Without it this acts as a single flat texture
      void configure( int, int, int number = 0, Uint32 period = 0 );

      // Returns true if the spritesheeet is to be updated every frame
      bool isAnimated() const { return _updatePeriod > 0u; }


      // Render with the current renderer object
      virtual void draw( SDL_Rect* );


      // Accessors
      int getWidth() const { return _spriteWidth; }
      int getHeight() const { return _spriteHeight; }
      virtual float getAngle() const { return _angle; }


      // Manually set the clip around the texture (not compatible with spritesheet/animations)
      virtual void setClip( SDL_Rect c );
      virtual SDL_Rect getClip() { return _clip; }


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


      // Animation update period
      void setUpdatePeriod( Uint32 );

      // Update with time in ms
      virtual void update( Uint32 );


      // Sprite details
      int getNumberSprites() const { return _numSprites; }
      void setSpriteNumber( int );
      const int& currentSpriteNumber() { return _currentSprite; }
  };
}

#endif // __REGOLITH__TEXTURE_H__

