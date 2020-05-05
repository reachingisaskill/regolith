
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
//    Manager* man = Manager::getInstance();

    if ( _currentFocus == _buttons.end() )

    if ( (*_currentFocus != nullptr ) )
    {
      (*_currentFocus)->click();
    }
  }


  void ClickableSet::clear()
  {
    _buttons.clear();
    _currentFocus = ButtonSet::iterator();
  }


  void ClickableSet::mouseDown( const Vector& point )
  {
    ButtonSet::iterator end = _buttons.end();
    for ( ButtonSet::iterator it = _buttons.begin(); it != end; ++it )
    {
      if ( contains( (*it), point ) )
      {
        (*it)->down();
      }
      else
      {
        (*it)->takeFocus();
      }
    }
  }


  void ClickableSet::mouseUp( const Vector& point )
  {
    ButtonSet::iterator end = _buttons.end();
    for ( ButtonSet::iterator it = _buttons.begin(); it != end; ++it )
    {
      if ( contains( (*it), point ) )
      {
        (*it)->up();
      }
      else
      {
        (*it)->takeFocus();
      }
    }
  }


  void ClickableSet::mouseMove( const Vector& point )
  {
    ButtonSet::iterator end = _buttons.end();
    for ( ButtonSet::iterator it = _buttons.begin(); it != end; ++it )
    {
      if ( contains( (*it), point ) )
      {
        (*it)->giveFocus();
      }
      else
      {
        (*it)->takeFocus();
      }
    }
  }
}

