//#define __DEBUG_OFF__

#include "Texture.h"

#include "Exception.h"
#include "Camera.h"

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
    _destination( { 0, 0, 0, 0 } ),
    _theRenderer( nullptr )
  {
  }


  Texture::Texture( RawTexture tex) :
    _theTexture( tex ),
    _angle( 0.0 ),
    _flipFlag( SDL_FLIP_NONE ),
    _clip( { 0, 0, 0, 0 } ),
    _destination( { 0, 0, 0, 0 } ),
    _theRenderer( nullptr )
  {
  }


  Texture::Texture( Texture&& other ) :
    _theTexture( std::exchange( other._theTexture, { nullptr, 0, 0 } ) ),
    _angle( std::move( other._angle ) ),
    _flipFlag( std::move( other._flipFlag ) ),
    _clip( std::move( other._clip ) ),
    _destination( std::move( other._destination) ),
    _theRenderer( std::move( other._theRenderer ) )
  {
  }


  Texture& Texture::operator=( Texture&& other ) 
  {
    // Move the data memebers
    _theTexture =  std::exchange( other._theTexture, { nullptr, 0, 0 } );
    _angle = std::move( other._angle );
    _flipFlag = std::move( other._flipFlag );
    _clip = std::move( other._clip );
    _destination = std::move( other._destination );
    _theRenderer = std::move( other._theRenderer );

    return *this;
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


  void Texture::render( Camera* camera )
  {
    if ( _theRenderer == nullptr )
      ERROR_LOG( "No renderer present!" );

    if ( _theTexture.texture == nullptr )
      ERROR_LOG( "No texture present" );

    DEBUG_LOG( "Placing destination rectangle" );
    SDL_Rect newDestination = camera->place( _destination );
    DEBUG_STREAM << "Destination : " << newDestination.x << ", " << newDestination.y << ", " << newDestination.w << ", " << newDestination.h;

    // Render it to the window
    SDL_RenderCopyEx( _theRenderer, _theTexture.texture, &_clip, &newDestination, _angle, nullptr, _flipFlag );
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

}

