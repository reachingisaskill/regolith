
#include "Drawable.h"

#include "Manager.h"

#include "logtastic.h"

#include <utility>


namespace Regolith
{

  Drawable::Drawable() :
    _theRenderer( nullptr ),
    _position( 0.0 ),
    _velocity( 0.0 ),
    _force( 0.0 ),
    _inputForce( 0.0 ),
    _rotation( 0.0 ),
    _team( 0 ),
    _width( 0 ),
    _height( 0 ),
    _mass( 0.0 ),
    _inverseMass( 0.0 ),
    _collisionActive( true )
  {
  }


  Drawable::Drawable( int width, int height, float rot, float mass ) :
    _theRenderer( nullptr ),
    _position( 0.0 ),
    _velocity( 0.0 ),
    _force( 0.0 ),
    _inputForce( 0.0 ),
    _rotation( rot ),
    _team( 0 ),
    _width( width ),
    _height( height ),
    _mass( mass ),
    _inverseMass( (_mass < epsilon) ? 0.0 : (1.0/_mass) ),
    _collisionActive( true )
  {
  }


  Drawable::~Drawable()
  {
  }


  Drawable* Drawable::cloneAt( Vector pos ) const
  {
    Drawable* temp = this->clone();
    temp->setPosition( pos );
    return temp;
  }


  void Drawable::setMass( float m )
  {
    _mass = m;
    if ( _mass < epsilon )
      _inverseMass = 0.0;
    else
      _inverseMass = 1.0/_mass;
  }


  void Drawable::step( float timestep )
  {
    static Manager* man = Manager::getInstance();
    // Starting with Euler Step algorithm.
    // Might move to leap-frog/Runge-Kutta later
    
    Vector accel = _inverseMass * (_force + _inputForce);

    _velocity += accel * timestep;

    _position += _velocity * timestep;

    DEBUG_STREAM << "Position : " << _position;
    // Update complete - reset forces
    _force = _inputForce + man->getGravity() - _velocity*man->getDrag();
  }


}

