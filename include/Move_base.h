
#ifndef __REGOLITH__MOVE_BASE_H__
#define __REGOLITH__MOVE_BASE_H__

#include "Vector.h"


namespace Regolith
{

  class Move_base
  {
    private:
      float _mass;
      Vector _position;
      Vector _velocity;

    public:
      Move_base( float mass = 1.0f );

      // Update positions and velocities with a timesstep
      void update( float );


  };

}


#endif // __REGOLITH__MOVE_BASE_H__

