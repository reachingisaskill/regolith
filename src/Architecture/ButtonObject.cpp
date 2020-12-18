#define LOGTASTIC_DEBUG_OFF

#include "Regolith/Architecture/ButtonObject.h"
#include "Regolith/Managers/FocusHandler.h"


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
