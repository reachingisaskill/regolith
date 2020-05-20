
#include "Regolith/Architecture/ContextLayer.h"


namespace Regolith
{

  ContextLayer::ContextLayer( Vector pos, Vector move_scale, float width, float height ) :
    _position( pos ),
    _movementScale( move_scale ),
    _width( width ),
    _height( height ),
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
      it->second.clear();
    }
    teams.clear();
  }

}

