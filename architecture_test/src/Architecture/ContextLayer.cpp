
#include "Architecture/ContextLayer.h"


namespace Regolith
{
  namespace Interfaces
  {

    ContextLayer::ContextLayer( Vector pos, float w, float h ) :
      _position( pos ),
      _width( w ),
      _height( h ),
      drawables(),
      moveables(),
      teams()
    {
    }


    ContextLayer::~ContextLayer()
    {
      drawables.clear();

      moveables.clear();

      for ( TeamMap::iterator it = teams.begin(); it != teams.end(); ++it )
      {
        it->clear();
      }
      teams.clear();
    }

  }
}

