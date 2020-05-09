
#include "Architecture/Collidable.h"


namespace Regolith
{
  namespace Interfaces
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

