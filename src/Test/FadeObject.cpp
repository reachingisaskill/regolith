
#include "Regolith/Test/FadeObject.h"
#include "Regolith/Managers/ContextGroup.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  FadeObject::FadeObject() :
    PhysicalObject(),
    DrawableObject(),
    AnimatedObject(),
    _texture(),
    _fade( 0.0 ),
    _delay( 0.0 ),
    _alpha( 0 ),
    _time( 0.0 )
  {
  }


  FadeObject::FadeObject( const FadeObject& other ) :
    PhysicalObject( other ),
    DrawableObject( other ),
    AnimatedObject( other ),
    _texture( other._texture ),
    _fade( other._fade ),
    _alpha( 0 ),
    _time( 0.0 )
  {
  }


  FadeObject::~FadeObject()
  {
  }


  void FadeObject::configure( Json::Value& json_data, ContextGroup& cg )
  {
    // Configure the physical base class
    PhysicalObject::configure( json_data, cg );

    validateJson( json_data, "texture", JsonType::OBJECT );
    validateJson( json_data, "fade_time", JsonType::FLOAT );

    // Configure the parent class first
    PhysicalObject::configure( json_data, cg);

    // Texture details
    _texture.configure( json_data["texture"], cg.getDataHandler() );

    // Fade out time
    _fade = json_data["fade_time"].asFloat();
    _delay = json_data["delay"].asFloat();

    DEBUG_STREAM << "FadeObject::configure : Configured : " << _fade;
  }


  void FadeObject::reset()
  {
    // Reset the parent object
    PhysicalObject::reset();

    //Reset the alpha value
    _alpha = 0;
    _texture.setAlpha( _alpha );
    _time = 0.0;
  }


  Texture& FadeObject::getTexture()
  {
    return _texture;
  }


  void FadeObject::update( float time )
  {
    _time += time;

    if ( ( _time > _delay ) && ( _alpha < 255 ) )
    {
      float fade_time = _time - _delay;
      float temp_alpha = (fade_time / _fade) * 255;
      DEBUG_STREAM << "FadeObject::update : Calculated Alpha = " << temp_alpha;

      if ( temp_alpha > 255.0 ) _alpha = 255;
      else _alpha = (Uint8) temp_alpha;

      DEBUG_STREAM << "FadeObject::update : Actual Alpha = " << (int)_alpha;

      _texture.setAlpha( _alpha );
    }
  }

}

