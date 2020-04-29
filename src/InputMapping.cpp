
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
  // RegolithEvent/Button Mapping class

  RegolithEventMapping::RegolithEventMapping() :
    _theMap(),
    _lastBehaviour( REGOLITH_NULL )
  {
    for ( unsigned int i = 0; i < REGOLITH_EVENT_TOTAL; ++i )
    {
      _theMap[ i ] = REGOLITH_NULL;
    }
  }


  RegolithEventMapping::~RegolithEventMapping()
  {
  }



  void RegolithEventMapping::registerBehaviour( unsigned int event_code, InputBehaviour action )
  {
    DEBUG_STREAM << "Regolith Event Mapping registered: " << action << " -> " << event_code;
    _theMap[ event_code ] = action;
  }


  InputBehaviour RegolithEventMapping::getBehaviour( SDL_Event& event )
  {
//    _lastBehaviour = _theMap[event.user.code];
    _lastBehaviour = event.user.code;
    return _lastBehaviour;
  }


  void RegolithEventMapping::propagate( Controllable* object ) const
  {
    object->eventAction( (InputEvent)_lastBehaviour );
  }


  InputBehaviour RegolithEventMapping::getRegisteredBehaviour( unsigned int event_code ) const
  {
    return _theMap[event_code];
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

}

