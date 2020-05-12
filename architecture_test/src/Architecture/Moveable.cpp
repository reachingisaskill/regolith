
#include "Regolith/Architecture/Moveable.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  Moveable::Moveable() :
    PhysicalObject(),
    _mass( 0.0 ),
    _inverseMass( 0.0 ),
    _forces( 0.0 ),
    _inputForce( 0.0 ),
    _velocity( 0.0 )
  {
  }


  void Moveable::step( float timestep )
  {
    // Starting with Euler Step algorithm.
    // Might move to leap-frog/Runge-Kutta later
    
    Vector accel = _inverseMass * ( _forces + _inputForce );

    _velocity += accel * timestep;

    move( _velocity * timestep );

    DEBUG_STREAM << "Position : " << position();
    // Update complete - reset forces
    _forces = _mass*Manager::getInstance()->getGravity() - _velocity*Manager::getInstance()->getDrag();
  }


  void Moveable::setMass( float m )
  {
    _mass = m;
    if ( _mass < epsilon )
      _inverseMass = 0.0;
    else
      _inverseMass = 1.0/_mass;
  }


  void Moveable::configure( Json::Value& json_data )
  {
    Utilities::validateJson( json_data, "mass", Utilities::JSON_TYPE_FLOAT );
    setMass( json_data["mass"].asFloat() );

    if ( Utilities::validateJson( json_data, "velocity", Utilities::JSON_TYPE_FLOAT, false ) )
    {
      Utilities::validateJsonArray( json_data["velocity"], 2, Utilities::JSON_TYPE_FLOAT );
      float vx = json_data["velocity"][0].asFloat();
      float vy = json_data["velocity"][1].asFloat();
      setVelocity( Vector( vx, vy ) );
    }
  }

}

