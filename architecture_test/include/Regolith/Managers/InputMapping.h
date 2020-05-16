
#ifndef REGOLITH_INPUT_MAPPING_H_
#define REGOLITH_INPUT_MAPPING_H_

#include "Regolith/Global/Global.h"

#include <map>


namespace Regolith
{

  // Constant to scale the axis inputs by
  const float AXIS_MAX_VALUE = 32767.0;

////////////////////////////////////////////////////////////////////////////////


  class ControllableInterface;

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

      virtual InputAction getAction( SDL_Event& ) = 0;

      virtual InputAction getRegisteredAction( unsigned int ) const = 0;

      virtual void propagate( ControllableInterface*  ) const = 0;
  };


////////////////////////////////////////////////////////////////////////////////
  // Keyboard Mapping class

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

      InputAction getAction( SDL_Event& event );

      InputAction getRegisteredAction( unsigned int ) const;

      void propagate( ControllableInterface* ) const;
  };


////////////////////////////////////////////////////////////////////////////////
  // Mouse movement Mapping class

  class MotionMapping : public InputMapping
  {
    private:
      InputAction _theAction;
      Vector _lastPosition;

    public:
      MotionMapping();

      virtual ~MotionMapping();

      void registerAction( unsigned int, InputAction );

      InputAction getAction( SDL_Event& event );

      InputAction getRegisteredAction( unsigned int ) const;

      void propagate( ControllableInterface* ) const;
  };


////////////////////////////////////////////////////////////////////////////////
  // Mouse button Mapping class

  class MouseMapping : public InputMapping
  {
    private:
      InputAction _theMap[NUMBER_MOUSE_BUTTONS]; // Map for 7 mouse buttons
      InputAction _lastAction;
      Vector _lastPosition;
      bool _lastValue;

    public:
      MouseMapping();

      ~MouseMapping();

      void registerAction( unsigned int, InputAction );

      InputAction getAction( SDL_Event& );

      InputAction getRegisteredAction( unsigned int ) const;

      void propagate( ControllableInterface* ) const;
  };


////////////////////////////////////////////////////////////////////////////////
  // Controller Axis Mapping class

  class ControllerAxisMapping : public InputMapping
  {
    private:
      InputAction _theMap[SDL_CONTROLLER_AXIS_MAX]; // Map for all controller joysticks!
      InputAction _lastAction;
      float _lastPosition;

    public:
      ControllerAxisMapping();

      void registerAction( unsigned int, InputAction );

      InputAction getAction( SDL_Event& );

      void propagate( ControllableInterface* ) const;
  };


////////////////////////////////////////////////////////////////////////////////
  // Controller Button Mapping class

  class ControllerButtonMapping : public InputMapping
  {
    private:
      InputAction _theMap[SDL_CONTROLLER_BUTTON_MAX];
      InputAction _lastAction;
      bool _lastValue;

    public:
      ControllerButtonMapping();

      void registerAction( unsigned int, InputAction );

      InputAction getAction( SDL_Event& );

      void propagate( ControllableInterface* ) const;
  };
  
}

#endif // REGOLITH_INPUT_MAPPING_H_

