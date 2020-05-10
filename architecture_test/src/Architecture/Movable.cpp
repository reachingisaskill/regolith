
#include "Architecture/Movable.h"

namespace Regolith
{
  namespace Architecture
  {

    Movable::Movable() :
      PhysicalObject(),
      _mass( 0.0 ),
      _inverseMass( 0.0 ),
      _inputForce( 0.0 ),
      _forces( 0.0 ),
      _inputForce( 0.0 ),
      _velocity( 0.0 )
    {
    }


    void Movable::step( Uint32 timestep )
    {
      // Starting with Euler Step algorithm.
      // Might move to leap-frog/Runge-Kutta later
      
      Vector accel = _inverseMass * ( _force + _inputForce );

      _velocity += accel * timestep;

      _position += _velocity * timestep;

      DEBUG_STREAM << "Position : " << _position;
      // Update complete - reset forces
      _forces = _mass*Manager::getInstance()->getGravity() - _velocity*Manager::getInstance()->getDrag();
    }


    void Movable::setMass( float m )
    {
      _mass = m;
      if ( _mass < epsilon )
        _inverseMass = 0.0;
      else
        _inverseMass = 1.0/_mass;
    }

  }
}

