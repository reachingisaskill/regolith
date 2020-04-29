
#include "InputManager.h"

#include "logtastic.h"


namespace Regolith
{

  InputManager::InputManager() :
    _inputMaps(),
    _eventMaps(),
    _theEvent()
  {
    for ( size_t i = 0; i < INPUT_TYPE_TOTAL; ++i )
    {
      _inputMaps[ i ] = nullptr; 
    }

    _inputMaps[ INPUT_TYPE_KEYBOARD ] = new KeyboardMapping();
  }


  InputManager::~InputManager()
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


  void InputManager::handleEvents( InputHandler* handler )
  {
    if ( handler == nullptr )
    {
      WARN_LOG( "No input handler provided in this context" );
    }

    while ( SDL_PollEvent( &_theEvent ) != 0 )
    {
      this->handleEvent( _theEvent, handler );
    }
  }


  void InputManager::handleEvent( SDL_Event& sdl_event, InputHandler* handler )
  {
    // At this point we must assume ALL of the mapping objects are correctly configured!

    InputEventType event_type = INPUT_TYPE_NULL;
    InputMapping* mapper = nullptr;
    InputAction action = INPUT_ACTION_NULL;
    InputEvent event = INPUT_EVENT_NULL;

    ControllableSet::iterator end;

    DEBUG_STREAM << "Handling event";

    switch ( sdl_event.type )
    {
      case SDL_WINDOWEVENT :
        break;

      case SDL_KEYDOWN :
      case SDL_KEYUP :
        if ( handler == nullptr ) break;

        DEBUG_LOG( "  Keyboard key-press type event" );
        event_type = INPUT_TYPE_KEYBOARD;
        mapper = _inputMaps[ event_type ];
        action = (InputAction&)mapper->getBehaviour( sdl_event );

        if ( action == INPUT_ACTION_NULL ) break;

        end = handler->getRegisteredObjects( action ).end();
        for ( ControllableSet::iterator it = handler->getRegisteredObjects( action ).begin(); it != end; ++it )
        {
          DEBUG_STREAM << "  Propagating action : " << action;
          mapper->propagate( (*it) );
        }
        break;

      case SDL_MOUSEMOTION :
        break;

      case SDL_MOUSEWHEEL :
        break;

      case SDL_MOUSEBUTTONDOWN :
      case SDL_MOUSEBUTTONUP :
        break;


      case SDL_CONTROLLERAXISMOTION :
        break;

      case SDL_CONTROLLERBUTTONDOWN :
      case SDL_CONTROLLERBUTTONUP :
        break;

      case SDL_USEREVENT :
        break;

      case SDL_DISPLAYEVENT :

      case SDL_SYSWMEVENT :

      case SDL_CONTROLLERDEVICEADDED :
      case SDL_CONTROLLERDEVICEREMOVED :
      case SDL_CONTROLLERDEVICEREMAPPED :

      case SDL_JOYAXISMOTION :
      case SDL_JOYBALLMOTION :
      case SDL_JOYHATMOTION :
      case SDL_JOYBUTTONDOWN :
      case SDL_JOYBUTTONUP :

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


  void InputManager::addInputMap( InputEventType event_type, InputMapping* mapping )
  {
    if ( _inputMaps[ event_type ] != nullptr )
    {
      delete _inputMaps[ event_type ];
      _inputMaps[ event_type ] = nullptr;
    }

    _inputMaps[ event_type ] = mapping;
  }


  void InputManager::registerInputRequest( Controllable* object, InputEvent event )
  {
    INFO_STREAM << "Registered input request for event: " << event << " " << object;
    _eventMaps[event].insert( object );
  }


  ControllableSet& InputManager::getRegisteredObjects( InputEvent event )
  {
    return _eventMaps[event];
  }


  void InputManager::registerBehaviour( InputEventType event_type, unsigned int code, InputBehaviour event )
  {
    _inputMaps[ event_type ]->registerBehaviour( code, event );
  }


  InputBehaviour InputManager::getRegisteredBehaviour( InputEventType event_type, unsigned int code )
  {
    return _inputMaps[ event_type ]->getRegisteredBehaviour( code );
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Input Handler

  InputHandler::InputHandler() :
    _actionMaps()
  {
  }


  void InputHandler::registerInputRequest( Controllable* object, InputAction action )
  {
    INFO_STREAM << "Registered input request for action: " << action << " " << object;
    _actionMaps[action].insert( object );
  }


}

