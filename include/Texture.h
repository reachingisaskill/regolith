
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
      // Class owns this memory
      RawTexture _theTexture;
      float _angle;
      SDL_RendererFlip _flipFlag;
      SDL_Rect _clip;

    protected:

    public:
      Texture();

      explicit Texture( RawTexture );

      virtual ~Texture();

      // Fullfill the Drawable class requirements

      // Render with the current renderer object
      virtual void draw( SDL_Rect* );


      // Accessors
      virtual int getWidth() const;
      virtual int getHeight() const;
      virtual float getAngle() const { return _angle; }


      // Set the clip around the texture
      virtual void setClip( SDL_Rect c ) { _clip = c; }
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
  };
}

#endif // __REGOLITH__TEXTURE_H__

