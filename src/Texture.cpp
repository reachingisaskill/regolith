
#include "Texture.h"

#include "Exception.h"

#include "logtastic.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <utility>


namespace Regolith
{

  Texture::Texture() :
    _theTexture( nullptr ),
    _width( 0 ),
    _height( 0 ),
    _angle( 0.0 ),
    _flipFlag( SDL_FLIP_NONE ),
    _theRenderer( nullptr )
  {
  }


  Texture::Texture( Texture&& other ) :
    _theTexture( std::exchange( other._theTexture, nullptr ) ),
    _width( std::move( other._width ) ),
    _height( std::move( other._height ) ),
    _angle( std::move( other._angle ) ),
    _flipFlag( std::move( other._flipFlag ) ),
    _theRenderer( std::move( other._theRenderer ) )
  {
  }


  Texture& Texture::operator=( Texture&& other ) 
  {
    // Clear the class first
    this->free();

    // Move the data memebers
    _theTexture =  std::exchange( other._theTexture, nullptr );
    _width = std::move( other._width );
    _height = std::move( other._height );
    _angle = std::move( other._angle );
    _flipFlag = std::move( other._flipFlag );
    _theRenderer = std::move( other._theRenderer );

    return *this;
  }


  Texture::~Texture()
  {
    // Delete all allocated memory
    this->free();
  }


  void Texture::free()
  {
    this->_free();

    if ( _theTexture != nullptr )
    {
      SDL_DestroyTexture( _theTexture );
      _width = 0;
      _height = 0;
    }
  }


  void Texture::_free()
  {
  }


  void Texture::render()
  {
    if ( _theRenderer == nullptr )
      ERROR_LOG( "No renderer present!" );

    if ( _theTexture == nullptr )
      ERROR_LOG( "No texture present" );

    // Render it to the window
    SDL_RenderCopyEx( _theRenderer, _theTexture, &_clip, &_destination, _angle, nullptr, _flipFlag );
  }


  void Texture::_setClip( SDL_Rect clip )
  {
    _clip = clip;
    _destination.w = _clip.w;
    _destination.h = _clip.h;
  }


  void Texture::setPosition( int x, int y )
  {
    _destination.x = x;
    _destination.y = y;
  }


  void Texture::setColor( Uint8 red, Uint8 green, Uint8 blue )
  {
    SDL_SetTextureColorMod( _theTexture, red, green, blue );
  }


  void Texture::setAlpha( Uint8 alpha )
  {
    SDL_SetTextureAlphaMod( _theTexture, alpha );
  }


  void Texture::setBlendMode( SDL_BlendMode blendmode )
  {
    SDL_SetTextureBlendMode( _theTexture, blendmode );
  }


  void Texture::setRotation( double angle )
  {
    _angle = angle;
  }


  void Texture::setFlip( SDL_RendererFlip flip )
  {
    _flipFlag = flip;
  }

}

