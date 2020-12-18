
#ifndef REGOLITH_ARCHITECTURE_BUTTON_OBJECT_H_
#define REGOLITH_ARCHITECTURE_BUTTON_OBJECT_H_

#include "Regolith/Architecture/GameObject.h"


namespace Regolith
{
  // Forward declare the focus handler
  class FocusHandler;

  /*
   * The class that defines an interface for anything that could act like a button.
   */
  class ButtonObject : virtual public GameObject
  {
    private:
      FocusHandler* _handler;

    public:
      ButtonObject() : _handler( nullptr ) {}

      virtual ~ButtonObject() {}


      // This object is clickable
      virtual bool hasButton() const override { return true; }


      // Register this object with the focus handler
      void registerFocus( FocusHandler* );


      // Focus is given to this object by the focus handler
      virtual void giveFocus() = 0;

      // Focus is taken from this object by the focus handler
      virtual void takeFocus() = 0;

      // Forcibly requests focus from the focus handler
      void requestFocus();
  };

}

#endif // REGOLITH_ARCHITECTURE_BUTTON_OBJECT_H_

