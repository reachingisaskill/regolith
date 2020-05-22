
#ifndef REGOLITH_ARCHITECTURE_CONTROLLABLE_INTERFACE_H_
#define REGOLITH_ARCHITECTURE_CONTROLLABLE_INTERFACE_H_

#include "Regolith/Global/Global.h"


namespace Regolith
{

  // Forward declarations
  class InputHandler;

  /*
   * This class defines the interface for all objects that can receive user action corresponding to hardware inputs
   */
  class ControllableInterface
  {
    public :
      // Make the detructor virtual
      virtual ~ControllableInterface() {}

      // Register context-wide actions with the handler
      virtual void registerActions( InputHandler& ) = 0;

      // Interfaces for input
      // Handled and mapped actions
      virtual void inputAction( const InputAction& ) {}
      virtual void booleanAction( const InputAction&, bool ) {}
      virtual void floatAction( const InputAction&, float ) {}
      virtual void vectorAction( const InputAction&, const Vector& ) {}
      virtual void mouseAction( const InputAction&, bool, const Vector& ) {}

  };

}

#endif // REGOLITH_ARCHITECTURE_CONTROLLABLE_INTERFACE_H_

