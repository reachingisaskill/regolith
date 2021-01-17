
#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Handlers/ContextGroup.h"
#include "Regolith/Handlers/DataHandler.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  PhysicalObject::PhysicalObject() :
    _destroyMe( false ),
    _hasMoveable( false ),
    _hasPhysics( false ),
    _position(),
    _rotation( 0.0 ),
    _flipFlag( SDL_FLIP_NONE ),
    _center(),
    _centerPoint( {0, 0} ),
    _mass( 0.0 ),
    _inverseMass( 0.0 ),
    _inertiaDensity( 1.0 ),
    _inverseInertiaDensity( 1.0 ),
    _elasticity( 0.0 ),
    _width( 0.0 ),
    _height( 0.0 ),
    _velocity(),
    _forces(),
    _angularVel( 0.0 ),
    _torques( 0.0 ),
    _collisionTeam( 0 )
  {
  }


  // Manually copy the children
  PhysicalObject::PhysicalObject( const PhysicalObject& other ) :
    _destroyMe( other._destroyMe ),
    _hasMoveable( other._hasMoveable ),
    _hasPhysics( other._hasPhysics ),
    _position( other._position ),
    _rotation( other._rotation ),
    _flipFlag( other._flipFlag ),
    _center( other._center ),
    _centerPoint( other._centerPoint ),
    _mass( other._mass ),
    _inverseMass( other._inverseMass ),
    _inertiaDensity( other._inertiaDensity ),
    _inverseInertiaDensity( other._inverseInertiaDensity ),
    _elasticity( other._elasticity ),
    _width( other._width ),
    _height( other._height ),
    _velocity( other._velocity ),
    _forces(),
    _angularVel( other._angularVel ),
    _torques( 0.0 ),
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
    validateJson( json_data, "has_moveable", JsonType::BOOLEAN );
    validateJson( json_data, "has_physics", JsonType::BOOLEAN );

    // Configure the flags of physical object
    _hasMoveable = json_data["has_moveable"].asBool();
    _hasPhysics = json_data["has_physics"].asBool();


    // Set the mass properties - Determine if an object can even be moved
    if ( validateJson( json_data, "mass", JsonType::FLOAT, false ) )
    {
      setMass( json_data["mass"].asFloat() );
    }

    // Set the angular inertia properties - Determine if an object can even be rotated. Actually an inertial density, weighted by the mass
    if ( validateJson( json_data, "inertia", JsonType::FLOAT, false ) )
    {
      setInertiaDensity( json_data["inertia"].asFloat() );
    }

    // Set the coefficient of elasticity of the object. 1 = perfectly elastic.
    if ( validateJson( json_data, "elasticity", JsonType::FLOAT, false ) )
    {
      _elasticity = json_data["elasticity"].asFloat();
    }

    // Set the starting position (defaults to zero, zero)
    if ( validateJson( json_data, "position", JsonType::ARRAY, false ) )
    {
      validateJsonArray( json_data["position"], 2, JsonType::FLOAT );

      float x =  json_data["position"][0].asFloat();
      float y =  json_data["position"][1].asFloat();
      _position = Vector( x, y );
    }

    // Set the starting rotation (defaults to zero)
    if ( validateJson( json_data, "rotation", JsonType::FLOAT, false ) )
    {
      _rotation =  json_data["rotation"].asFloat();
    }

    // Set the center point for the object, w.r.t the bounding box
    if ( validateJson( json_data, "center", JsonType::ARRAY, false ) )
    {
      validateJsonArray( json_data["center"], 2, JsonType::FLOAT );

      this->setCenter( Vector( json_data["center"][0].asFloat(), json_data["center"][1].asFloat() ) );
    }

    // Set the starting flip state
    if ( validateJson( json_data, "flip_horizontal", JsonType::BOOLEAN, false ) )
    {
      if ( json_data["flip_horizontal"].asBool() )
        _flipFlag = (SDL_RendererFlip) (_flipFlag ^ SDL_FLIP_HORIZONTAL);
    }
    if ( validateJson( json_data, "flip_vertical", JsonType::BOOLEAN, false ) )
    {
      if ( json_data["flip_vertical"].asBool() )
        _flipFlag = (SDL_RendererFlip) (_flipFlag ^ SDL_FLIP_VERTICAL);
    }

    // Set the initial velocity
    if ( validateJson( json_data, "velocity", JsonType::ARRAY, false ) )
    {
      validateJsonArray( json_data["velocity"], 2, JsonType::FLOAT );
      float vx = json_data["velocity"][0].asFloat();
      float vy = json_data["velocity"][1].asFloat();
      setVelocity( Vector( vx, vy ) );
    }

    // Set the initial angular velocity
    if ( validateJson( json_data, "angular_velocity", JsonType::FLOAT, false ) )
    {
      setAngularVelocity( json_data["angular_velocity"].asFloat() );
    }

    // Set the collision properties
    if ( validateJson( json_data, "bounding_box", JsonType::OBJECT, false ) )
    {
      Json::Value& bounding_box_data = json_data["bounding_box"];

      validateJson( bounding_box_data, "width", JsonType::FLOAT );
      validateJson( bounding_box_data, "height", JsonType::FLOAT );
      validateJson( bounding_box_data, "collision_team", JsonType::STRING );

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
    _angularVel = 0.0;
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


  void PhysicalObject::setInertiaDensity( float i )
  {
    _inertiaDensity = i;
    if ( _inertiaDensity < epsilon )
      _inverseInertiaDensity = 0.0;
    else
      _inverseInertiaDensity = 1.0/_inertiaDensity;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Functions that enable physics


  void PhysicalObject::step( float time )
  {
    // Starting with Euler Step algorithm.
    // Might move to leap-frog/Runge-Kutta later
    Vector accel = _inverseMass * _forces;
    float r_accel = _inverseInertiaDensity * _torques;

    _velocity += ( accel * time );
    _position += ( _velocity * time );

    _angularVel += ( r_accel * time );
    _rotation += ( _angularVel * time );

    // Update complete - reset forces
    _forces.zero();
    _torques = 0.0;
//    DEBUG_STREAM << "PhysicalObject::step : Position : " << _position << ", Vel : " << _velocity << ", Accel : " << accel << ", InvM : " << _inverseMass << ", Delta T : " << time;
  }

}

