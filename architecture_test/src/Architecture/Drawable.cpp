
#include "Regolith/Architecture/Drawable.h"
#include "Regolith/Managers/Manager.h"


namespace Regolith
{

  Drawable::Drawable() :
    PhysicalObject(),
    _theRenderer( Manager::getInstance()->getRendererPointer() )
  {
  }

}

