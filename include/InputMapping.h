
#ifndef REGOLITH_INPUT_MAPPING_H_
#define REGOLITH_INPUT_MAPPING_H_

#include "Definitions.h"
#include "Vector.h"

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

  typedef unsigned int InputBehaviour;

  class InputMapping
  {
    public :
      virtual ~InputMapping() {}

      virtual void registerBehaviour( unsigned int, InputBehaviour ) = 0;

      virtual InputBehaviour getBehaviour( SDL_Event& ) = 0;

      virtual InputBehaviour getRegisteredBehaviour( unsigned int ) const = 0;

      virtual void propagate( Controllable*  ) const = 0;
  };


////////////////////////////////////////////////////////////////////////////////
  // Keyboard/Button Mapping class

  class KeyboardMapping : public InputMapping
  {
    private:
      InputBehaviour _theMap[SDL_NUM_SCANCODES];
      InputBehaviour _lastBehaviour;
      bool _lastValue;

    public:
      KeyboardMapping();

      virtual ~KeyboardMapping();

      void registerBehaviour( unsigned int, InputBehaviour );

      InputBehaviour getBehaviour( SDL_Event& event );

      InputBehaviour getRegisteredBehaviour( unsigned int ) const;

      void propagate( Controllable* ) const;
  };


////////////////////////////////////////////////////////////////////////////////
  // Keyboard/Button Mapping class

  class MotionMapping : public InputMapping
  {
    private:
      InputBehaviour _theBehaviour;
      Vector _lastPosition;

    public:
      MotionMapping();

      virtual ~MotionMapping();

      void registerBehaviour( unsigned int, InputBehaviour );

      InputBehaviour getBehaviour( SDL_Event& event );

      InputBehaviour getRegisteredBehaviour( unsigned int ) const;

      void propagate( Controllable* ) const;
  };


////////////////////////////////////////////////////////////////////////////////
  // Controller Axis Mapping class

//  class ControllerAxisMapping : public InputMapping
//  {
//    private:
//      InputBehaviour* _theMap; // Map for all controller joysticks!
//      InputBehaviour _lastBehaviour;
//      float _lastPosition;
//
//    public:
//      ControllerAxisMapping( unsigned int num = SDL_CONTROLLER_AXIS_MAX );
//
//      void registerBehaviour( unsigned int, InputBehaviour );
//
//      InputBehaviour getBehaviour( SDL_Event& );
//
//      void propagate( Controllable* ) const;
//  };
  

////////////////////////////////////////////////////////////////////////////////
  // Mouse Mapping class

  class MouseMapping : public InputMapping
  {
    private:
      InputBehaviour _theMap[14]; // Map for 7 mouse buttons, (up+down)
      InputBehaviour _lastBehaviour;
      Vector _lastPosition;
      bool _lastValue;

    public:
      MouseMapping();

      ~MouseMapping();

      void registerBehaviour( unsigned int, InputBehaviour );

      InputBehaviour getBehaviour( SDL_Event& );

      InputBehaviour getRegisteredBehaviour( unsigned int ) const;

      void propagate( Controllable* ) const;
  };
  
}

#endif // REGOLITH_INPUT_MAPPING_H_

