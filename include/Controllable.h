
#ifndef __REGOLITH__CONTROLLABLE_H__
#define __REGOLITH__CONTROLLABLE_H__

#include "Vector.h"


namespace Regolith
{

  class InputHandler;


  /*
   * This class defines the interface for all objects that can receive input from events.
   * All classes (cameras, objects, windows, etc) that require this functionality must
   * publically inherit from this class.
   */
  class Controllable
  {
    public :


      // Update the objects behaviour based on the provided timestep
      virtual void registerEvents( InputHandler* ) = 0;


      // Interfaces for input
      virtual void eventAction( const InputAction& ) = 0;
      virtual void booleanAction( const InputAction&, bool ) = 0;
      virtual void floatAction( const InputAction&, float ) = 0;
      virtual void vectorAction( const InputAction&, const Vector& ) = 0;

  };

}

#endif // __REGOLITH__CONTROLLABLE_H__

