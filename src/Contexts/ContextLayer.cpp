
#include "Regolith/Contexts/ContextLayer.h"

#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/Contexts/Context.h"


namespace Regolith
{

  ContextLayer::ContextLayer() :
    _owner( nullptr ),
    _name( "" ),
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


  void ContextLayer::configure( Context* owner, std::string name, Vector pos, Vector move_scale, float width, float height )
  {
    _owner = owner;
    _name = name;
    _position = pos;
    _movementScale = move_scale;
    _width = width;
    _height = height;
  }

}

