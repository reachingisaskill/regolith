
#include "InputMapping.h"

#include "Controllable.h"

#include "logtastic.h"


namespace Regolith
{

////////////////////////////////////////////////////////////////////////////////
  // Keyboard/Button Mapping class

  KeyboardMapping::KeyboardMapping() :
    _theMap(),
    _lastAction( INPUT_ACTION_NULL ),
    _lastValue( false )
  {
    for ( unsigned int i = 0; i < SDL_NUM_SCANCODES; ++i )
    {
      _theMap[ i ] = INPUT_ACTION_NULL;
    }
  }


  KeyboardMapping::~KeyboardMapping()
  {
  }



  void KeyboardMapping::registerAction( unsigned int scan_code, InputAction action )
  {
    _theMap[ scan_code ] = action;
  }


  InputAction& KeyboardMapping::getAction( SDL_Event& event )
  {
    if ( event.key.repeat != 0 )
    {
      _lastAction = INPUT_ACTION_NULL;
      return _lastAction;
    }

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


  void KeyboardMapping::propagate( Controllable* object ) const
  {
    object->booleanAction( _lastAction, _lastValue );
  }


  InputAction KeyboardMapping::getRegisteredAction( unsigned int code ) const
  {
    return _theMap[code];
  }


////////////////////////////////////////////////////////////////////////////////
  // Controller Axis Mapping class

//  ControllerAxisMapping::ControllerAxisMapping( unsigned int size ) :
//    _theMap( new InputAction[size] ),
//    _lastAction(),
//    _lastPosition()
//  {
//  }
//
//
//  void ControllerAxisMapping::registerAction( unsigned int controller, InputAction action )
//  {
//    _theMap[ controller ] = action;
//  }
//
//
//  InputAction& ControllerAxisMapping::getAction( SDL_Event& event )
//  {
//    _lastPosition = event.jaxis.value;
//    _lastAction = _theMap[ event.jaxis.axis ];
//    return _lastAction;
//  }
//
//
//  void ControllerAxisMapping::propagate( Controllable* object ) const
//  {
//    object->floatAction( _lastAction, _lastPosition );
//  }


////////////////////////////////////////////////////////////////////////////////
  // Mouse Mapping class

}

