
#include "ClickableSet.h"

#include "Manager.h"


namespace Regolith
{

  ClickableSet::ClickableSet() :
    _buttons(),
    _currentFocus()
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

    if ( (*_currentFocus)->tryClick() )
    {
      ContextEvent event = (*_currentFocus)->getOption();
      man->currentContext()->raiseContextEvent( event );
    }
  }


  void ClickableSet::clear()
  {
    _buttons.clear();
    _currentFocus = ButtonSet::iterator();
  }


  void ClickableSet::registerActions( InputHandler* handler )
  {
    handler->registerInputRequest( this, INPUT_ACTION_NEXT );
    handler->registerInputRequest( this, INPUT_ACTION_PREV );
    handler->registerInputRequest( this, INPUT_ACTION_SELECT );
    handler->registerInputRequest( this, INPUT_ACTION_JUMP ); // Bind the jump key just in case!
    handler->registerInputRequest( this, INPUT_ACTION_CLICK ); // Bind the mouse click
  }


//  void ClickableSet::eventAction( const RegolithEvent& event )
//  {
//  }


  void ClickableSet::booleanAction( const InputAction& action, bool value )
  {
    switch ( action )
    {
      case INPUT_ACTION_NEXT :
        if ( value ) this->focusNext();
        break;

      case INPUT_ACTION_PREV :
        if ( value ) this->focusPrev();
        break;

      case INPUT_ACTION_SELECT :
      case INPUT_ACTION_JUMP :
        if ( value ) this->select();
        break;

      default :
        break;
    }
  }


//  void ClickableSet::floatAction( const InputAction& action, float )
//  {
//  }


  void ClickableSet::vectorAction( const InputAction& action, const Vector& vector )
  {
  }

}

