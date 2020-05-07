
#ifndef REGOLITH_CONTROLLABLE_H_
#define REGOLITH_CONTROLLABLE_H_

#include "Components/Vector.h"


namespace Regolith
{

  // Forward declarations
  class InputManager;
  class InputHandler;


  /*
   * This class defines the interface for all objects that can receive input from events.
   * All classes (cameras, objects, windows, etc) that require this functionality must
   * publically inherit from this class.
   */
  class Controllable
  {
    public :
      // Make the detructor virtual
      virtual ~Controllable() {}

      // Register game-wide events with the manager
      virtual void registerEvents( InputManager* ) = 0;

      // Register context-wide actions with the handler
      virtual void registerActions( InputHandler* ) = 0;


      // Interfaces for input
      // Handled and mapped actions
      virtual void inputAction( const InputAction& ) {}
      virtual void booleanAction( const InputAction&, bool ) {}
      virtual void floatAction( const InputAction&, float ) {}
      virtual void vectorAction( const InputAction&, const Vector& ) {}
      virtual void mouseAction( const InputAction&, bool, const Vector& ) {}

      // Regolith events
      virtual void eventAction( const RegolithEvent&, const SDL_Event& ) {}

  };

}

#endif // REGOLITH_CONTROLLABLE_H_

