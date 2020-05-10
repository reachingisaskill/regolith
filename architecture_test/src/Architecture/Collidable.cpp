
#include "Architecture/Collidable.h"


namespace Regolith
{
  namespace Architecture
  {

    Collidable::Collidable() :
      PhysicalObject(),
      _collision(),
      _collisionActive( false ),
      _collisionType( 0 )
    {
    }

  }
}

