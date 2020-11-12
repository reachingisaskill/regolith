
#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/Managers/ContextGroup.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{
////////////////////////////////////////////////////////////////////////////////////////////////////
  // Construction & destruction

  PhysicalObject::PhysicalObject() :
    _destroyMe( false ),
    _position(),
    _rotation( 0.0 ),
    _mass( 0.0 ),
    _inverseMass( 0.0 ),
    _width( 0.0 ),
    _height( 0.0 ),
    _collisionTeam( 0 ),
    _collisionType( 0 ),
    _children()
  {
  }


  // Manually copy the children
  PhysicalObject::PhysicalObject( const PhysicalObject& other ) :
    _destroyMe( other._destroyMe ),
    _position( other._position ),
    _rotation( other._rotation ),
    _mass( other._mass ),
    _inverseMass( other._inverseMass ),
    _width( other._width ),
    _height( other._height ),
    _collisionTeam( other._collisionTeam ),
    _collisionType( other._collisionType ),
    _children()
  {
    for ( PhysicalObjectMap::iterator it = other._children.begin(); it != other._children.end(); ++it )
    {
      this->_children[ it->first ] = it->second.clone();
    }
  }


  // Destroy the children
  PhysicalObject::~PhysicalObject()
  {
    for ( PhysicalObjectMap::iterator it = _children.begin(); it != _children.end(); ++it )
    {
      delete it->second;
    }
    _children.clear();
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Configuration

  void PhysicalObject::configure( Json::Value& json_data, ContextGroup& cg, DataHandler& handler )
  {
    // Set the mass properties - Determine if an object can even be moved
    if ( Utilities::validateJson( json_data, "mass", Utilities::JSON_TYPE_FLOAT, false ) )
    {
      setMass( json_data["mass"].asFloat() );
    }

    // Set the starting position (defaults to zero, zero)
    if ( Utilities::validateJson( json_data, "position", Utilities::JSON_TYPE_ARRAY, false ) )
    {
      Utilities::validateJsonArray( json_data["position"], 2, Utilities::JSON_TYPE_FLOAT );

      float x =  json_data["position"][0].asFloat();
      float y =  json_data["position"][1].asFloat();
      _position = Vector( x, y );
    }

    // Set the starting rotation (defaults to zero)
    if ( Utilities::validateJson( json_data, "rotation", Utilities::JSON_TYPE_FLOAT, false ) )
    {
      _rotation =  json_data["rotation"].asFloat();
    }

    // Set the collision properties
    if ( Utilities::validateJson( json_data, "collision", Utilities::JSON_TYPE_OBJECT, false ) )
    {
      Json::Value& collision_data = json_data["collision"];

      Utilities::validateJson( collision_data, "width", Utilities::JSON_TYPE_FLOAT );
      Utilities::validateJson( collision_data, "height", Utilities::JSON_TYPE_FLOAT );
      Utilities::validateJson( collision_data, "collision_team", Utilities::JSON_TYPE_STRING );
      Utilities::validateJson( collision_data, "collision_type", Utilities::JSON_TYPE_STRING );

      _width = collision_data["position"][0].asFloat();
      _height = collision_data["position"][1].asFloat();

      std::string collision_team = json_data["collision_team"].asString();
      std::string collision_type = json_data["collision_type"].asString();

      INFO_STREAM << "Configuring collidable object with type: " << collision_type << " and team: " << collision_team;

      _collisionTeam = Manager::getInstance()->getCollisionTeam( collision_team );
      _collisionType = Manager::getInstance()->getCollisionType( collision_type );
    }

    // If children are present, configure them too
    if ( Utilities::validateJson( json_data, "children", Utilities::JSON_TYPE_OBJECT, false ) )
    {
      Json::Value& children = json_data["children"];

      for( Json::Value::iterator c_it = children.begin(); c_it != children.end(); ++c_it )
      {
        std::string child_name = c_it.key().asString();
        _children.set( cont_name, nullptr );
      }
    }

  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Accessors and modifiers

  void PhysicalObject::setMass( float m )
  {
    _mass = m;
    if ( _mass < epsilon )
      _inverseMass = 0.0;
    else
      _inverseMass = 1.0/_mass;
  }


  void PhysicalObject::setCollision( CollisionTeam team, CollisionType type )
  {
    _collisionTeam = team;
    _collisionType = type;
  }

}

