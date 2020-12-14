
#include "Regolith/SimpleObject.h"
#include "Regolith/Managers/ContextGroup.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  SimpleObject::SimpleObject() :
    PhysicalObject(),
    _texture(),
    _collision(),
    _updatePeriod( 0.0 ),
    _count( 0.0 ),
    _numberFrames( 0 ),
    _currentFrame( 0 )
  {
  }


  SimpleObject::SimpleObject( const SimpleObject& other ) :
    PhysicalObject( other ),
    _texture( other._texture ),
    _collision( other._collision ),
    _updatePeriod( other._updatePeriod ),
    _count( other._count ),
    _numberFrames( other._numberFrames ),
    _currentFrame( 0 )
  {
  }


  SimpleObject::~SimpleObject()
  {
  }


  void SimpleObject::configure( Json::Value& json_data, ContextGroup& cg )
  {
    // Configure the parent class first
    PhysicalObject::configure( json_data, cg);

    // Texture details
    if ( Utilities::validateJson( json_data, "texture", Utilities::JSON_TYPE_OBJECT, this->hasTexture() ) )
    {
      _texture.configure( json_data["texture"], cg.getDataHandler() );
    }
    
    // Hitbox details
    if ( Utilities::validateJson( json_data, "collision", Utilities::JSON_TYPE_OBJECT, false ) )
    {
      _collision.configure( json_data["collision"] );
    }

    // Animation details
    if ( Utilities::validateJson( json_data, "update_period", Utilities::JSON_TYPE_FLOAT, this->hasAnimation() ) )
    {
      _updatePeriod = json_data[ "update_period" ].asFloat();
    }
  }


  void SimpleObject::reset()
  {
    // Reset the parent object
    PhysicalObject::reset();

    // Set curret frame and current frame count to zero
    _count = 0.0;
    _currentFrame = 0;
  }


  Texture& SimpleObject::getTexture()
  {
    return _texture;
  }


  Collision& SimpleObject::getCollision()
  {
    return _collision;
  }


  void SimpleObject::update( float timestep )
  {
    // Update the timer
    _count += timestep;

    // Determine how many frames have elapsed in the animation
    _currentFrame += _count / _updatePeriod;
    _currentFrame = _currentFrame % _numberFrames;

    // Reset the timer based on the update period
    _count = std::fmod( _count, _updatePeriod );

    // Update the texture and collision objects
    _texture.setFrameNumber( _currentFrame );
    _collision.setFrameNumber( _currentFrame );

    DEBUG_STREAM << "SimpleObject::update : _count : " << _count << ", frame No. : " << _currentFrame << " of " << _numberFrames << ", update rate : " << _updatePeriod;
  }

}

