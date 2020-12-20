
#ifndef REGOLITH_OBJECT_INTERFACES_CONTROLLABLE_OBJECT_H_
#define REGOLITH_OBJECT_INTERFACES_CONTROLLABLE_OBJECT_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/Architecture/ControllableInterface.h"


namespace Regolith
{

  /*
   * This class defines the base class for game objects that can respond to hardware input controls
   */
  class ControllableObject : public ControllableInterface, virtual public PhysicalObject
  {
    public :
      // Make the detructor virtual
      virtual ~ControllableObject() {}

      // Signal that derived classes expect input
      bool hasInput() const { return true; }

  };

}

#endif // REGOLITH_OBJECT_INTERFACES_CONTROLLABLE_OBJECT_H_

