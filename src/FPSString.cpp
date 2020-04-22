#define __DEBUG_OFF__

#include "FPSString.h"

#include "Texture.h"

#include "logtastic.h"


namespace Regolith
{

  FPSString::FPSString() :
    Drawable(),
    _theTexture( { nullptr, 0, 0 } ),
    _phrase( "Current FPS: " ),
    _frameCounter( 0 ),
    _limit( 1000 ),
    _tickCount( 0 ),
    _stringstream(),
    _theFont( nullptr ),
    _theColor( { 0, 0, 0, 0 } ),
    _destination( { 0, 0, 0, 0 } )
  {
  }


  FPSString::FPSString( const FPSString& other ) :
    Drawable( other ),
    _theTexture( other._theTexture ),
    _phrase( other._phrase ),
    _frameCounter( other._frameCounter ),
    _limit( other._limit ),
    _tickCount( other._tickCount ),
    _stringstream(),
    _theFont( other._theFont ),
    _theColor( other._theColor ),
    _destination( other._destination )
  {
  }


  FPSString::~FPSString()
  {
  }


  void FPSString::setPosition( int x, int y )
  {
    _destination.x = x;
    _destination.y = y;
  }


  void FPSString::render( Camera* camera )
  {
    SDL_Rect newDestination = camera->place( _destination );

    // Render it to the window
    SDL_RenderCopy( getRenderer(), _theTexture.texture, nullptr, &newDestination );
  }


  void FPSString::update( Uint32 time )
  {
    ++_frameCounter;
    _tickCount += time;

    if ( _tickCount > _limit )
    {
      DEBUG_STREAM << _tickCount << ", " << _limit << ", " << _frameCounter;
      SDL_DestroyTexture( _theTexture.texture );

      updateString();

      _frameCounter = 0;
      _tickCount = 0;
    }
  }


  Drawable* FPSString::clone() const
  {
    return (Drawable*) new FPSString( *this );
  }


  void FPSString::updateString()
  {
    _stringstream.str( "" );
    float fps = _frameCounter / (1.0e-3 * _tickCount);

    _stringstream << _phrase << fps;

    _theTexture = makeTextureFromText( _theFont, _stringstream.str().c_str(), _theColor );

    _destination.w = _theTexture.width;
    _destination.h = _theTexture.height;
  }
}
