
#include "Regolith/Test/TiledObject.h"
#include "Regolith/Handlers/ContextGroup.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  TiledObject::TiledObject() :
    PhysicalObject(),
    DrawableObject(),
    _texture(),
    _collision()
  {
  }


  TiledObject::TiledObject( const TiledObject& other ) :
    PhysicalObject( other ),
    DrawableObject( other ),
    _texture( other._texture ),
    _collision( other._collision )
  {
  }


  TiledObject::~TiledObject()
  {
  }


  void TiledObject::configure( Json::Value& json_data, ContextGroup& cg )
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


  void TiledObject::reset()
  {
    // Reset the parent object
    PhysicalObject::reset();
  }


  Texture& TiledObject::getTexture()
  {
    return _texture;
  }


  Collision& TiledObject::getCollision()
  {
    return _collision;
  }


  void TiledObject::onCollision( Contact& /*contact*/, CollidableObject* /*other_object*/)
  {
  }

}

