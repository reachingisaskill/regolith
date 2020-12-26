
#include "Regolith/Test/SimpleObject.h"
#include "Regolith/Managers/ContextGroup.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  SimpleObject::SimpleObject() :
    PhysicalObject(),
    DrawableObject(),
    _texture(),
    _collision()
  {
  }


  SimpleObject::SimpleObject( const SimpleObject& other ) :
    PhysicalObject( other ),
    DrawableObject( other ),
    _texture( other._texture ),
    _collision( other._collision )
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
    if ( validateJson( json_data, "texture", JsonType::OBJECT ) )
    {
      _texture.configure( json_data["texture"], cg.getDataHandler() );
    }
    
    // Hitbox details
    if ( validateJson( json_data, "collision", JsonType::OBJECT ) )
    {
      _collision.configure( json_data["collision"] );
    }
  }


  void SimpleObject::reset()
  {
    // Reset the parent object
    PhysicalObject::reset();
  }


  Texture& SimpleObject::getTexture()
  {
    return _texture;
  }


  Collision& SimpleObject::getCollision()
  {
    return _collision;
  }


  void SimpleObject::onCollision( Contact& contact, CollidableObject* /*other_object*/)
  {
    DEBUG_STREAM << "PhysicalObject::onCollision : " << contact.overlap << " | " << contact.impulse;
    this->move( contact.inertiaRatio * contact.overlap );
    this->kick( contact.impulse );
    DEBUG_STREAM << "PhysicalObject::onCollision : Position : " << this->getPosition() << ", Vel : " << this->getVelocity();
  }

}

