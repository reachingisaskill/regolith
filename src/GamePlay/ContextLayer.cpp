
#include "Regolith/GamePlay/ContextLayer.h"

#include "Regolith/Architecture/ControllableInterface.h"
#include "Regolith/GamePlay/Context.h"
#include "Regolith/GamePlay/PhysicalObject.h"


namespace Regolith
{

  ContextLayer::ContextLayer() :
    _owner( nullptr ),
    _position( 0.0 ),
    _movementScale( 0.0 ),
    _width( 0.0 ),
    _height( 0.0 ),
    layerGraph()
  {
  }


  ContextLayer::~ContextLayer()
  {
    layerGraph.clear();
  }


  void ContextLayer::configure( Context* owner, Vector pos, Vector move_scale, float width, float height )
  {
    _owner = owner;
    _position = pos;
    _movementScale = move_scale;
    _width = width;
    _height = height;
  }

}

