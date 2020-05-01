
#ifndef REGOLITH_CONTROLLABLE_H_
#define REGOLITH_CONTROLLABLE_H_

#include "Vector.h"


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
      virtual void booleanAction( const InputAction&, bool ) = 0;
      virtual void floatAction( const InputAction&, float ) = 0;
      virtual void vectorAction( const InputAction&, const Vector& ) = 0;

      // Regolith events
      virtual void eventAction( const RegolithEvent&, const SDL_Event& ) = 0;

  };

}

#endif // REGOLITH_CONTROLLABLE_H_

