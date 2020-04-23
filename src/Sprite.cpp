
#include "Sprite.h"

#include "Camera.h"

namespace Regolith
{

  Sprite::Sprite() :
    Drawable(),
    _spriteSheet(),
    _destination(),
    _collision( nullptr )
  {
  }


  Sprite::Sprite( SpriteSheet sheet ) :
    Drawable( sheet.getWidth(), sheet.getHeight() ),
    _spriteSheet( sheet ),
    _destination( { 0, 0, sheet.getWidth(), sheet.getHeight() } ),
    _collision( nullptr )
  {
  }


  Sprite::Sprite( const Sprite& sp ) :
    Drawable( sp ),
    _spriteSheet( sp._spriteSheet ),
    _destination( sp._destination ),
    _collision( nullptr )
  {
    if ( sp._collision != nullptr )
      _collision = sp._collision->clone();
  }


  Sprite::~Sprite()
  {
    delete _collision;
  }


  int Sprite::getProperties() const
  {
    if ( _collision == nullptr )
      return OBJECT_SIMPLE;
    else
      return OBJECT_SIMPLE & OBJECT_HAS_COLLISION;
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


  unsigned int Sprite::getCollision( Collision*& col )
  {
    col = _collision;
    return 1;
  }

}

