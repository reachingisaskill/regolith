
#include "Regolith/GamePlay/PhysicalObject.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Managers/ContextGroup.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Utilities/Contact.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  PhysicalObject::PhysicalObject() :
    _destroyMe( false ),
    _hasMoveable( false ),
    _hasTexture( false ),
    _hasAnimation( false ),
    _hasPhysics( false ),
    _position(),
    _rotation( 0.0 ),
    _flipFlag( SDL_FLIP_NONE ),
    _mass( 0.0 ),
    _inverseMass( 0.0 ),
    _elasticity( 0.0 ),
    _width( 0.0 ),
    _height( 0.0 ),
    _velocity(),
    _forces(),
    _collisionTeam( 0 )
  {
  }


  // Manually copy the children
  PhysicalObject::PhysicalObject( const PhysicalObject& other ) :
    _destroyMe( other._destroyMe ),
    _hasMoveable( other._hasMoveable ),
    _hasTexture( other._hasTexture ),
    _hasAnimation( other._hasAnimation ),
    _hasPhysics( other._hasPhysics ),
    _position( other._position ),
    _rotation( other._rotation ),
    _flipFlag( other._flipFlag ),
    _mass( other._mass ),
    _inverseMass( other._inverseMass ),
    _elasticity( other._elasticity ),
    _width( other._width ),
    _height( other._height ),
    _velocity( other._velocity ),
    _forces(),
    _collisionTeam( other._collisionTeam )
  {
  }


  // Destroy the children
  PhysicalObject::~PhysicalObject()
  {
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Configuration

  void PhysicalObject::configure( Json::Value& json_data, ContextGroup& /*cg*/ )
  {
    Utilities::validateJson( json_data, "has_moveable", Utilities::JSON_TYPE_BOOLEAN );
    Utilities::validateJson( json_data, "has_texture", Utilities::JSON_TYPE_BOOLEAN );
    Utilities::validateJson( json_data, "has_animation", Utilities::JSON_TYPE_BOOLEAN );
    Utilities::validateJson( json_data, "has_physics", Utilities::JSON_TYPE_BOOLEAN );

    // Configure the basic properties of the object
    _hasMoveable = json_data["has_moveable"].asBool();
    _hasTexture = json_data["has_texture"].asBool();
    _hasAnimation = json_data["has_animation"].asBool();
    _hasPhysics = json_data["has_physics"].asBool();


    // Set the mass properties - Determine if an object can even be moved
    if ( Utilities::validateJson( json_data, "mass", Utilities::JSON_TYPE_FLOAT, false ) )
    {
      setMass( json_data["mass"].asFloat() );
    }

    // Set the coefficient of elasticity of the object. 1 = perfectly elastic.
    if ( Utilities::validateJson( json_data, "elasticity", Utilities::JSON_TYPE_FLOAT, false ) )
    {
      _elasticity = json_data["elasticity"].asFloat();
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
      WARN_LOG( "PhysicalObject::configure() : Rotations are not currently fully supported for collision detection." );
    }

    // Set the starting flip state
    if ( Utilities::validateJson( json_data, "flip_horizontal", Utilities::JSON_TYPE_BOOLEAN, false ) )
    {
      if ( json_data["flip_horizontal"].asBool() )
        _flipFlag = (SDL_RendererFlip) (_flipFlag ^ SDL_FLIP_HORIZONTAL);
    }
    if ( Utilities::validateJson( json_data, "flip_vertical", Utilities::JSON_TYPE_BOOLEAN, false ) )
    {
      if ( json_data["flip_vertical"].asBool() )
        _flipFlag = (SDL_RendererFlip) (_flipFlag ^ SDL_FLIP_VERTICAL);
    }

    // Set the initial velocity
    if ( Utilities::validateJson( json_data, "velocity", Utilities::JSON_TYPE_ARRAY, false ) )
    {
      Utilities::validateJsonArray( json_data["velocity"], 2, Utilities::JSON_TYPE_FLOAT );
      float vx = json_data["velocity"][0].asFloat();
      float vy = json_data["velocity"][1].asFloat();
      setVelocity( Vector( vx, vy ) );
    }

    // Set the collision properties
    if ( Utilities::validateJson( json_data, "bounding_box", Utilities::JSON_TYPE_OBJECT, false ) )
    {
      Json::Value& bounding_box_data = json_data["bounding_box"];

      Utilities::validateJson( bounding_box_data, "width", Utilities::JSON_TYPE_FLOAT );
      Utilities::validateJson( bounding_box_data, "height", Utilities::JSON_TYPE_FLOAT );
      Utilities::validateJson( bounding_box_data, "collision_team", Utilities::JSON_TYPE_STRING );

      _width = bounding_box_data["width"].asFloat();
      _height = bounding_box_data["height"].asFloat();

      std::string collision_team = bounding_box_data["collision_team"].asString();
      _collisionTeam = Manager::getInstance()->getCollisionTeam( collision_team );

      INFO_STREAM << "PhysicalObject::configure : Configuring physical object with collision team: " << collision_team;
    }

    DEBUG_STREAM << "PhysicalObject::configure : Configured physical object. Pos = " << _position << " Vel = " << _velocity << " M = " << _mass << " w/h = " << _width << ", " << _height;
  }


  void PhysicalObject::reset()
  {
    _destroyMe = false;
    _velocity.zero();
    _forces.zero();
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


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Functions that enable physics


  void PhysicalObject::step( float time )
  {
    // Starting with Euler Step algorithm.
    // Might move to leap-frog/Runge-Kutta later
    Vector accel = _inverseMass * _forces;

    _velocity += ( accel * time );
    _position += ( _velocity * time );

    // Update complete - reset forces
    _forces.zero();
    DEBUG_STREAM << "PhysicalObject::step : Position : " << _position << ", Vel : " << _velocity << ", Accel : " << accel << ", InvM : " << _inverseMass << ", Delta T : " << time;
  }


  void PhysicalObject::onCollision( Contact& contact, PhysicalObject* /*other_object*/)
  {
    DEBUG_STREAM << "PhysicalObject::onCollision : " << contact.overlap << " | " << contact.impulse;
    this->move( contact.inertiaRatio * contact.overlap );
    this->kick( contact.impulse );
    DEBUG_STREAM << "PhysicalObject::onCollision : Position : " << _position << ", Vel : " << _velocity;
  }

}

