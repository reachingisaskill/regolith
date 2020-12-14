
#include "Regolith/CompositeObject.h"
#include "Regolith/Managers/ContextGroup.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{
  CompositeObject::CompositeObject() :
    PhysicalObject(),
    _children()
  {
    Exception ex( "CompositeObject::CompositeObject()", "Composite objects not yet available." );
    throw ex;
  }


  // Manually copy the children
  CompositeObject::CompositeObject( const CompositeObject& other ) :
    PhysicalObject( other ),
    _children()
  {
    for ( PhysicalObjectMap::const_iterator it = other._children.begin(); it != other._children.end(); ++it )
    {
      this->_children[ it->first ] = it->second->clone();
    }
  }


  // Destroy the children
  CompositeObject::~CompositeObject()
  {
    for ( PhysicalObjectMap::iterator it = _children.begin(); it != _children.end(); ++it )
    {
      delete it->second;
    }
    _children.clear();
  }


  void CompositeObject::configure( Json::Value& json_data, ContextGroup& cg )
  {
    // Configure the parent class first
    PhysicalObject::configure( json_data, cg);

    // If children are present, configure them too
    if ( Utilities::validateJson( json_data, "children", Utilities::JSON_TYPE_OBJECT, false ) )
    {
      Json::Value& children = json_data["children"];

      for( Json::Value::iterator c_it = children.begin(); c_it != children.end(); ++c_it )
      {
        std::string child_name = c_it.key().asString();
        _children[ child_name ] = nullptr;


        // Use the builder to create child objects
      }
    }

  }


  void CompositeObject::reset()
  {
    PhysicalObject::reset();
  }


  const PhysicalObjectVector& CompositeObject::getChildren() const
  {
  }


  Texture& CompositeObject::getTexture()
  {
  }


  Collision& CompositeObject::getCollision()
  {
  }

}

