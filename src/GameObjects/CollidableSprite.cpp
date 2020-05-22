//#define LOGTASTIC_DEBUG_OFF

#include "Regolith/GameObjects/CollidableSprite.h"
#include "Regolith/GamePlay/Camera.h"

#include "logtastic.h"


namespace Regolith
{

  CollidableSprite::CollidableSprite() :
    Drawable(),
    Collidable(),
    _texture(),
    _destination()
  {
  }


  CollidableSprite::~CollidableSprite()
  {
  }


  void CollidableSprite::render( const Camera& camera )
  {
    // Place the object
    _destination.x = position().x();
    _destination.y = position().y();
    
    // Move into the camera reference frame
    SDL_Rect destination = camera.place( _destination );
    DEBUG_STREAM << "RENDER: Original: " << _destination.w << ", " << _destination.h << ", NEW: " << destination.x << ", " << destination.y << ", " << destination.w << ", " << destination.h;

    _texture.draw( &destination );
  }


  PhysicalObject* CollidableSprite::clone( const Vector& pos ) const
  {
    PhysicalObject* temp = new CollidableSprite( *this );
    temp->setPosition( pos );
    return temp;
  }


  void CollidableSprite::configure( Json::Value& json_data )
  {
    Drawable::configure( json_data );
    Collidable::configure( json_data );

    _texture.configure( json_data );

    // Set the current sprite position
    _destination.w = _texture.getWidth();
    _destination.h = _texture.getHeight();

    setWidth( _texture.getWidth() );
    setHeight( _texture.getHeight() );
  }


  void CollidableSprite::onCollision( const Vector&, float, const Collidable* )
  {
  }

}

