
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

  class Texture : public Drawable
  {
    private:
      // Class owns this memory
      RawTexture _theTexture;
      float _angle;
      SDL_RendererFlip _flipFlag;
      SDL_Rect _clip;
      SDL_Rect _destination;

    protected:

    public:
      Texture();

      Texture( RawTexture );

      virtual ~Texture();

      // Fullfill the Drawable class requirements

      // Return the object properties
      virtual int getProperties() const { return OBJECT_SIMPLE; }

      // For derived classes to update every frame
      virtual void update( Uint32 ) { }

      // Render with the current renderer object
      virtual void render( Camera* );

      // Handle events
      virtual void handleEvent( SDL_Event& ) {}

      // Returns the collision object for the class;
      virtual unsigned int getCollision( Collision*& ) { return 0; }


      // Accessors
      virtual int getWidth() const;
      virtual int getHeight() const;
      virtual float getAngle() const { return _angle; }

      // Set the clip around the texture
      virtual void setClip( SDL_Rect );

      virtual void setPosition( int, int );

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

