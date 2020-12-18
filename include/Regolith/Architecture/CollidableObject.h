
#ifndef REGOLITH_ARCHITECTURE_COLLIDABLE_OBJECT_H_
#define REGOLITH_ARCHITECTURE_COLLIDABLE_OBJECT_H_

#include "Regolith/Architecture/PhysicalObject.h"


namespace Regolith
{

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


#endif // REGOLITH_ARCHITECTURE_COLLIDABLE_OBJECT_H_

