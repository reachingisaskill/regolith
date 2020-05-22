
#include "Regolith/Managers/FocusHandler.h"
#include "Regolith/Managers/InputHandler.h"


namespace Regolith
{
  bool contains( Clickable* obj, const Vector& vec )
  {
    if ( ( vec.x() > obj->position().x() ) && ( vec.x() < (obj->position().x() + obj->getWidth()) ) )
    {
      if ( ( vec.y() > obj->position().y() ) && ( vec.y() < (obj->position().y() + obj->getHeight()) ) )
      {
        return true;
      }
    }
    return false;
  }

////////////////////////////////////////////////////////////////////////////////////////////////////

  FocusHandler::FocusHandler() :
    ControllableInterface(),
    _clickables(),
    _currentFocus( _clickables.end() )
  {
  }


  FocusHandler::~FocusHandler()
  {
  }


  void FocusHandler::addObject( Clickable* object, bool giveFocus )
  {
    if ( _clickables.empty() )
    {
      _clickables.push_back( object );
      _currentFocus = _clickables.begin();
      (*_currentFocus)->giveFocus();
    }
    else
    {
      _clickables.push_back( object );

      if ( giveFocus )
      {
        if ( _currentFocus != _clickables.end() )
          (*_currentFocus)->takeFocus();

        _currentFocus = --_clickables.end();
        (*_currentFocus)->giveFocus();
      }
    }
  }


  void FocusHandler::registerActions( InputHandler& handler )
  {
    handler.registerInputRequest( this, INPUT_ACTION_NEXT );
    handler.registerInputRequest( this, INPUT_ACTION_PREV );
    handler.registerInputRequest( this, INPUT_ACTION_SELECT );
    handler.registerInputRequest( this, INPUT_ACTION_MOUSE_MOVE );
    handler.registerInputRequest( this, INPUT_ACTION_CLICK );
    handler.registerInputRequest( this, INPUT_ACTION_BACK );
    handler.registerInputRequest( this, INPUT_ACTION_RETURN );
    handler.registerInputRequest( this, INPUT_ACTION_CANCEL );
  }


  void FocusHandler::inputAction( const InputAction& action )
  {
    if ( _clickables.empty() ) return;

    switch ( action )
    {
      case INPUT_ACTION_NEXT :
        (*_currentFocus)->takeFocus();

        ++_currentFocus;
        if ( _currentFocus == _clickables.end() )
          _currentFocus = _clickables.begin();

        (*_currentFocus)->giveFocus();
        break;

      case INPUT_ACTION_PREV :
        (*_currentFocus)->takeFocus();

        if ( _currentFocus == _clickables.begin() )
          _currentFocus = _clickables.end();
        --_currentFocus;

        (*_currentFocus)->giveFocus();
        break;

      case INPUT_ACTION_SELECT :
//        (*_currentFocus)->down();
        (*_currentFocus)->click();
//        (*_currentFocus)->up();
        break;

      default:
        break;
    }
  }


  void FocusHandler::booleanAction( const InputAction& action, bool value )
  {
    // Immediately call the input action function if a key is released.
    // I think this is the simplest implementation
    if ( ! value ) inputAction( action );
  }


  void FocusHandler::vectorAction( const InputAction& action, const Vector& vec )
  {
    if ( action != INPUT_ACTION_MOUSE_MOVE || _clickables.empty() ) return;

    ClickableList::iterator end = _clickables.end();
    (*_currentFocus)->takeFocus();

    for ( ClickableList::iterator it = _clickables.begin(); it != end; ++it )
    {
      if ( contains( (*it), vec ) )
      {
        (*it)->giveFocus();
        _currentFocus = it;
        return;
      }
    }
  }


  void FocusHandler::mouseAction( const InputAction& action, bool click, const Vector& vec )
  {
    if ( action != INPUT_ACTION_CLICK || _clickables.empty() ) return;

    ClickableList::iterator end = _clickables.end();

    for ( ClickableList::iterator it = _clickables.begin(); it != end; ++it )
    {
      if ( contains( (*it), vec ) )
      {
        if ( click )
        {
          (*it)->down();
        }
        else
        {
          (*it)->up();
        }
        return;
      }
    }
  }


}

