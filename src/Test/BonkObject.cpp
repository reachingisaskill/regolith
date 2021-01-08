
#include "Regolith/Test/BonkObject.h"
#include "Regolith/Handlers/ContextGroup.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  BonkObject::BonkObject() :
    PhysicalObject(),
    DrawableObject(),
    _texture(),
    _collision()
  {
  }


  BonkObject::BonkObject( const BonkObject& other ) :
    PhysicalObject( other ),
    DrawableObject( other ),
    _texture( other._texture ),
    _collision( other._collision ),
    _bonk( other._bonk )
  {
  }


  BonkObject::~BonkObject()
  {
  }


  void BonkObject::configure( Json::Value& json_data, ContextGroup& cg )
  {
    validateJson( json_data, "collision", JsonType::OBJECT );
    validateJson( json_data, "texture", JsonType::OBJECT );
    validateJson( json_data, "bonk", JsonType::OBJECT );

    // Configure the parent class first
    PhysicalObject::configure( json_data, cg);

    // Texture details
    _texture.configure( json_data["texture"], cg.getDataHandler() );
    
    // Hitbox details
    _collision.configure( json_data["collision"] );

    _bonk.configure( json_data["bonk"], cg.getDataHandler() );
  }

  void BonkObject::registerSoundEffects( AudioHandler* handler )
  {
    handler->requestChannel( _bonk );
  }


  void BonkObject::reset()
  {
    // Reset the parent object
    PhysicalObject::reset();
  }


  Texture& BonkObject::getTexture()
  {
    return _texture;
  }


  Collision& BonkObject::getCollision()
  {
    return _collision;
  }


  void BonkObject::onCollision( Contact& contact, CollidableObject* /*other_object*/)
  {
    DEBUG_STREAM << "PhysicalObject::onCollision : " << contact.overlap << " | " << contact.impulse;
    this->move( contact.inertiaRatio * contact.overlap );
    this->kick( contact.impulse );
    DEBUG_STREAM << "PhysicalObject::onCollision : Position : " << this->getPosition() << ", Vel : " << this->getVelocity();

    this->playSound( _bonk );
  }

}

