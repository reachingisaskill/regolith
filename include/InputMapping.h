
#ifndef __REGOLITH__INPUT_MAPPING_H__
#define __REGOLITH__INPUT_MAPPING_H__

#include "Definitions.h"

#include <map>


namespace Regolith
{

  class Drawable;

  /*
   * Stores the lookup array for O(1) determination of action
   * Some useful sizes:
   * SDL_NUM_SCANCODES = Max number of keyboard keys
   * SDL_CONTROLLER_BUTTON_MAX = max number of controller button inputs
   */

  class InputMapping_base
  {
    public :
      virtual ~InputMapping_base() {}

      virtual void registerAction( unsigned int, InputAction ) = 0;

      virtual InputAction& getAction( SDL_Event& ) = 0;

      virtual void propagate( Drawable*, InputAction&, SDL_Event&  ) const = 0;
  };

////////////////////////////////////////////////////////////////////////////////
  // Keyboard/Button Mapping class

  class KeyboardMapping : public InputMapping_base
  {
    private:
      InputAction* _theMap;
      InputAction _lastAction;
      bool _lastValue;

    public:
      KeyboardMapping( unsigned int size = SDL_NUM_SCANCODES );

      void registerAction( unsigned int, InputAction );

      InputAction& getAction( SDL_Event& event );

      void propagate( Drawable* ) const;
  };


////////////////////////////////////////////////////////////////////////////////
  // Controller Axis Mapping class

  class ControllerAxisMapping : public InputMapping_base
  {
    private:
      InputAction* _theMap; // Map for all controller joysticks!
      InputAction _lastAction;
      float _lastPosition;

    public:
      ControllerAxisMapping( unsigned int num = SDL_CONTROLLER_AXIS_MAX );

      void registerAction( unsigned int, InputAction );

      InputAction& getAction( SDL_Event& );

      void propagate( Drawable* ) const;
  };
  

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
//      void propagate( Drawable* ) const { }
//  };
  
}

#endif // __REGOLITH__INPUT_MAPPING_H__

