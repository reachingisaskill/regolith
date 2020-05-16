
#include "Regolith/Managers/HardwareManager.h"
#include "Regolith/Managers/InputManager.h"

#include "logtastic.h"


namespace Regolith
{

  HardwareManager::HardwareManager() :
    Component(),
    _joysticks(),
    _controllers()
  {
  }


  HardwareManager::~HardwareManager()
  {
  }


  void HardwareManager::clear()
  {
    INFO_LOG( "Closing Joysticks" );
    for ( JoystickMap::iterator it = _joysticks.begin(); it != _joysticks.end(); ++it )
    {
      SDL_JoystickClose( it->second );
    }
    _joysticks.clear();

    INFO_LOG( "Closing Game Controllers" );
    for ( GameControllerMap::iterator it = _controllers.begin(); it != _controllers.end(); ++it )
    {
      SDL_GameControllerClose( it->second );
    }
    _controllers.clear();
  }


  void HardwareManager::joystickAdd( Sint32 id )
  {
    INFO_STREAM << "Joystick added: " << id;
    SDL_Joystick* joystick = SDL_JoystickOpen( id );
    _joysticks[id] = joystick;
  }


  void HardwareManager::joystickRemove( Sint32 id )
  {
    INFO_STREAM << "Joystick removed: " << id;
    _joysticks.erase( _joysticks.find( id ) );
  }


  void HardwareManager::controllerAdd( Sint32 id )
  {
    INFO_STREAM << "Controller added: " << id;
    SDL_GameController* controller = SDL_GameControllerOpen( id );
    _controllers[id] = controller;
  }


  void HardwareManager::controllerRemove( Sint32 id )
  {
    INFO_STREAM << "Controller removed: " << id;
    _controllers.erase( _controllers.find( id ) );
  }


  void HardwareManager::registerEvents( InputManager& manager )
  {
    manager.registerEventRequest( this, REGOLITH_EVENT_JOYSTICK_HARDWARE );
    manager.registerEventRequest( this, REGOLITH_EVENT_CONTROLLER_HARDWARE );
    manager.registerEventRequest( this, REGOLITH_EVENT_AUDIO_HARDWARE );
  }


  void HardwareManager::eventAction( const RegolithEvent& regolith_event, const SDL_Event& sdl_event )
  {
    switch ( regolith_event )
    {
      case REGOLITH_EVENT_JOYSTICK_HARDWARE :
        if ( sdl_event.jdevice.type == SDL_JOYDEVICEREMOVED )
        {
          joystickRemove( sdl_event.jdevice.which );
        }
        else
        {
          joystickAdd( sdl_event.jdevice.which );
        }
        break;

      case REGOLITH_EVENT_CONTROLLER_HARDWARE :
        switch ( sdl_event.cdevice.type )
        {
          case SDL_CONTROLLERDEVICEREMOVED :
            controllerRemove( sdl_event.cdevice.which );
            break;

          case SDL_CONTROLLERDEVICEADDED :
            controllerAdd( sdl_event.cdevice.which );
            break;

          case SDL_CONTROLLERDEVICEREMAPPED :
          // Do something?
            break;
        }
        break;

      case REGOLITH_EVENT_AUDIO_HARDWARE :
        break;

      default :
        break;
    }
  }

}

