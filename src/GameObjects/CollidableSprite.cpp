#define LOGTASTIC_DEBUG_OFF

#include "Regolith/GameObjects/CollidableSprite.h"
#include "Regolith/Managers/DataHandler.h"
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


  void CollidableSprite::render( SDL_Renderer* renderer, const Camera& camera ) const
  {
    // Place the object
    _destination.x = position().x();
    _destination.y = position().y();
    _destination.w = getWidth();
    _destination.h = getHeight();
    
    // Move into the camera reference frame
    SDL_Rect destination = camera.place( _destination );
    DEBUG_STREAM << "RENDER: Original: " << _destination.w << ", " << _destination.h << ", NEW: " << destination.x << ", " << destination.y << ", " << destination.w << ", " << destination.h;

    _texture.draw( renderer, &destination );
  }


  PhysicalObject* CollidableSprite::clone( const Vector& pos ) const
  {
    PhysicalObject* temp = new CollidableSprite( *this );
    temp->setPosition( pos );
    return temp;
  }


  void CollidableSprite::configure( Json::Value& json_data, ContextGroup& cg, DataHandler& handler )
  {
    Drawable::configure( json_data, cg, handler );
    Collidable::configure( json_data, cg, handler );

    _texture.configure( json_data, handler );
  }


  void CollidableSprite::onCollision( const Vector&, float, const Collidable* )
  {
  }

}

