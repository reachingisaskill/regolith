
#include "Regolith/Managers/FocusHandler.h"
#include "Regolith/Managers/InputHandler.h"


namespace Regolith
{

  FocusHandler::FocusHandler() :
    _buttons( 1, nullptr ),
    _numberLookup(),
    _currentFocus( 0 )
  {
  }


  FocusHandler::~FocusHandler()
  {
  }


  void FocusHandler::addObject( ButtonObject* object )
  {
    if ( _numberLookup.find( object ) != _numberLookup.end() )
    {
      Exception ex( "FocusHandler::addObject()", "Attempting to add the same object twice to the focus handler" );
      throw ex;
    }

    _buttons.push_back( object );
    _numberLookup[ object ] = _buttons.size();

    if ( _currentFocus == 0 )
    {
      _currentFocus = 1;
      _buttons[ _currentFocus ]->giveFocus();
    }
  }


  void FocusHandler::next()
  {
    if ( (++_currentFocus) >= _buttons.size() )
    {
      _currentFocus = 1;
    }
  }


  void FocusHandler::previous()
  {
    if ( (--_currentFocus) <= 0 )
    {
      _currentFocus = _buttons.size()-1;
    }
  }


  void FocusHandler::requestFocus( ButtonObject* object )
  {
    _buttons[ _currentFocus ]->takeFocus();
    _currentFocus = _numberLookup[ object ];
    _buttons[ _currentFocus ]->giveFocus();
  }

}

