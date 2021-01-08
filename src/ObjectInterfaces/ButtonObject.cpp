
#include "Regolith/ObjectInterfaces/ButtonObject.h"
#include "Regolith/Handlers/FocusHandler.h"


namespace Regolith
{

  void ButtonObject::registerFocus( FocusHandler* handler )
  {
    _handler = handler;
    _handler->addObject( this );
  }

  void ButtonObject::requestFocus()
  {
    _handler->requestFocus( this );
  }

}

