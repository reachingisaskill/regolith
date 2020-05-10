
#ifndef REGOLITH_ARCHITECTURE_ANIMATED_INTERFACE_H_
#define REGOLITH_ARCHITECTURE_ANIMATED_INTERFACE_H_

#include "Regolith/Global/Global.h"


namespace Regolith
{
  namespace Architecture
  {

    /*
     * Class which defines the interface for any generic animated class
     * This is any object, visible or not, that requires a call to void update( timestep ) on every frame
     */
    class AnimatedInterface
    {
      public:
        virtual ~AnimatedInterface() {}

        virtual void update( float ) = 0;
    };

  }
}

#endif // REGOLITH_ARCHITECTURE_ANIMATED_INTERFACE_H_

