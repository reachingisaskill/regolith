
#include "Context.h"
#include "Manager.h"


namespace Regolith
{

  void Context::giveFocus()
  {
    Manager::getInstance()->setContext( this );
  }

}

