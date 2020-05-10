
#ifndef REGOLITH_ARCHITECTURE_ANIMATED_INTERFACE_H_
#define REGOLITH_ARCHITECTURE_ANIMATED_INTERFACE_H_

#include "Regolith/Global/Global.h"


namespace Regolith
{
  namespace Interfaces
  {

    /*
     * Class which defines the interface for any generic animated class
     * This is any object, visible or not, that requires a call to void update( timestep ) on every frame
     */
    class AnimatedInterface
    {
      public:
        virtual ~AnimatedInterface() {}

        virtual void update( Uint32 ) = 0;
    };

  }
}

#endif // REGOLITH_ARCHITECTURE_ANIMATED_INTERFACE_H_

