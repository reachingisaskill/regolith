
#include "Regolith/Test/ContainerObject.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  ContainerObject::ContainerObject() :
    PhysicalObject(),
    _collision()
  {
  }


  ContainerObject::ContainerObject( const ContainerObject& other ) :
    PhysicalObject( other ),
    _collision( other._collision )
  {
  }


  ContainerObject::~ContainerObject()
  {
  }


  void ContainerObject::configure( Json::Value& json_data, ContextGroup& cg )
  {
    // Configure the parent class first
    PhysicalObject::configure( json_data, cg);

    DEBUG_LOG( "ContainerObject::configure : Configuring ContainerObject" );
    
    // Hitbox details
    if ( Utilities::validateJson( json_data, "collision", Utilities::JSON_TYPE_OBJECT ) )
    {
      _collision.configure( json_data["collision"] );
    }
  }


  void ContainerObject::reset()
  {
    // Reset the parent object
    PhysicalObject::reset();
  }


  Collision& ContainerObject::getCollision()
  {
    return _collision;
  }

  void ContainerObject::onCollision( Contact&, CollidableObject* )
  {
  }

}

