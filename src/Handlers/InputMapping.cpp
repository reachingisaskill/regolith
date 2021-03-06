
#include "Regolith/Architecture/ControllableInterface.h"
#include "Regolith/Handlers/InputMapping.h"


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
    DEBUG_STREAM << "Keyboard Mapping registered: " << action << " -> " << scan_code;
    _theMap[ scan_code ] = action;
  }


  InputAction KeyboardMapping::getAction( SDL_Event& event )
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


  void KeyboardMapping::propagate( ControllableInterface* object ) const
  {
    object->booleanAction( _lastAction, _lastValue );
  }


  InputAction KeyboardMapping::getRegisteredAction( unsigned int code ) const
  {
    return _theMap[code];
  }


////////////////////////////////////////////////////////////////////////////////
  // Motion Mapping class

  MotionMapping::MotionMapping() :
    _theAction( INPUT_ACTION_MOUSE_MOVE ),
    _lastPosition( 0.0 )
  {
  }


  MotionMapping::~MotionMapping()
  {
  }



  void MotionMapping::registerAction( unsigned int, InputAction action )
  {
    DEBUG_STREAM << "Motion Mapping registered: " << action;
    _theAction = action;
  }


  InputAction MotionMapping::getAction( SDL_Event& event )
  {
    _lastPosition.x() = event.motion.x;
    _lastPosition.y() = event.motion.y;

    return _theAction;
  }


  void MotionMapping::propagate( ControllableInterface* object ) const
  {
    DEBUG_STREAM << "PROPAGATING MOUSE_MOVE " << object;
    object->vectorAction( (InputAction)_theAction, _lastPosition );
  }


  InputAction MotionMapping::getRegisteredAction( unsigned int ) const
  {
    return _theAction;
  }


////////////////////////////////////////////////////////////////////////////////
  // Mouse Mapping class

  MouseMapping::MouseMapping() :
    _theMap(),
    _lastAction( INPUT_ACTION_NULL ),
    _lastPosition( 0.0 ),
    _lastValue( false )
  {
    for ( unsigned int i = 0; i < NUMBER_MOUSE_BUTTONS; ++i )
    {
      _theMap[ i ] = INPUT_ACTION_CLICK;
    }
  }


  MouseMapping::~MouseMapping()
  {
  }



  void MouseMapping::registerAction( unsigned int button, InputAction action )
  {
    DEBUG_STREAM << "Mouse Mapping registered: " << action << " -> " << button;
    _theMap[ button ] = action;
  }


  InputAction MouseMapping::getAction( SDL_Event& event )
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
    _lastAction = _theMap[event.button.button];

    return _lastAction;
  }


  void MouseMapping::propagate( ControllableInterface* object ) const
  {
    object->mouseAction( _lastAction, _lastValue, _lastPosition );
  }


  InputAction MouseMapping::getRegisteredAction( unsigned int code ) const
  {
    return _theMap[code];
  }


////////////////////////////////////////////////////////////////////////////////
  // Controller Axis Mapping class

  ControllerAxisMapping::ControllerAxisMapping() :
    _theMap(),
    _lastAction( INPUT_ACTION_NULL ),
    _lastPosition()
  {
    for ( unsigned int i = 0; i < SDL_CONTROLLER_AXIS_MAX; ++i )
    {
      _theMap[ i ] = INPUT_ACTION_NULL;
    }
  }


  void ControllerAxisMapping::registerAction( unsigned int axis, InputAction action )
  {
    _theMap[ axis ] = action;
  }


  InputAction ControllerAxisMapping::getAction( SDL_Event& event )
  {
    _lastPosition = (float)event.jaxis.value / AXIS_MAX_VALUE;
    _lastAction = _theMap[ event.jaxis.axis ];
    return _lastAction;
  }


  void ControllerAxisMapping::propagate( ControllableInterface* object ) const
  {
    object->floatAction( _lastAction, _lastPosition );
  }


  InputAction ControllerAxisMapping::getRegisteredAction( unsigned int code ) const
  {
    return _theMap[code];
  }


////////////////////////////////////////////////////////////////////////////////
  // Controller Button Mapping class

  ControllerButtonMapping::ControllerButtonMapping() :
    _theMap(),
    _lastAction( INPUT_ACTION_NULL ),
    _lastValue( false )
  {
    for ( unsigned int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; ++i )
    {
      _theMap[ i ] = INPUT_ACTION_NULL;
    }
  }


  void ControllerButtonMapping::registerAction( unsigned int button, InputAction action )
  {
    _theMap[ button ] = action;
  }


  InputAction ControllerButtonMapping::getAction( SDL_Event& event )
  {
    if ( event.type == SDL_CONTROLLERBUTTONDOWN )
    {
      _lastValue = true;
    }
    else
    {
      _lastValue = false;
    }
    _lastAction = _theMap[event.cbutton.button];

    return _lastAction;
  }


  void ControllerButtonMapping::propagate( ControllableInterface* object ) const
  {
    object->booleanAction( _lastAction, _lastValue );
  }


  InputAction ControllerButtonMapping::getRegisteredAction( unsigned int code ) const
  {
    return _theMap[code];
  }

}

