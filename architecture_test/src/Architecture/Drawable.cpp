
#include "Architecture/Drawable.h"

namespace Regolith
{
  namespace Interfaces
  {


    Drawable::Drawable() :
      PhysicalObject(),
      _theRenderer( nullptr )
    {
    }


    Drawable::Drawable( SDL_Renderer* r ) :
      PhysicalObject(),
      _theRenderer( r )
    {
    }

  }
}

