
#include "Sprite.h"

#include "Camera.h"

namespace Regolith
{

  Sprite::Sprite() :
    Drawable(),
    _spriteSheet(),
    _destination()
  {
  }


  Sprite::Sprite( SpriteSheet sheet ) :
    Drawable( sheet.getWidth(), sheet.getHeight() ),
    _spriteSheet( sheet ),
    _destination( { 0, 0, sheet.getWidth(), sheet.getHeight() } )
  {
  }


  Sprite::~Sprite()
  {
  }


  void Sprite::render( Camera* camera )
  {
    // Set the current sprite position
    _destination.x = position().x();
    _destination.y = position().y();
    
    // Move into the camera reference frame
    SDL_Rect destination = camera->place( _destination );

    _spriteSheet.draw( &destination );
  }


  Drawable* Sprite::clone() const
  {
    return (Drawable*) new Sprite( *this );
  }

}

