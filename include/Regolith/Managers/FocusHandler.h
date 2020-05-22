
#ifndef REGOLITH_MANAGERS_FOCUS_HANDLER_H_
#define REGOLITH_MANAGERS_FOCUS_HANDLER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/Clickable.h"
#include "Regolith/Architecture/ControllableInterface.h"

#include <list>


namespace Regolith
{

  /*
   * Class provides the logic for a context to handle the focus of buttons/objects.
   * It Handles the mouse clicks and movements to handle different clickable object state changes
   * In the future it will probably have a binary seach tree rather than a list, but we'll get there.
   */
  class FocusHandler : public ControllableInterface
  {
    typedef std::list< Clickable* > ClickableList;

    private:
      ClickableList _clickables;
      ClickableList::iterator _currentFocus;

    public:
      FocusHandler();

      virtual ~FocusHandler();

      // Add an object to the handler
      void addObject( Clickable*, bool f = false );


//////////////////////////////////////////////////
      // Define an iterator interface

//      typedef ClickableSet::iterator iterator;
//      typedef ClickableSet::const_iterator const_iterator;
//
//      iterator begin() { return _clickables.begin(); }
//      const_iterator begin() const { return _clickables.begin(); }
//
//      iterator end() { return _clickables.end(); }
//      const_iterator end() const { return _clickables.end(); }


//////////////////////////////////////////////////
      // Requirements for the ControllableInterface - input action handling

      // Register context-wide actions with the handler
      virtual void registerActions( InputHandler& ) override;

      // Interfaces for input
      // Handled and mapped actions
      virtual void inputAction( const InputAction& ) override;
      virtual void booleanAction( const InputAction&, bool ) override;
      virtual void floatAction( const InputAction&, float ) override {}
      virtual void vectorAction( const InputAction&, const Vector& ) override;
      virtual void mouseAction( const InputAction&, bool, const Vector& ) override;

  };

}

#endif // REGOLITH_MANAGERS_FOCUS_HANDLER_H_

