#define LOGTASTIC_DEBUG_OFF

#include "GameObjects/Sprite.h"
#include "Components/Camera.h"

#include "logtastic.h"


namespace Regolith
{

  Sprite::Sprite() :
    Drawable(),
    _texture(),
    _destination(),
    _collision( nullptr )
  {
  }


  Sprite::Sprite( Texture tex ) :
    Drawable( tex.getWidth(), tex.getHeight() ),
    _texture( tex ),
    _destination( { 0, 0, tex.getWidth(), tex.getHeight() } ),
    _collision( nullptr )
  {
  }


  Sprite::Sprite( const Sprite& sp ) :
    Drawable( sp ),
    _texture( sp._texture ),
    _destination( sp._destination ),
    _collision( nullptr )
  {
    if ( sp._collision != nullptr )
      _collision = sp._collision->clone();
  }


  Sprite::~Sprite()
  {
    if ( _collision != nullptr )
    {
      delete _collision;
      _collision = nullptr;
    }
  }


  void Sprite::update( Uint32 time )
  {
    if ( _texture.isAnimated() )
    {
      _texture.update( time );
    }

    this->step( time );
  }


  void Sprite::render( Camera* camera )
  {
    // Set the current sprite position
    _destination.x = position().x();
    _destination.y = position().y();
    
    // Move into the camera reference frame
    SDL_Rect destination = camera->place( _destination );
    DEBUG_STREAM << "RENDER: Original: " << _destination.w << ", " << _destination.h << ", NEW: " << destination.w << ", " << destination.h;

    _texture.draw( &destination );
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


  void Sprite::addCollision( Collision* col )
  {
    if ( _collision != nullptr )
      delete _collision;

    _collision = col;
  }

}

