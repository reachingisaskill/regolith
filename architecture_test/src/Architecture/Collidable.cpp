
#include "Regolith/Architecture/Collidable.h"


namespace Regolith
{

  Collidable::Collidable() :
    PhysicalObject(),
    _collision(),
    _collisionActive( false ),
    _collisionType( 0 )
  {
  }

}

