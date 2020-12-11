
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

    // Hitbox details
    if ( Utilities::validateJson( json_data, "collision", Utilities::JSON_TYPE_OBJECT, false ) )
    {
      _collision.configure( json_data["collision"] );
    }

    // Configure the font
    _font.configure( json_data, cg.getDataHandler() );

    // Configure the text to display
    _text.configure( json_data, cg.getDataHandler() );
  }


  void TextObject::reset()
  {
    // Reset the parent object
    PhysicalObject::reset();
  }


  const Texture& TextObject::getTexture() const
  {
    return _texture;
  }


  const Collision& TextObject::getCollision() const
  {
    return _collision;
  }


  void TextObject::update( float )
  {
  }

}

