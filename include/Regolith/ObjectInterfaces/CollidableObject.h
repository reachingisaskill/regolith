
#ifndef REGOLITH_OBJECT_INTERFACES_COLLIDABLE_OBJECT_H_
#define REGOLITH_OBJECT_INTERFACES_COLLIDABLE_OBJECT_H_

#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/Collisions/Collision.h"


namespace Regolith
{

  // Struct holding all the info required for resolving a collision.
  struct Contact
  {
    // A pointer to the other contact, seen from the colliding object's perspective
    Contact* other;

    // Ratio of the inverse masses.
    float massRatio;
    // Impulse resulting from the collision
    Vector impulse;
    // Normal vector to the collision. Use to calculate impulse
    Vector normal;


    // Ratio of the inverse inertias
    float inertiaRatio;
    // Angular impulse resulting from the collision
    float angularImpulse;
    // Interaction point. Used to calculate angular impulse
    Vector point;


    // Size of the overlap between the hitboxes
    float overlap;

    // Type of the object collided with
    CollisionType type;
  };



  class CollidableObject : virtual public PhysicalObject
  {
    private:

    public:
      CollidableObject() {}
      virtual ~CollidableObject() {}


      // Signals that derived classes have collision
      virtual bool hasCollision() const override { return true; }


      // For the collision handler to request the current hitboxes. Derived class must define the collision object!
      virtual const Collision& getCollision() = 0;

      // Call back function for when this object collides with another
      virtual void onCollision( Contact&, CollidableObject* ) = 0;

  };

}


#endif // REGOLITH_OBJECT_INTERFACES_COLLIDABLE_OBJECT_H_

