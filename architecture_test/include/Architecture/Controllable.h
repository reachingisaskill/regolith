
#ifndef REGOLITH_ARCHITECTURE_CONTROLLABLE_H_
#define REGOLITH_ARCHITECTURE_CONTROLLABLE_H_

#include "Global/Global.h"
#include "Architecture/GameObject.h"
#include "Architecture/ControllableInterface.h"


namespace Regolith
{
  namespace Interfaces
  {

    /*
     * This class defines the base class for game objects that can respond to hardware input controls
     */
    class Controllable : public ControllableInterface, virtual public GameObject
    {
      public :
        // Make the detructor virtual
        virtual ~Controllable() {}

        bool hasInput() const { return true; }

    };

  }
}

#endif // REGOLITH_ARCHITECTURE_CONTROLLABLE_H_

