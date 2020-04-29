
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

  typedef unsigned int InputBehaviour;

  class InputMapping
  {
    public :
      virtual ~InputMapping() {}

      virtual void registerBehaviour( unsigned int, InputBehaviour ) = 0;

      virtual InputBehaviour& getBehaviour( SDL_Event& ) = 0;

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

      InputBehaviour& getBehaviour( SDL_Event& event );

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
//      InputBehaviour& getBehaviour( SDL_Event& );
//
//      void propagate( Controllable* ) const;
//  };
  

////////////////////////////////////////////////////////////////////////////////
  // Mouse Mapping class

//  class MouseMapping : public Mapping_base
//  {
//    private:
//      InputBehaviour _theBehaviour; // Map for all joysticks!
//      Vector _lastPosition;
//
//    public:
//      MouseMapping() {}
//
//      void registerBehaviour( unsigned int, InputBehaviour );
//
//      InputBehaviour& getBehaviour( SDL_Event& ) const { return INPUT_NULL; }
//
//      void propagate( Controllable* ) const { }
//  };
  
}

#endif // __REGOLITH__INPUT_MAPPING_H__

