
#ifndef REGOLITH_ARCHITECTURE_CONTROLLABLE_OBJECT_H_
#define REGOLITH_ARCHITECTURE_CONTROLLABLE_OBJECT_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/GameObject.h"
#include "Regolith/Architecture/ControllableInterface.h"


namespace Regolith
{

  /*
   * This class defines the base class for game objects that can respond to hardware input controls
   */
  class ControllableObject : public ControllableInterface, virtual public GameObject
  {
    public :
      // Make the detructor virtual
      virtual ~ControllableObject() {}


      virtual void configure( Json::Value&, ContextGroup&, DataHandler& ) override {}


      bool hasInput() const { return true; }

  };

}

#endif // REGOLITH_ARCHITECTURE_CONTROLLABLE_OBJECT_H_

