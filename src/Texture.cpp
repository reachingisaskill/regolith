#define __DEBUG_OFF__

#include "Texture.h"

#include "Exception.h"
#include "Camera.h"
#include "Manager.h"

#include "logtastic.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <utility>


namespace Regolith
{

  Texture::Texture() :
    _theTexture( { nullptr, 0, 0 } ),
    _angle( 0.0 ),
    _flipFlag( SDL_FLIP_NONE ),
    _clip( { 0, 0, 0, 0 } )
  {
  }


  Texture::Texture( RawTexture tex) :
    _theTexture( tex ),
    _angle( 0.0 ),
    _flipFlag( SDL_FLIP_NONE ),
    _clip( { 0, 0, 0, 0 } )
  {
  }


  Texture::~Texture()
  {
  }


  int Texture::getWidth() const
  {
    return _theTexture.width;
  }


  int Texture::getHeight() const
  {
    return _theTexture.height;
  }


  void Texture::draw( SDL_Rect* destination )
  {
    static SDL_Renderer* renderer = Manager::getInstance()->getRendererPointer();
    // Render it to the window
    SDL_RenderCopyEx( renderer, _theTexture.texture, &_clip, destination, _angle, nullptr, _flipFlag );
  }


  void Texture::setColor( Uint8 red, Uint8 green, Uint8 blue )
  {
    SDL_SetTextureColorMod( _theTexture.texture, red, green, blue );
  }


  void Texture::setAlpha( Uint8 alpha )
  {
    SDL_SetTextureAlphaMod( _theTexture.texture, alpha );
  }


  void Texture::setBlendMode( SDL_BlendMode blendmode )
  {
    SDL_SetTextureBlendMode( _theTexture.texture, blendmode );
  }


  void Texture::setRotation( double angle )
  {
    _angle = angle;
  }


  void Texture::setFlip( SDL_RendererFlip flip )
  {
    _flipFlag = flip;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Useful functions

  RawTexture makeTextureFromText( TTF_Font* font, std::string textureString, SDL_Color color )
  {
    static SDL_Renderer* renderer = Manager::getInstance()->getRendererPointer();
    RawTexture theTexture;

    SDL_Surface* textSurface = TTF_RenderText_Solid( font, textureString.c_str(), color );
    if ( textSurface == nullptr )
    {
      std::cerr << "Could not create text surface. Error : " << TTF_GetError() << std::endl;
    }
    else
    {
      // Create the texture from the surface
      theTexture.texture = SDL_CreateTextureFromSurface( renderer, textSurface );
      if ( theTexture.texture == nullptr )
      {
        std::cerr << "Could not create texture from surface. Error : " << SDL_GetError() << std::endl;
      }
      else
      {
        theTexture.width = textSurface->w;
        theTexture.height = textSurface->h;
      }
      // Remove the unneeded surface
      SDL_FreeSurface( textSurface );
    }

    return theTexture;
  }

}

