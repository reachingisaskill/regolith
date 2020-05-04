
#include "InputManager.h"
#include "Controllable.h"

#include "logtastic.h"


namespace Regolith
{

  InputManager::InputManager() :
    _inputHandlers( "input_handlers" ),
    _eventMaps(),
    _theEvent()
  {
  }


  InputManager::~InputManager()
  {
  }


  void InputManager::handleEvents( InputHandler* handler )
  {
//    if ( handler == nullptr )
//    {
//      WARN_LOG( "No input handler provided in this context" );
//    }

    while ( SDL_PollEvent( &_theEvent ) != 0 )
    {
      this->handleEvent( _theEvent, handler );
    }
  }


  void InputManager::handleEvent( SDL_Event& sdl_event, InputHandler* handler )
  {
    InputEventType event_type;
    InputMapping* mapper = nullptr;
    InputAction action;
    RegolithEvent event;
    Vector vec;

    ControllableSet::iterator end;

    DEBUG_STREAM << "Handling event";

    switch ( sdl_event.type )
    {
      //////////////////////////////////////////////////
      // Input-type events
      case SDL_KEYDOWN :
      case SDL_KEYUP :
        if ( handler == nullptr ) break;

        DEBUG_LOG( "  Keyboard key-press type event" );
        event_type = INPUT_TYPE_KEYBOARD;
        mapper = handler->_inputMaps[ event_type ];
        action = mapper->getAction( sdl_event );
        if ( action == INPUT_ACTION_NULL ) break;

        end = handler->getRegisteredObjects( action ).end();
        for ( ControllableSet::iterator it = handler->getRegisteredObjects( action ).begin(); it != end; ++it )
        {
          DEBUG_STREAM << "  Propagating action : " << action;
          mapper->propagate( (*it) );
        }
        break;

      case SDL_MOUSEMOTION :
        if ( handler == nullptr ) break;

        event_type = INPUT_TYPE_MOUSE_MOVE;
        mapper = handler->_inputMaps[ event_type ];
        action = mapper->getAction( sdl_event );
        DEBUG_STREAM << "  Regolith Mouse Motion Event";

        end = handler->getRegisteredObjects( action ).end();
        for ( ControllableSet::iterator it = handler->getRegisteredObjects( action ).begin(); it != end; ++it )
        {
          DEBUG_STREAM << "  Propagating action : " << action;
          mapper->propagate( (*it) );
        }
        break;

      case SDL_MOUSEWHEEL :
        break;

      case SDL_MOUSEBUTTONDOWN :
      case SDL_MOUSEBUTTONUP :
        if ( handler == nullptr ) break;

        DEBUG_LOG( "  Mouse button-press type event" );
        event_type = INPUT_TYPE_MOUSE_BUTTON;
        mapper = handler->_inputMaps[ event_type ];
        action = mapper->getAction( sdl_event );
        if ( action == INPUT_ACTION_NULL ) break;

        end = handler->getRegisteredObjects( action ).end();
        for ( ControllableSet::iterator it = handler->getRegisteredObjects( action ).begin(); it != end; ++it )
        {
          DEBUG_STREAM << "  Propagating action : " << action;
          mapper->propagate( (*it) );
        }
        break;


      case SDL_CONTROLLERAXISMOTION :
        break;

      case SDL_CONTROLLERBUTTONDOWN :
      case SDL_CONTROLLERBUTTONUP :
        break;

      case SDL_JOYAXISMOTION :
      case SDL_JOYBALLMOTION :
      case SDL_JOYHATMOTION :
        break;

      case SDL_JOYBUTTONDOWN :
      case SDL_JOYBUTTONUP :
        break;

      //////////////////////////////////////////////////
      // Global-type events

      case SDL_WINDOWEVENT :
        event = REGOLITH_EVENT_WINDOW;
        DEBUG_STREAM << "  Regolith Window Event";
        end = this->getRegisteredObjects( event ).end();
        for ( ControllableSet::iterator it = this->getRegisteredObjects( event ).begin(); it != end; ++it )
        {
          DEBUG_STREAM << "  Propagating event : " << event;
          (*it)->eventAction( event, sdl_event );
        }
        break;


      case SDL_USEREVENT :
        event = (RegolithEvent)sdl_event.user.code;
        DEBUG_STREAM << "  Regolith User Event " << event;

        if ( event == REGOLITH_EVENT_NULL ) break;
        end = this->getRegisteredObjects( event ).end();
        for ( ControllableSet::iterator it = this->getRegisteredObjects( event ).begin(); it != end; ++it )
        {
          DEBUG_STREAM << "  Propagating event : " << event;
          (*it)->eventAction( event, sdl_event );
        }
        break;

      case SDL_DISPLAYEVENT :

      case SDL_SYSWMEVENT :

      case SDL_CONTROLLERDEVICEADDED :
      case SDL_CONTROLLERDEVICEREMOVED :
      case SDL_CONTROLLERDEVICEREMAPPED :

      case SDL_JOYDEVICEADDED :
      case SDL_JOYDEVICEREMOVED :

      case SDL_TEXTEDITING :
      case SDL_TEXTINPUT :
      case SDL_KEYMAPCHANGED :

      case SDL_FINGERDOWN :
      case SDL_FINGERUP :
      case SDL_FINGERMOTION :
      case SDL_DOLLARGESTURE :
      case SDL_DOLLARRECORD :
      case SDL_MULTIGESTURE :
      case SDL_CLIPBOARDUPDATE :
      case SDL_DROPFILE :
      case SDL_DROPTEXT :
      case SDL_DROPBEGIN :
      case SDL_DROPCOMPLETE :
      case SDL_AUDIODEVICEADDED :
      case SDL_AUDIODEVICEREMOVED :
      case SDL_SENSORUPDATE :
      case SDL_RENDER_TARGETS_RESET :
      case SDL_RENDER_DEVICE_RESET :
      default :
        // No logic for these event types yet.
        break;
    }
  }


