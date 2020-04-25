
#include "InputHandler.h"

#include "logtastic.h"


namespace Regolith
{

  InputHandler::InputHandler() :
    _inputMaps(),
    _actionMaps(),
    _theEvent()
  {
    for ( size_t i = 0; i < INPUT_EVENT_TOTAL; ++i )
    {
      _inputMaps[ i ] = nullptr; 
    }

    _inputMaps[ INPUT_EVENT_KEYBOARD ] = new KeyboardMapping();
  }


  InputHandler::~InputHandler()
  {
    for ( size_t i = 0; i < INPUT_EVENT_TOTAL; ++i )
    {
      if ( _inputMaps[ i ] != nullptr )
      {
        delete _inputMaps[ i ];
        _inputMaps[ i ] = nullptr;
      }
    }
  }


  void InputHandler::handleEvents()
  {
    while ( SDL_PollEvent( &_theEvent ) != 0 )
    {
      this->handleEvent( _theEvent );
    }
  }


  void InputHandler::handleEvent( SDL_Event& event )
  {
    // At this point we must assume ALL of the mapping objects are correctly configured!

    InputEventType event_type = INPUT_EVENT_NULL;
    InputMapping* mapper = nullptr;
    InputAction action = INPUT_ACTION_NULL;

    DrawableSet::iterator end;

    DEBUG_STREAM << "Handling event";

    switch ( event.type )
    {
      case SDL_WINDOWEVENT :
        break;

      case SDL_KEYDOWN :
      case SDL_KEYUP :
        DEBUG_LOG( "  Keyboard key-press type event" );
        event_type = INPUT_EVENT_KEYBOARD;
        mapper = _inputMaps[ event_type ];
        action = mapper->getAction( event );
        if ( action == INPUT_ACTION_NULL ) break;

        end = _actionMaps[action].end();
        for ( DrawableSet::iterator it = _actionMaps[action].begin(); it != end; ++it )
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
        // Not logic for these event types yet.
        break;
    }
  }


  void InputHandler::addInputMap( InputEventType event_type, InputMapping* mapping )
  {
    if ( _inputMaps[ event_type ] != nullptr )
    {
      delete _inputMaps[ event_type ];
      _inputMaps[ event_type ] = nullptr;
    }

    _inputMaps[ event_type ] = mapping;
  }


  void InputHandler::registerInputRequest( Controllable* object, InputAction action )
  {
    INFO_STREAM << "Registered input request for action: " << action;
    _actionMaps[action].insert( object );
  }


  InputHandler::DrawableSet InputHandler::getRegisteredObjects( InputAction action )
  {
    return _actionMaps[action];
  }


  void InputHandler::registerAction( InputEventType event, unsigned int code, InputAction action )
  {
    _inputMaps[ event ]->registerAction( code, action );
  }


  InputAction InputHandler::getRegisteredAction( InputEventType event_type, unsigned int code )
  {
    return _inputMaps[ event_type ]->getRegisteredAction( code );
  }

}

