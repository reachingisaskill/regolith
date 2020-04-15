
#ifndef __REGOLITH__TEXTURE_WRAPPER_H__
#define __REGOLITH__TEXTURE_WRAPPER_H__

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

    private:
      SDL_Texture* _theTexture;
      int _width;
      int _height;

      double _angle;
      SDL_RendererFlip _flipFlag;

      SDL_Renderer* _theRenderer;

    protected:

    public:
      Texture();

      Texture( Texture&& ); // Move constructor

      Texture& operator=( Texture&& ); // Move assignment

      ~Texture();

      void setRenderer( SDL_Renderer* rend ) { _theRenderer = rend; }

      // Accessors
      int getWidth() const { return _width; }
      int getHeight() const { return _height; }


      // Delete all the heap allocated memory and reset
      void free();

      // Initialise from a texture file
      void loadFromFile( std::string, SDL_Color* key = nullptr );

      // Initialise from a string
      void loadFromText( std::string, TTF_Font*, SDL_Color );

      // Render with the current renderer object
      void render( int x, int y, SDL_Rect* clip = nullptr );

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

#endif // __REGOLITH__TEXTURE_WRAPPER_H__

