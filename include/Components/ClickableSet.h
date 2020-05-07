
#ifndef REGOLITH_CLICKABLE_SET_H_
#define REGOLITH_CLICKABLE_SET_H_

#include "Managers/Definitions.h"
#include "GameObjects/Button.h"

#include <set>


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

  class ClickableSet
  {
    typedef std::set< Button* > ButtonSet;

    private:
      ButtonSet _buttons;

      ButtonSet::iterator _currentFocus;

    public:
      ClickableSet();

      void addButton( Button* );


      void focusNext();

      void focusPrev();

      void select();

      void clear();

      void mouseDown( const Vector& );

      void mouseUp( const Vector& );

      void mouseMove( const Vector& );
  };

}

#endif // REGOLITH_CLICKABLE_SET_H_

