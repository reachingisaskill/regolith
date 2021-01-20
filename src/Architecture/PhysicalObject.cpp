
#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Handlers/ContextGroup.h"
#include "Regolith/Handlers/DataHandler.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  PhysicalObject::PhysicalObject() :
    _destroyMe( false ),
    _hasTranslatable( false ),
    _hasRotatable( false ),
    _hasPhysics( false ),
    _position(),
    _rotation( 0.0 ),
    _flipFlag( SDL_FLIP_NONE ),
    _center(),
    _centerPoint( {0, 0} ),
    _mass( 0.0 ),
    _inverseMass( 0.0 ),
    _inertiaDensity( 100.0 ),
    _inverseInertiaDensity( 0.01 ),
    _elasticity( 0.0 ),
    _boundingBox(),
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
    _hasTranslatable( other._hasTranslatable ),
    _hasRotatable( other._hasRotatable ),
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
    _boundingBox( other._boundingBox ),
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
    validateJson( json_data, "has_translatable", JsonType::BOOLEAN );
    validateJson( json_data, "has_rotatable", JsonType::BOOLEAN );
    validateJson( json_data, "has_physics", JsonType::BOOLEAN );

    // Configure the flags of physical object
    _hasTranslatable = json_data["has_translatable"].asBool();
    _hasRotatable = json_data["has_rotatable"].asBool();
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
      _rotation =  json_data["rotation"].asFloat() * degrees_to_radians;
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
      setAngularVelocity( json_data["angular_velocity"].asFloat() * degrees_to_radians );
    }

    // Set the collision properties
    if ( validateJson( json_data, "bounding_box", JsonType::OBJECT, false ) )
    {
      Json::Value& bounding_box_data = json_data["bounding_box"];
      Vector position = zeroVector;

      if ( validateJson( bounding_box_data, "position", JsonType::ARRAY, false ) )
      {
        validateJsonArray( json_data["position"], 2, JsonType::FLOAT );
        position.set( bounding_box_data["position"][0].asFloat(), bounding_box_data["position"][1].asFloat() );
      }

      validateJson( bounding_box_data, "width", JsonType::FLOAT );
      validateJson( bounding_box_data, "height", JsonType::FLOAT );
      validateJson( bounding_box_data, "collision_team", JsonType::STRING );

      std::string collision_team = bounding_box_data["collision_team"].asString();
      _collisionTeam = Manager::getInstance()->getCollisionTeam( collision_team );

      float width = bounding_box_data["width"].asFloat();
      float height = bounding_box_data["height"].asFloat();

      _boundingBox.configure( position, width, height );

      INFO_STREAM << "PhysicalObject::configure : Configuring physical object with collision team: " << collision_team;
    }
    else
    {
      _boundingBox.configure( zeroVector, 0.0, 0.0 );
    }

    DEBUG_STREAM << "PhysicalObject::configure : Configured physical object. Pos = " << _position << " Vel = " << _velocity << " M = " << _mass << " w/h = " << _boundingBox.width << ", " << _boundingBox.height;
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


  void PhysicalObject::setWidth( float w )
  {
    _boundingBox.width = w;
    // Have to update the points
    _boundingBox.points[1].set( _boundingBox.points[0].x() + w, _boundingBox.points[0].y() );
    _boundingBox.points[2].set( _boundingBox.points[3].x() + w, _boundingBox.points[3].y() );
  }


  void PhysicalObject::setHeight( float h )
  {
    _boundingBox.height = h;
    // Have to update the points
    _boundingBox.points[2].set( _boundingBox.points[1].x(), _boundingBox.points[1].y() + h );
    _boundingBox.points[2].set( _boundingBox.points[0].x(), _boundingBox.points[0].y() + h );
  }


  void PhysicalObject::setCenter( Vector c )
  {
    _center = c;
    // Rounding for the float -> int conversion
    _centerPoint.x = c.x()+0.5;
    _centerPoint.y = c.y()+0.5;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Functions that enable physics


  void PhysicalObject::step( float time )
  {
    // Starting with Euler Step algorithm.
    // Might move to leap-frog/Runge-Kutta later

    if ( _hasTranslatable )
    {
      Vector accel = _inverseMass * _forces;

      // Euler Step Integration
      _velocity += ( accel * time );
      _position += ( _velocity * time );

      _forces.zero();
    }

    if ( _hasRotatable )
    {
      float r_accel = _inverseInertiaDensity * _torques;

      // Euler Step Integration
      _angularVel += ( r_accel * time );
      _rotation += ( _angularVel * time );

      _torques = 0.0;
    }
  }

  /*
  // Copy of the Runge Kutta Order 4 integration algorith for a future me...

  dy/dt = f( y, t )

  k1 = f( y0, t0 )
  k2 = f( y0+h*(k1/2), t0+h/2 )
  k3 = f( y0+h*(k2/2), t0+h/2 )
  k4 = f( y0+h*k3, t0+h )

  y1 = y0 + 1/6 * h * ( k1 + 2*k2 + 2*k3 + k4 )
  */

}

