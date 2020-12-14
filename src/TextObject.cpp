
#include "Regolith/TextObject.h"
#include "Regolith/Managers/ContextGroup.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  TextObject::TextObject() :
    PhysicalObject(),
    _texture(),
    _collision()
  {
  }


  TextObject::TextObject( const TextObject& other ) :
    PhysicalObject( other ),
    _texture( other._texture ),
    _collision( other._collision )
  {
  }


  TextObject::~TextObject()
  {
  }


  void TextObject::configure( Json::Value& json_data, ContextGroup& cg )
  {
    // Configure the parent class first
    PhysicalObject::configure( json_data, cg);

    // Configure the texture
    _texture.configure( json_data, cg.getDataHandler() );

    // Hitbox details
    if ( Utilities::validateJson( json_data, "collision", Utilities::JSON_TYPE_OBJECT, false ) )
    {
      _collision.configure( json_data["collision"] );
    }

  }


  void TextObject::reset()
  {
    // Reset the parent object
    PhysicalObject::reset();
  }


  Texture& TextObject::getTexture()
  {
    return _texture;
  }


  Collision& TextObject::getCollision()
  {
    return _collision;
  }


  void TextObject::update( float )
  {
  }

}

