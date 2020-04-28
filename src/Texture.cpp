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
    _clip( { 0, 0, 0, 0 } ),
    _spriteWidth( 0 ),
    _spriteHeight( 0 ),
    _currentSprite( 0 ),
    _rows( 0 ),
    _columns( 0 ),
    _numSprites( 0 ),
    _updatePeriod( 0 ),
    _count( 0 )
  {
  }


  Texture::Texture( RawTexture tex) :
    _theTexture( tex ),
    _angle( 0.0 ),
    _flipFlag( SDL_FLIP_NONE ),
    _clip( { 0, 0, tex.width, tex.height } ),
    _spriteWidth( tex.width ),
    _spriteHeight( tex.height ),
    _currentSprite( 0 ),
    _rows( 0 ),
    _columns( 0 ),
    _numSprites( 0 ),
    _updatePeriod( 0 ),
    _count( 0 )
  {
  }


  Texture::~Texture()
  {
  }


  void Texture::configure( int rows, int cols, int number, Uint32 period )
  {
    _rows = rows;
    _columns = cols;
    _updatePeriod = period;

    if ( number == 0 )
    {
      _numSprites = _rows * _columns;
    }
    else
    {
      _numSprites = number;
    }

    _spriteWidth = Texture::getWidth() / _columns;
    _spriteHeight = Texture::getHeight() / _rows;

    this->setSpriteNumber( 0 );
  }


  void Texture::draw( SDL_Rect* destination )
  {
    static SDL_Renderer* renderer = Manager::getInstance()->getRendererPointer();
    // Render it to the window
    SDL_RenderCopyEx( renderer, _theTexture.texture, &_clip, destination, _angle, nullptr, _flipFlag );
  }


  void Texture::setClip( SDL_Rect c )
  {
    _spriteWidth = c.w;
    _spriteHeight = c.h;
    _clip = c;
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


  void Texture::setSpriteNumber( int num )
  {
    _currentSprite = num;
    int sprite_x = (_currentSprite % _columns) * _spriteWidth;
    int sprite_y = (_currentSprite / _columns) * _spriteHeight;

    _clip.x = sprite_x;
    _clip.y = sprite_y;
    _clip.w = _spriteWidth;
    _clip.h = _spriteHeight;
  }


  void Texture::update( Uint32 timestep )
  {
    _count += timestep;

    int frame_number = _currentSprite;

    frame_number += _count / _updatePeriod;
    frame_number = frame_number % _numSprites;
    this->setSpriteNumber( frame_number );
    _count = _count % _updatePeriod;

    DEBUG_STREAM << "Sprite Animation : _count : " << _count << ", frame No. : " << frame_number << ", update rate : " << _updatePeriod;
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

