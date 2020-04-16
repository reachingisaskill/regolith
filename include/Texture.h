
#ifndef __REGOLITH__TEXTURE_H__
#define __REGOLITH__TEXTURE_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <string>


namespace Regolith
{

  class Texture
  {
    Texture( const Texture& ) = delete; // Non-copy-constructable
    Texture& operator=( const Texture& ) = delete; // Non-copy-assignable

    friend class TextureBuilder;

    private:
      // Class owns this memory
      SDL_Texture* _theTexture;
      int _width;
      int _height;
      float _angle;
      SDL_RendererFlip _flipFlag;
      SDL_Rect _clip;
      SDL_Rect _destination;

      // Pointer to object owned by a manager class
      SDL_Renderer* _theRenderer;

    protected:
      virtual void _free(); // For derived classes to implement

      virtual void _setClip( SDL_Rect );

    public:
      Texture();

      Texture( Texture&& ); // Move constructor

      Texture& operator=( Texture&& ); // Move assignment

      virtual ~Texture();

      void setRenderer( SDL_Renderer* rend ) { _theRenderer = rend; }

      // Accessors
      virtual int getWidth() const { return _width; }
      virtual int getHeight() const { return _height; }
      virtual float getAngle() const { return _angle; }


      // Delete all the heap allocated memory and reset
      void free();

      // Render with the current renderer object
      virtual void render();

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

