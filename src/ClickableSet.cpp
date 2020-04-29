
#include "ClickableSet.h"


namespace Regolith
{

  ClickableSet::ClickableSet() :
    _buttons(),
    _currentFocus( nullptr )
  {
  }


  void ClickableSet::addButton( Button* )
  {
  }


  void ClickableSet::registerActions( InputHandler* handler )
  {
  }


//  void ClickableSet::eventAction( const InputEvent& event )
//  {
//  }


  void ClickableSet::booleanAction( const InputAction& action, bool )
  {
  }


//  void ClickableSet::floatAction( const InputAction& action, float )
//  {
//  }


  void ClickableSet::vectorAction( const InputAction& action, const Vector& vector )
  {
  }

}

