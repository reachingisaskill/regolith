
#ifndef REGOLITH_ARCHITECTURE_ANIMATED_H_
#define REGOLITH_ARCHITECTURE_ANIMATED_H_

#include "Global/Global.h"
#include "Architecture/GameObject.h"


namespace Regolith
{
  namespace Interfaces
  {

    /*
     * Class which defines the interface for animated objects.
     * This is any object, visible or not, that requires a call to void update( timestep ) on every frame
     */
    class Animated : virtual public GameObject
    {
      public:
        virtual ~Animated() {}

        bool hasAnimation() const override { return true; }

        void update( Uint32 ) = 0;
    };

  }
}

#endif // REGOLITH_ARCHITECTURE_ANIMATED_H_


