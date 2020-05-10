
#ifndef REGOLITH_ARCHITECTURE_COMPONENT_H_
#define REGOLITH_ARCHITECTURE_COMPONENT_H_

#include "Regolith/Global/Global.h"


namespace Regolith
{

  // Forward declarations
  class InputManager;


  namespace Architecture
  {

    /*
     * This class defines the interface for all regolith components, and enables the functionality to receive Regolith events.
     */
    class Component
    {
      Component( const Component& ) = delete;
      Conponent& operator=( const Component& ) = delete;

      public :
        // Make the detructor virtual
        virtual ~Component() {}

        // Register game-wide events with the manager
        virtual void registerEvents( InputManager* ) const = 0;

        // Regolith events
        virtual void eventAction( const RegolithEvent&, const SDL_Event& ) = 0;

    };

  }
}

#endif // REGOLITH_ARCHITECTURE_COMPONENT_H_

