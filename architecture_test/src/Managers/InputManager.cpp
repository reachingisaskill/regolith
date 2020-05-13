
#include "Regolith/Managers/InputManager.h"
#include "Regolith/Managers/Manager.h"

#include "logtastic.h"


namespace Regolith
{

  InputManager::InputManager() :
    _inputMappers( "input_mapping_sets" ),
    _eventMaps(),
    _theEvent()
  {
  }


  InputManager::~InputManager()
  {
    INFO_LOG( "Clearing the input mappings" );
    _inputMappers.clear();
  }


  void InputManager::handleEvents( InputHandler* handler )
  {
    InputEventType event_type;
    InputMapping* mapper = nullptr;
    InputAction action;
    RegolithEvent event;

    ControllableSet::iterator end;
    ComponentSet::iterator components_end;

    DEBUG_STREAM << "Handling events";

    while ( SDL_PollEvent( &_theEvent ) != 0 )
    {
      switch ( _theEvent.type )
      {
        //////////////////////////////////////////////////
        // Input-type events
        case SDL_QUIT :
          event = REGOLITH_EVENT_QUIT;
          DEBUG_STREAM << "  Regolith Quit Event";
          components_end = this->getRegisteredComponents( event ).end();
          for ( ComponentSet::iterator it = this->getRegisteredComponents( event ).begin(); it != components_end; ++it )
          {
            DEBUG_STREAM << "  Propagating event : " << event;
            (*it)->eventAction( event, _theEvent );
          }
          break;

        case SDL_KEYDOWN :
        case SDL_KEYUP :
          if ( handler == nullptr ) break;

          DEBUG_LOG( "  Keyboard key-press type event" );
          event_type = INPUT_TYPE_KEYBOARD;
          mapper = handler->_inputMaps->mapping[ event_type ];
          action = mapper->getAction( _theEvent );
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
          mapper = handler->_inputMaps->mapping[ event_type ];
          action = mapper->getAction( _theEvent );
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
          mapper = handler->_inputMaps->mapping[ event_type ];
          action = mapper->getAction( _theEvent );
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
          components_end = this->getRegisteredComponents( event ).end();
          for ( ComponentSet::iterator it = this->getRegisteredComponents( event ).begin(); it != components_end; ++it )
          {
            DEBUG_STREAM << "  Propagating event : " << event;
            (*it)->eventAction( event, _theEvent );
          }
          break;


        case SDL_USEREVENT :
          event = (RegolithEvent)_theEvent.user.code;
          DEBUG_STREAM << "  Regolith User Event " << event;

          if ( event == REGOLITH_EVENT_NULL ) break;
          components_end = this->getRegisteredComponents( event ).end();
          for ( ComponentSet::iterator it = this->getRegisteredComponents( event ).begin(); it != components_end; ++it )
          {
            DEBUG_STREAM << "  Propagating event : " << event;
            (*it)->eventAction( event, _theEvent );
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
  }


  void InputManager::registerEventRequest( Component* object, RegolithEvent event )
  {
    INFO_STREAM << "Registered input request for event: " << event << " " << object;
    _eventMaps[event].insert( object );
  }


  ComponentSet& InputManager::getRegisteredComponents( RegolithEvent event )
  {
    return _eventMaps[event];
  }


  void InputManager::registerInputAction( std::string mapping, InputEventType event_type, unsigned int code, InputAction event )
  {
    _inputMappers.get( mapping )->mapping[ event_type ]->registerAction( code, event );
  }


  InputAction InputManager::getRegisteredInputAction( std::string mapping, InputEventType event_type, unsigned int code )
  {
    return _inputMappers.get( mapping )->mapping[ event_type ]->getRegisteredAction( code );
  }


  InputMappingSet* InputManager::requestMapping( std::string name )
  {
    if ( ! _inputMappers.exists( name ) ) 
    {
      INFO_STREAM << "Creating new input mapping set: " << name;
      _inputMappers.addObject( new InputMappingSet(), name );
    }

    return _inputMappers.get( name );
  }


  void InputManager::simulateInputAction( InputHandler* handler, InputAction action )
  {
    DEBUG_LOG( "Simulating Input Action" );
    if ( action == INPUT_ACTION_NULL ) return;

    ControllableSet::iterator end = handler->getRegisteredObjects( action ).end();
    for ( ControllableSet::iterator it = handler->getRegisteredObjects( action ).begin(); it != end; ++it )
    {
      DEBUG_STREAM << "  Propagating action : " << action;
      (*it)->inputAction( action );
    }
  }


  void InputManager::simulateBooleanAction( InputHandler* handler, InputAction action, bool val )
  {
    DEBUG_LOG( "Simulating Boolean Action" );
    if ( action == INPUT_ACTION_NULL ) return;

    ControllableSet::iterator end = handler->getRegisteredObjects( action ).end();
    for ( ControllableSet::iterator it = handler->getRegisteredObjects( action ).begin(); it != end; ++it )
    {
      DEBUG_STREAM << "  Propagating action : " << action;
      (*it)->booleanAction( action, val );
    }
  }


  void InputManager::simulateFloatAction( InputHandler* handler, InputAction action, float val )
  {
    DEBUG_LOG( "Simulating Float Action" );
    if ( action == INPUT_ACTION_NULL ) return;

    ControllableSet::iterator end = handler->getRegisteredObjects( action ).end();
    for ( ControllableSet::iterator it = handler->getRegisteredObjects( action ).begin(); it != end; ++it )
    {
      DEBUG_STREAM << "  Propagating action : " << action;
      (*it)->floatAction( action, val );
    }
  }


  void InputManager::simulateVectorAction( InputHandler* handler, InputAction action, const Vector& val )
  {
    DEBUG_LOG( "Simulating Vector Action" );
    if ( action == INPUT_ACTION_NULL ) return;

    ControllableSet::iterator end = handler->getRegisteredObjects( action ).end();
    for ( ControllableSet::iterator it = handler->getRegisteredObjects( action ).begin(); it != end; ++it )
    {
      DEBUG_STREAM << "  Propagating action : " << action;
      (*it)->vectorAction( action, val );
    }
  }


  void InputManager::simulateMouseAction( InputHandler* handler, InputAction action, bool click, const Vector& vec )
  {
    DEBUG_LOG( "Simulating Mouse Action" );
    if ( action == INPUT_ACTION_NULL ) return;

    ControllableSet::iterator end = handler->getRegisteredObjects( action ).end();
    for ( ControllableSet::iterator it = handler->getRegisteredObjects( action ).begin(); it != end; ++it )
    {
      DEBUG_STREAM << "  Propagating action : " << action;
      (*it)->mouseAction( action, click, vec );
    }
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Input Mapping Set

  InputMappingSet::InputMappingSet() :
    mapping()
  {
    for ( size_t i = 0; i < INPUT_TYPE_TOTAL; ++i )
    {
      mapping[ i ] = nullptr; 
    }

    mapping[ INPUT_TYPE_KEYBOARD ] = new KeyboardMapping();
    mapping[ INPUT_TYPE_MOUSE_BUTTON ] = new MouseMapping();
    mapping[ INPUT_TYPE_MOUSE_MOVE ] = new MotionMapping();
  }


  InputMappingSet::~InputMappingSet()
  {
    for ( size_t i = 0; i < INPUT_TYPE_TOTAL; ++i )
    {
      if ( mapping[ i ] != nullptr )
      {
        delete mapping[ i ];
        mapping[ i ] = nullptr;
      }
    }
  }


//////////////////////////////////////////////////////////////////////////////////////////////////
  // Input Manager Init

  void InputManager::configure( Json::Value& json_data )
  {
    Utilities::validateJson( json_data, "require", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJson( json_data, "keymappings", Utilities::JSON_TYPE_ARRAY );


    // Configure the input objects
    Json::Value required = json_data["require"];
    Json::Value keymaps = json_data["keymappings"];

    // Quick vallidation - TODO: add a hardware check function to makesure hardware actually exists
    Json::ArrayIndex required_size = required.size();
    for ( Json::ArrayIndex i = 0; i != required_size; ++i )
    {
      if ( required[i].asString() == "controller" )
      {
        WARN_LOG( "Controllers are not yet supported!" );
      }
      else if ( required[i].asString() == "joystick" )
      {
        WARN_LOG( "Joystick is not yet supported!" );
      }
    }


    Json::ArrayIndex keymaps_size = required.size();
    for ( Json::ArrayIndex i = 0; i != keymaps_size; ++i )
    {
      Utilities::validateJson( keymaps[i], "name", Utilities::JSON_TYPE_STRING );
      Utilities::validateJson( keymaps[i], "keymapping", Utilities::JSON_TYPE_ARRAY );

      std::string mapping_name = keymaps[i]["name"].asString();
      Json::Value keymapping = keymaps[i]["keymapping"];

      // Create the an empty mapping object
      requestMapping( mapping_name );

      // Loop over the key mapping hardware
      Json::ArrayIndex keymapping_size = keymapping.size();
      for ( Json::ArrayIndex j = 0; j != keymapping_size; ++j )
      {
        Utilities::validateJson( keymapping[j], "type", Utilities::JSON_TYPE_STRING );
        Utilities::validateJson( keymapping[j], "mapping", Utilities::JSON_TYPE_OBJECT );

        std::string type = keymapping[j]["type"].asString();
        Json::Value keys = keymapping[j]["mapping"];

        if ( type == "keyboard" )
        {
          INFO_LOG( "Loading Keyboard Mapping." );

          Json::Value::const_iterator keys_end = keys.end();
          for ( Json::Value::const_iterator it = keys.begin(); it != keys_end; ++it )
          {
            SDL_Scancode code = getScancodeID( it.key().asString() );
            InputAction action = getActionID( it->asString() );
            registerInputAction( mapping_name, INPUT_TYPE_KEYBOARD, code, action );
            INFO_STREAM << "Key Registered to map: " << mapping_name << " -- " << it.key().asString() << "(" << code << ")" << " as action : " << it->asString() << "(" << action << ")";
          }
        }

        else if ( type == "mouse_buttons" )
        {
          INFO_LOG( "Loading Mouse Button Mapping." );

          Json::Value::const_iterator keys_end = keys.end();
          for ( Json::Value::const_iterator it = keys.begin(); it != keys_end; ++it )
          {
            MouseButton code = getMouseButtonID( it.key().asString() );
            InputAction action = getActionID( it->asString() );
            registerInputAction( mapping_name, INPUT_TYPE_MOUSE_BUTTON, code, action );
            INFO_STREAM << "Mouse Button Registered to map: " << mapping_name << " -- " << it.key().asString() << "(" << code << ")" << " as action : " << it->asString() << "(" << action << ")";
          }
        }

        else if ( type == "mouse_movement" )
        {
          INFO_LOG( "Loading Mouse Movement." );

          // Only require the movement input - it must be present if a mouse movement mapping is provided
          Utilities::validateJson( keys, "movement", Utilities::JSON_TYPE_STRING );

          unsigned code = 0; // Dummy variable - only one thing to map with mouse movement!
          InputAction action = getActionID( keys["movement"].asString() );
          registerInputAction( mapping_name, INPUT_TYPE_MOUSE_MOVE, code, action );
          INFO_STREAM << "Registered : mouse movement as action to map: " << mapping_name << " -- " << keys["movement"].asString() << "(" << action << ")";
        }
        else
        {
          WARN_LOG( "Key mapping specified for an unsupported interface." );
          WARN_LOG( "Please try again with a future version." );
        }
      }
    }
  }
}

