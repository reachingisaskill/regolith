#define LOGTASTIC_DEBUG_OFF

#include "Regolith/Architecture/ButtonInterface.h"
#include "Regolith/Managers/FocusHandler.h"


namespace Regolith
{

  void ButtonInterface::registerFocus( FocusHandler* handler )
  {
    _handler = handler;
    _handler->addObject( this );
  }

  void ButtonInterface::requestFocus()
  {
    _handler->requestFocus( this );
  }

}

