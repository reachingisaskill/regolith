
#ifndef __REGOLITH__INPUT_MAPPING_H__
#define __REGOLITH__INPUT_MAPPING_H__

#include "Definitions.h"

#include <map>


namespace Regolith
{

  class Controllable;

  /*
   * Stores the lookup array for O(1) determination of action
   * Some useful sizes:
   * SDL_NUM_SCANCODES = Max number of keyboard keys
   * SDL_CONTROLLER_BUTTON_MAX = max number of controller button inputs
   */

  class InputMapping
  {
    public :
      virtual ~InputMapping() {}

      virtual void registerAction( unsigned int, InputAction ) = 0;

      virtual InputAction& getAction( SDL_Event& ) = 0;

      virtual InputAction getRegisteredAction( unsigned int ) const = 0;

      virtual void propagate( Controllable*  ) const = 0;
  };

////////////////////////////////////////////////////////////////////////////////
  // Keyboard/Button Mapping class

  class KeyboardMapping : public InputMapping
  {
    private:
      InputAction _theMap[SDL_NUM_SCANCODES];
      InputAction _lastAction;
      bool _lastValue;

    public:
      KeyboardMapping();

      virtual ~KeyboardMapping();

      void registerAction( unsigned int, InputAction );

      InputAction& getAction( SDL_Event& event );

      InputAction getRegisteredAction( unsigned int ) const;

      void propagate( Controllable* ) const;
  };


////////////////////////////////////////////////////////////////////////////////
  // Controller Axis Mapping class

//  class ControllerAxisMapping : public InputMapping
//  {
//    private:
//      InputAction* _theMap; // Map for all controller joysticks!
//      InputAction _lastAction;
//      float _lastPosition;
//
//    public:
//      ControllerAxisMapping( unsigned int num = SDL_CONTROLLER_AXIS_MAX );
//
//      void registerAction( unsigned int, InputAction );
//
//      InputAction& getAction( SDL_Event& );
//
//      void propagate( Controllable* ) const;
//  };
  

////////////////////////////////////////////////////////////////////////////////
  // Mouse Mapping class

//  class MouseMapping : public Mapping_base
//  {
//    private:
//      InputAction _theAction; // Map for all joysticks!
//      Vector _lastPosition;
//
//    public:
//      MouseMapping() {}
//
//      void registerAction( unsigned int, InputAction );
//
//      InputAction& getAction( SDL_Event& ) const { return INPUT_NULL; }
//
//      void propagate( Controllable* ) const { }
//  };
  
}

#endif // __REGOLITH__INPUT_MAPPING_H__

