
#include "Regolith/Architecture/Moveable.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Managers/ContextGroup.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  Moveable::Moveable() :
    PhysicalObject(),
    _forces( 0.0 ),
    _inputForce( 0.0 ),
    _velocity( 0.0 )
  {
  }


  void Moveable::step( float timestep )
  {
    // Starting with Euler Step algorithm.
    // Might move to leap-frog/Runge-Kutta later
    Vector accel = getInverseMass() * ( _forces + _inputForce );

    DEBUG_STREAM << "Position : " << position() << ", Vel : " << _velocity << ", Accel : " << accel << ", InvM : " << getInverseMass() << ", Delta T : " << timestep;

    _velocity += accel * timestep;

    move( _velocity * timestep );

    // Update complete - reset forces
    _forces = getMass()*Manager::getInstance()->getGravity() - _velocity*Manager::getInstance()->getDrag();
  }


  void Moveable::configure( Json::Value& json_data, ContextGroup& cg, DataHandler& handler )
  {
    PhysicalObject::configure( json_data, cg, handler );

    if ( Utilities::validateJson( json_data, "velocity", Utilities::JSON_TYPE_FLOAT, false ) )
    {
      Utilities::validateJsonArray( json_data["velocity"], 2, Utilities::JSON_TYPE_FLOAT );
      float vx = json_data["velocity"][0].asFloat();
      float vy = json_data["velocity"][1].asFloat();
      setVelocity( Vector( vx, vy ) );
    }
  }

}

