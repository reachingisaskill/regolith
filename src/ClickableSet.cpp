
#include "ClickableSet.h"

#include "Manager.h"


namespace Regolith
{

  ClickableSet::ClickableSet() :
    _buttons(),
    _currentFocus( _buttons.begin() )
  {
  }


  void ClickableSet::addButton( Button* button )
  {
    _buttons.insert( button );
    _currentFocus = _buttons.begin();
  }


  void ClickableSet::focusNext()
  {
    (*_currentFocus)->takeFocus();
    ++_currentFocus;
    if ( _currentFocus == _buttons.end() )
      _currentFocus = _buttons.begin();

    (*_currentFocus)->giveFocus();
  }


  void ClickableSet::focusPrev()
  {
    (*_currentFocus)->takeFocus();
    if ( _currentFocus == _buttons.begin() )
      _currentFocus = _buttons.end();
    --_currentFocus;

    (*_currentFocus)->giveFocus();
  }


  void ClickableSet::select()
  {
    Manager* man = Manager::getInstance();

    if ( _currentFocus == _buttons.end() )

    if ( (*_currentFocus != nullptr ) && (*_currentFocus)->tryClick() )
    {
      InputAction event = (*_currentFocus)->getOption();
//      man->currentContext()->raiseContextEvent( event );
      man->currentContext()->booleanAction( event, true );
    }
  }


  void ClickableSet::clear()
  {
    _buttons.clear();
    _currentFocus = ButtonSet::iterator();
  }


  void ClickableSet::clickHere( Vector& )
  {
    // Write the code to determine if the mouse clicked on anything interactable
  }
}

