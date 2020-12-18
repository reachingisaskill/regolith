
#ifndef REGOLITH_ARCHITECTURE_COLLIDABLE_OBJECT_H_
#define REGOLITH_ARCHITECTURE_COLLIDABLE_OBJECT_H_

#include "Regolith/Architecture/GameObject.h"


namespace Regolith
{

  class CollidableObject : virtual public GameObject
  {
    private:

    public:
      CollidableObject() {}

      virtual ~CollidableObject() {}


      // Signal that derived classes have collision
      virtual bool hasCollision() const override { return true; }

  };
}

#endif // REGOLITH_ARCHITECTURE_COLLIDABLE_OBJECT_H_
