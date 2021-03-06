
#ifndef REGOLITH_OBJECT_INTERFACES_ANIMATED_OBJECT_H_
#define REGOLITH_OBJECT_INTERFACES_ANIMATED_OBJECT_H_

#include "Regolith/Architecture/PhysicalObject.h"


namespace Regolith
{

  class AnimatedObject : virtual public PhysicalObject
  {
    private:

    public:
      AnimatedObject() {}
      virtual ~AnimatedObject() {}

      // Signal that derived classes have animations to increment
      virtual bool hasAnimation() const override { return true; }


      // Perform an update to all the animations
      virtual void update( float ) = 0;


  };

}

#endif // REGOLITH_OBJECT_INTERFACES_ANIMATED_OBJECT_H_

