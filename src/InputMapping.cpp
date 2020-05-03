
#include "InputMapping.h"

#include "Controllable.h"

#include "logtastic.h"


namespace Regolith
{

////////////////////////////////////////////////////////////////////////////////
  // Keyboard/Button Mapping class

  KeyboardMapping::KeyboardMapping() :
    _theMap(),
    _lastBehaviour( INPUT_ACTION_NULL ),
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



  void KeyboardMapping::registerBehaviour( unsigned int scan_code, InputBehaviour action )
  {
    DEBUG_STREAM << "Keyboard Mapping registered: " << action << " -> " << scan_code;
    _theMap[ scan_code ] = action;
  }


  InputBehaviour KeyboardMapping::getBehaviour( SDL_Event& event )
  {
    if ( event.key.repeat != 0 )
    {
      _lastBehaviour = INPUT_ACTION_NULL;
      return _lastBehaviour;
    }

    if ( event.type == SDL_KEYDOWN )
    {
      _lastValue = true;
    }
    else
    {
      _lastValue = false;
    }
    _lastBehaviour = _theMap[event.key.keysym.scancode];

    return _lastBehaviour;
  }


  void KeyboardMapping::propagate( Controllable* object ) const
  {
    object->booleanAction( (InputAction)_lastBehaviour, _lastValue );
  }


  InputBehaviour KeyboardMapping::getRegisteredBehaviour( unsigned int code ) const
  {
    return _theMap[code];
  }


////////////////////////////////////////////////////////////////////////////////
  // Motion Mapping class

  MotionMapping::MotionMapping() :
    _theBehaviour( INPUT_ACTION_MOUSE_MOVE ),
    _lastPosition( 0.0 )
  {
  }


  MotionMapping::~MotionMapping()
  {
  }



  void MotionMapping::registerBehaviour( unsigned int, InputBehaviour action )
  {
    DEBUG_STREAM << "Motion Mapping registered: " << action;
    _theBehaviour = action;
  }


  InputBehaviour MotionMapping::getBehaviour( SDL_Event& event )
  {
    _lastPosition.x() = event.motion.x;
    _lastPosition.y() = event.motion.y;

    return _theBehaviour;
  }


  void MotionMapping::propagate( Controllable* object ) const
  {
    DEBUG_STREAM << "PROPAGATING MOUSE_MOVE " << object;
    object->vectorAction( (InputAction)_theBehaviour, _lastPosition );
  }


  InputBehaviour MotionMapping::getRegisteredBehaviour( unsigned int ) const
  {
    return _theBehaviour;
  }


////////////////////////////////////////////////////////////////////////////////
  // Controller Axis Mapping class

//  ControllerAxisMapping::ControllerAxisMapping( unsigned int size ) :
//    _theMap( new InputBehaviour[size] ),
//    _lastBehaviour(),
//    _lastPosition()
//  {
//  }
//
//
//  void ControllerAxisMapping::registerBehaviour( unsigned int controller, InputBehaviour action )
//  {
//    _theMap[ controller ] = action;
//  }
//
//
//  InputBehaviour& ControllerAxisMapping::getBehaviour( SDL_Event& event )
//  {
//    _lastPosition = event.jaxis.value;
//    _lastBehaviour = _theMap[ event.jaxis.axis ];
//    return _lastBehaviour;
//  }
//
//
//  void ControllerAxisMapping::propagate( Controllable* object ) const
//  {
//    object->floatBehaviour( _lastBehaviour, _lastPosition );
//  }


////////////////////////////////////////////////////////////////////////////////
  // Mouse Mapping class

  MouseMapping::MouseMapping() :
    _theMap(),
    _lastBehaviour( 0 ),
    _lastPosition( 0.0 ),
    _lastValue( false )
  {
    for ( unsigned int i = 0; i < 14; ++i )
    {
      _theMap[ i ] = INPUT_ACTION_CLICK;
    }
  }


  MouseMapping::~MouseMapping()
  {
  }



  void MouseMapping::registerBehaviour( unsigned int button, InputBehaviour action )
  {
    DEBUG_STREAM << "Mouse Mapping registered: " << action << " -> " << button;
    _theMap[ button ] = action;
  }


  InputBehaviour MouseMapping::getBehaviour( SDL_Event& event )
  {
    _lastPosition.x() = event.button.x;
    _lastPosition.y() = event.button.y;

    if ( event.type == SDL_MOUSEBUTTONDOWN )
    {
      _lastValue = true;
    }
    else
    {
      _lastValue = false;
    }
    _lastBehaviour = _theMap[event.button.button];

    return _lastBehaviour;
  }


  void MouseMapping::propagate( Controllable* object ) const
  {
    object->mouseAction( (InputAction)_lastBehaviour, _lastValue, _lastPosition );
  }


  InputBehaviour MouseMapping::getRegisteredBehaviour( unsigned int code ) const
  {
    return _theMap[code];
  }

}

