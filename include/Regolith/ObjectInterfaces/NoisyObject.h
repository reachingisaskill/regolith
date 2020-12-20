
#ifndef REGOLITH_OBJECT_INTERFACES_NOISY_OBJECT_H_
#define REGOLITH_OBJECT_INTERFACES_NOISY_OBJECT_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/Architecture/NoisyInterface.h"


namespace Regolith
{
  /*
   * Defines the intferface for classes that can trigger sounds effects to be played.
   */
  class NoisyObject : public NoisyInterface, virtual public PhysicalObject
  {
    public:
      // Simple contructor
      NoisyObject() {}

      // Make the destructor virtual
      virtual ~NoisyObject() {}

      // Tell the caller that this object can make sounds
      virtual bool hasAudio() const { return true; }

  };

}

#endif // REGOLITH_OBJECT_INTERFACES_NOISY_OBJECT_H_

