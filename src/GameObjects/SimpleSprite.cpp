
#include "Regolith/GameObjects/SimpleSprite.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/GamePlay/Camera.h"


namespace Regolith
{

  SimpleSprite::SimpleSprite() :
    Drawable(),
    _texture(),
    _destination()
  {
  }


  SimpleSprite::~SimpleSprite()
  {
  }


  void SimpleSprite::render( SDL_Renderer* renderer, const Camera& camera ) const
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


  PhysicalObject* SimpleSprite::clone( const Vector& pos ) const
  {
    PhysicalObject* temp = new SimpleSprite( *this );
    temp->setPosition( pos );
    return temp;
  }


  void SimpleSprite::configure( Json::Value& json_data, ContextGroup& cg, DataHandler& handler )
  {
    Drawable::configure( json_data, cg, handler );

    _texture.configure( json_data, handler );
  }

}

