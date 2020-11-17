
#ifndef REGOLITH_ARCHITECTURE_BUTTON_OBJECT_H_
#define REGOLITH_ARCHITECTURE_BUTTON_OBJECT_H_

#include "Regolith/Architecture/GameObject.h"
#include "Regolith/Architecture/ButtonInterface.h"

namespace Regolith
{

  class ButtonObject : virtual public GameObject, public ButtonInterface
  {
    private:

    public:
      ButtonObject() {}

      virtual ~ButtonObject() {}

      // This object is clickable
      virtual bool hasButton() const override { return true; }

  };

}

#endif // REGOLITH_ARCHITECTURE_BUTTON_OBJECT_H_

