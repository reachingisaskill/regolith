
#include "Regolith/Architecture/ContextLayer.h"


namespace Regolith
{

  ContextLayer::ContextLayer( Vector pos, float w, float h, Vector move_scale ) :
    _camera(),
    _position( pos ),
    drawables(),
    moveables(),
    teams()
  {
    _camera.configure( w, h, move_scale );
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

