
#include "Regolith/Architecture/ContextLayer.h"


namespace Regolith
{

  ContextLayer::ContextLayer( Vector pos, float w, float h ) :
    _camera(),
    _position( pos ),
    drawables(),
    moveables(),
    teams()
  {
    _camera.configure( w, h );
  }


  ContextLayer::~ContextLayer()
  {
    drawables.clear();

    moveables.clear();

    for ( TeamMap::iterator it = teams.begin(); it != teams.end(); ++it )
    {
      it->second.clear();
    }
    teams.clear();
  }

}

