
#ifndef REGOLITH_ARCHITECTURE_ANIMATED_H_
#define REGOLITH_ARCHITECTURE_ANIMATED_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/AnimatedInterface.h"
#include "Regolith/Architecture/GameObject.h"


namespace Regolith
{

  /*
   * Class which defines the interface for gameobjects that are animated.
   * This is any object, visible or not, that requires a call to void update( timestep ) on every frame
   */
  class Animated : virtual public AnimatedInterface, virtual public GameObject
  {
    public:
      virtual ~Animated() {}

      virtual void configure( Json::Value& ) override {}


      bool hasAnimation() const override { return true; }

//        void update( float ) override = 0;
  };

}

#endif // REGOLITH_ARCHITECTURE_ANIMATED_H_


