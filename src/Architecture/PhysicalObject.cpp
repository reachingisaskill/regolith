
#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Managers/ContextGroup.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Utilities/Contact.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{


////////////////////////////////////////////////////////////////////////////////////////////////////
//// State details class////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
  // Construction

  PhysicalObject::StateDetails::StateDetails() :
    _count( 0.0 ),
    _numberFrames( 0 ),
    id( 0 ),
    updatePeriod( 0.0 ),
    texture(),
    collision(),
    currentFrame( 0 )
  {
  }


  // Update the current frame number
  void PhysicalObject::StateDetails::update( float timestep )
  {
    // Update the timer
    _count += timestep;

    // Determine how many frames have elapsed in the animation
    currentFrame += _count / updatePeriod;
    currentFrame = currentFrame % _numberFrames;

    // Reset the timer based on the update period
    _count = std::fmod( _count, updatePeriod );

    // Update the texture and collision objects
    texture.setFrameNumber( currentFrame );
    collision.setFrameNumber( currentFrame );

    DEBUG_STREAM << "StateDetails::update : _count : " << _count << ", frame No. : " << currentFrame << " of " << _numberFrames << ", update rate : " << updatePeriod;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
//// Physical object class//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
  // Construction & destruction

  PhysicalObject::PhysicalObject() :
    _destroyMe( false ),
    _hasMoveable( false ),
    _hasTexture( false ),
    _hasAnimation( false ),
    _position(),
    _rotation( 0.0 ),
    _mass( 0.0 ),
    _inverseMass( 0.0 ),
    _elasticity( 0.0 ),
    _width( 0.0 ),
    _height( 0.0 ),
    _velocity(),
    _forces(),
    _collisionTeam( 0 ),
//    _children(),
    _stateMap(),
    _startState(),
    _startStatePointer( nullptr ),
    _currentState( nullptr )
  {
  }


  // Manually copy the children
  PhysicalObject::PhysicalObject( const PhysicalObject& other ) :
    _destroyMe( other._destroyMe ),
    _hasMoveable( other._hasMoveable ),
    _hasTexture( other._hasTexture ),
    _hasAnimation( other._hasAnimation ),
    _position( other._position ),
    _rotation( other._rotation ),
    _mass( other._mass ),
    _inverseMass( other._inverseMass ),
    _elasticity( other._elasticity ),
    _width( other._width ),
    _height( other._height ),
    _velocity( other._velocity ),
    _forces(),
    _collisionTeam( other._collisionTeam ),
//    _children(),
    _stateMap( other._stateMap ),
    _startState( other._startState ),
    _startStatePointer( &_stateMap[_startState] ),
    _currentState( _startStatePointer )
  {
//    for ( PhysicalObjectMap::const_iterator it = other._children.begin(); it != other._children.end(); ++it )
//    {
//      this->_children[ it->first ] = it->second->clone();
//    }
  }


  // Destroy the children
  PhysicalObject::~PhysicalObject()
  {
//    for ( PhysicalObjectMap::iterator it = _children.begin(); it != _children.end(); ++it )
//    {
//      delete it->second;
//    }
//    _children.clear();
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Configuration

  void PhysicalObject::configure( Json::Value& json_data, ContextGroup& cg )
  {
    Utilities::validateJson( json_data, "has_moveable", Utilities::JSON_TYPE_BOOLEAN );
    Utilities::validateJson( json_data, "has_texture", Utilities::JSON_TYPE_BOOLEAN );
    Utilities::validateJson( json_data, "has_animation", Utilities::JSON_TYPE_BOOLEAN );
    Utilities::validateJson( json_data, "states", Utilities::JSON_TYPE_OBJECT );

    // Configure the basic properties of the object
    _hasMoveable = json_data["has_moveable"].asBool();
    _hasTexture = json_data["has_texture"].asBool();
    _hasAnimation = json_data["has_animation"].asBool();


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

      INFO_STREAM << "PhysicalObject::configure : Configuring collidable object with type: " << collision_team;
    }

//    // If children are present, configure them too
//    if ( Utilities::validateJson( json_data, "children", Utilities::JSON_TYPE_OBJECT, false ) )
//    {
//      Json::Value& children = json_data["children"];
//
//      for( Json::Value::iterator c_it = children.begin(); c_it != children.end(); ++c_it )
//      {
//        std::string child_name = c_it.key().asString();
//        _children[ child_name ] = nullptr;
//
//
//        // Use the builder to create child objects
//      }
//    }


    // Controlling the states
    Json::Value& states = json_data["states"];
    if ( states.size() == 0 )
    {
      Exception ex( "PhysicalObject::configure()", "Every physical object must have at least one state" );
      throw ex;
    }

    for( Json::Value::iterator s_it = states.begin(); s_it != states.end(); ++s_it )
    {
      std::string state_name = s_it.key().asString();
      Json::Value& state_data = (*s_it);

      _stateMap.emplace( std::make_pair( state_name, StateDetails() ) );

      // Texture details
      if ( Utilities::validateJson( state_data, "texture", Utilities::JSON_TYPE_OBJECT, _hasTexture ) )
      {
        _stateMap[ state_name ].texture.configure( state_data["texture"], cg.getDataHandler() );
      }
      
      // Hitbox details
      if ( Utilities::validateJson( state_data, "collision", Utilities::JSON_TYPE_OBJECT, false ) )
      {
        _stateMap[ state_name ].collision.configure( state_data["collision"] );
      }

      // Animation details
      if ( Utilities::validateJson( state_data, "update_period", Utilities::JSON_TYPE_FLOAT, _hasAnimation ) )
      {
        _stateMap[ state_name ].updatePeriod = state_data[ "update_period" ].asFloat();
      }
    }

    // Store the reset state if given
    if ( Utilities::validateJson( json_data, "start_state", Utilities::JSON_TYPE_STRING, false ) )
    {
      std::string start_state_name = json_data["start_state"].asString();
      StateMap::iterator found = _stateMap.find( start_state_name );
      if ( found == _stateMap.end() )
      {
        Exception ex( "PhysicalObject::configure()", "Requested start state not found in object configuration." );
        ex.addDetail( "Requested State Name", start_state_name );
        throw ex;
      }

      _startState = start_state_name;
      _startStatePointer = &found->second;
    }
    else
    {
      _startState = _stateMap.begin()->first;
      _startStatePointer = &_stateMap.begin()->second;
    }

    _currentState = _startStatePointer;

    DEBUG_STREAM << "PhysicalObject::configure : Configured physical object. Pos = " << _position << " Vel = " << _velocity << " M = " << _mass << " w/h = " << _width << ", " << _height;
  }


  void PhysicalObject::reset()
  {
    _destroyMe = false;
    _velocity.zero();
    _forces.zero();
    _currentState = _startStatePointer;
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


  void PhysicalObject::update( float time )
  {
    _currentState->update( time );
  }


  void PhysicalObject::onCollision( Contact& contact, PhysicalObject* /*other_object*/)
  {
    DEBUG_STREAM << "PhysicalObject::onCollision : " << contact.overlap << " | " << contact.impulse;
    this->move( contact.inertiaRatio * contact.overlap );
    this->kick( contact.impulse );
    DEBUG_STREAM << "PhysicalObject::onCollision : Position : " << _position << ", Vel : " << _velocity;
  }

}

