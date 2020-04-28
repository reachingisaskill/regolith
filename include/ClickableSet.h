
#ifndef __REGOLITH__CLICKABLE_SET_H__
#define __REGOLITH__CLICKABLE_SET_H__

#include "Definitions.h"

#include "ButtonSprite.h"
#include "Controllable.h"


namespace Regolith
{


  /*
   * Defines a class that stores a set of buttons for a given context.
   * This class handles the concept of "focus" allowing both arrow keys and mouse
   * interactions to select a specific button. The memory for the button sprites is not
   * owned by this class. Rather, this class simply calls giveFocus() to the button 
   * which is highlighted and uses some sort of call back function (TBD) to return the 
   * answer to the owning object.
   */

  class ClickableSet : public Controllable
  {
    typedef std::set< Button* > ButtonSet;

    private:
      ButtonSet _buttons;

      Button* _currentFocus;

    public:
      ClickableSet();

      void addButton( Button* );


      // Update the objects behaviour based on the provided timestep
      virtual void registerEvents( InputHandler* ) = 0;

      // Interfaces for input
      virtual void eventAction( const InputAction& ) = 0;
      virtual void booleanAction( const InputAction&, bool ) = 0;
      virtual void floatAction( const InputAction&, float ) = 0;
      virtual void vectorAction( const InputAction&, const Vector& ) = 0;
  };

}

#endif // __REGOLITH__CLICKABLE_SET_H__

