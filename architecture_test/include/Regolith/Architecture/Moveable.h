
#ifndef REGOLITH_ARCHITECTURE_MOVEABLE_H_
#define REGOLITH_ARCHITECTURE_MOVEABLE_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/PhysicalObject.h"


namespace Regolith
{

  /*
   * Class defining the interface for objects who's position is update on every frame by calling the function:
   *  void step( timestep ) function.
   */
  class Moveable : virtual public PhysicalObject
  {
    private:
      Vector _forces;
      Vector _inputForce;
      Vector _velocity;

    protected:
      void setVelocity( Vector v ) { _velocity = v; } 
      void setInputForce( Vector f ) { _inputForce = f; }

      Vector& velocity() { return _velocity; }
      Vector& inputForce() { return _inputForce; }

    public:
      Moveable();

      virtual ~Moveable() {}


      virtual void configure( Json::Value& ) override;


      // Signifies that this object is movable
      virtual bool hasMovement() const override { return true; }


      // Performs the integration step
      void step( float );


      // Add a force to the object
      void addForce( Vector f ) { _forces += f; }

      void addVelocity( Vector v ) { _velocity += v; }

      // Return the current state information
      Vector getVelocity() const { return _velocity; }
      Vector getForces() const { return _forces; }
      Vector getInputForce() const { return _inputForce; }
  };

}

#endif // REGOLITH_ARCHITECTURE_MOVEABLE_H_