  void InputManager::registerEventRequest( Controllable* object, RegolithEvent event )
  {
    INFO_STREAM << "Registered input request for event: " << event << " " << object;
    _eventMaps[event].insert( object );
  }


  ControllableSet& InputManager::getRegisteredObjects( RegolithEvent event )
  {
    return _eventMaps[event];
  }


  InputHandler* InputManager::requestHandler( std::string name )
  {
    if ( ! _inputHandlers.exists( name ) ) 
    {
      _inputHandlers.addObject( new InputHandler(), name );
    }

    return _inputHandlers.getByName( name );
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Input Handler

  InputHandler::InputHandler() :
    _inputMaps(),
    _actionMaps()
  {
    for ( size_t i = 0; i < INPUT_TYPE_TOTAL; ++i )
    {
      _inputMaps[ i ] = nullptr; 
    }

    _inputMaps[ INPUT_TYPE_KEYBOARD ] = new KeyboardMapping();
    _inputMaps[ INPUT_TYPE_MOUSE_BUTTON ] = new MouseMapping();
    _inputMaps[ INPUT_TYPE_MOUSE_MOVE ] = new MotionMapping();
  }


  InputHandler::~InputHandler()
  {
    for ( size_t i = 0; i < INPUT_TYPE_TOTAL; ++i )
    {
      if ( _inputMaps[ i ] != nullptr )
      {
        delete _inputMaps[ i ];
        _inputMaps[ i ] = nullptr;
      }
    }
  }


  void InputHandler::registerInputRequest( Controllable* object, InputAction action )
  {
    INFO_STREAM << "Registered input request for action: " << action << " " << object;
    _actionMaps[action].insert( object );
  }


  void InputHandler::registerInputAction( InputEventType event_type, unsigned int code, InputAction event )
  {
    _inputMaps[ event_type ]->registerAction( code, event );
  }


  InputAction InputHandler::getRegisteredInputAction( InputEventType event_type, unsigned int code )
  {
    return _inputMaps[ event_type ]->getRegisteredAction( code );
  }


}

