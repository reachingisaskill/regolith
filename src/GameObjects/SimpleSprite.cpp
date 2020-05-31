//#define LOGTASTIC_DEBUG_OFF

#include "Regolith/GameObjects/SimpleSprite.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/GamePlay/Camera.h"

#include "logtastic.h"


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


  void SimpleSprite::render( const Camera& camera )
  {
    // Place the object
    _destination.x = position().x();
    _destination.y = position().y();
    
    // Move into the camera reference frame
    SDL_Rect destination = camera.place( _destination );
    DEBUG_STREAM << "RENDER: Original: " << _destination.w << ", " << _destination.h << ", NEW: " << destination.x << ", " << destination.y << ", " << destination.w << ", " << destination.h;

    _texture.draw( &destination );
  }


  PhysicalObject* SimpleSprite::clone( const Vector& pos ) const
  {
    PhysicalObject* temp = new SimpleSprite( *this );
    temp->setPosition( pos );
    return temp;
  }


  void SimpleSprite::configure( Json::Value& json_data, DataHandler& handler )
  {
    Drawable::configure( json_data, handler );

    _texture.configure( json_data, handler );

    // Set the current sprite position
    _destination.w = _texture.getWidth();
    _destination.h = _texture.getHeight();

    setWidth( _texture.getWidth() );
    setHeight( _texture.getHeight() );
  }

}

