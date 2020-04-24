
#include "InputMapping.h"

#include "Drawable.h"


namespace Regolith
{

////////////////////////////////////////////////////////////////////////////////
  // Keyboard/Button Mapping class

  KeyboardMapping::KeyboardMapping( unsigned int size ) :
    _theMap( new InputAction[size] )
  {
  }


  void KeyboardMapping::registerAction( unsigned int scan_code, InputAction action )
  {
    _theMap[ scan_code ] = action;
  }


  InputAction& KeyboardMapping::getAction( SDL_Event& event )
  {
    if ( event.type == SDL_KEYDOWN )
    {
      _lastValue = true;
    }
    else
    {
      _lastValue = false;
    }
    _lastAction = _theMap[event.key.keysym.scancode];
    return _lastAction;
  }


  void KeyboardMapping::propagate( Drawable* object ) const
  {
    object->eventAction( _lastAction, _lastValue );
  }


////////////////////////////////////////////////////////////////////////////////
  // Controller Axis Mapping class

  ControllerAxisMapping::ControllerAxisMapping( unsigned int size ) :
    _theMap( new InputAction[size] ),
    _lastAction(),
    _lastPosition()
  {
  }


  void ControllerAxisMapping::registerAction( unsigned int controller, InputAction action )
  {
    _theMap[ controller ] = action;
  }


  InputAction& ControllerAxisMapping::getAction( SDL_Event& event )
  {
    _lastPosition = event.jaxis.value;
    _lastAction = _theMap[ event.jaxis.axis ];
    return _lastAction;
  }


  void ControllerAxisMapping::propagate( Drawable* object ) const
  {
    object->floatAction( _lastAction, _lastPosition );
  }


////////////////////////////////////////////////////////////////////////////////
  // Mouse Mapping class

}

