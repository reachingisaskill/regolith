
#include "TestDrawable.h"

#include "logtastic.h"


namespace Regolith
{

  TestDrawable::TestDrawable() :
    _collisionCount( 2 ),
    _collision( { Collision(), Collision() } )
  {
    _collision[0] = Collision( Vector( 0.0, 0.0 ), 1.0, 1.0 );
    _collision[1] = Collision( Vector( 2.0, 1.0 ), 0.5, 2.0 );
  }


  unsigned int TestDrawable::getCollision( Collision*& c )
  {
    c = _collision;
    return _collisionCount;
  }

}

