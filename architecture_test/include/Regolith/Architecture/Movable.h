
#ifndef REGOLITH_ARCHITECTURE_MOVABLE_H_
#define REGOLITH_ARCHITECTURE_MOVABLE_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/PhysicalObject.h"


namespace Regolith
{

  namespace Architecture
  {

    /*
     * Class defining the interface for objects who's position is update on every frame by calling the function:
     *  void step( timestep ) function.
     */
    class Movable : virtual public PhysicalObject
    {
      private:
        float _mass;
        float _inverseMass;
        Vector _forces;
        Vector _inputForce;
        Vector _velocity;

      protected:
        void setMass( float );
        void setVelocity( Vector v ) { _velocity = v; } 
        void setInputForce( Vector f ) { _inputForce = f; }

      public:
        Movable();

        virtual ~Movable() {}

        // Signifies that this object is movable
        virtual bool hasMovement() const override { return true; }


        // Performs the integration step
        void step( float );


        // Add a force to the object
        void addForce( Vector f ) { _forces += f; }


        // Return the current state information
        float getMass() const { return _mass; }
        Vector getVelocity() const { return _velocity; }
        Vector getForces() const { return _forces; }
        Vector getInputForce() const { return _inputForce; }
    };

  }
}

#endif // REGOLITH_ARCHITECTURE_MOVABLE_H_

