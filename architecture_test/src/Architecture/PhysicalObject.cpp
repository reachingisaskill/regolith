
#include "Architecture/PhysicalObject.h"

namespace Regolith
{
  namespace Architecture
  {

    PhysicalObject::PhysicalObject() :
      _destroyMe( false ),
      _position(),
      _width(),
      _height(),
      _rotation()
    {
    }


    PhysicalObject::PhysicalObject( Vector pos, float w, float h, float r ) :
      _destroyMe( false ),
      _position( pos ),
      _width( w ),
      _height( h ),
      _rotation( r )
    {
    }

  }
}

