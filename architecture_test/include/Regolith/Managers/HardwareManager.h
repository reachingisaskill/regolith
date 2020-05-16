
#ifndef REGOLITH_MANAGERS_HARWARE_MANAGER_H_
#define REGOLITH_MANAGERS_HARWARE_MANAGER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/Component.h"


namespace Regolith
{

  class HardwareManager : public Component
  {
    typedef std::map< Sint32, SDL_Joystick* > JoystickMap;
    typedef std::map< Sint32, SDL_GameController* > GameControllerMap;
    private:
      JoystickMap _joysticks;
      GameControllerMap _controllers;

    protected:
      void joystickAdd( Sint32 );
      void joystickRemove( Sint32 );

      void controllerAdd( Sint32 );
      void controllerRemove( Sint32 );

    public:
      HardwareManager();

      virtual ~HardwareManager();

      // Remove all the owned hardware objects
      void clear();

      // Register game-wide events with the manager
      virtual void registerEvents( InputManager& ) override;

      // Regolith events
      virtual void eventAction( const RegolithEvent&, const SDL_Event& ) override;
  };

}

#endif // REGOLITH_MANAGERS_HARWARE_MANAGER_H_

