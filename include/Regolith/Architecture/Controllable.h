
#ifndef REGOLITH_ARCHITECTURE_CONTROLLABLE_H_
#define REGOLITH_ARCHITECTURE_CONTROLLABLE_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/GameObject.h"
#include "Regolith/Architecture/ControllableInterface.h"


namespace Regolith
{

  /*
   * This class defines the base class for game objects that can respond to hardware input controls
   */
  class Controllable : public ControllableInterface, virtual public GameObject
  {
    public :
      // Make the detructor virtual
      virtual ~Controllable() {}


      virtual void configure( Json::Value&, ContextGroup&, DataHandler& ) override {}


      bool hasInput() const { return true; }

  };

}

#endif // REGOLITH_ARCHITECTURE_CONTROLLABLE_H_

