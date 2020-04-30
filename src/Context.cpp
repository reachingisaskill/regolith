
#include "Context.h"
#include "Manager.h"


namespace Regolith
{

  Context::Context() :
    _parent( nullptr ),
    _theInput(),
    _properties( CONTEXT_PROPERTIES_NONE )
  {
  }


  void Context::giveFocus()
  {
    Manager::getInstance()->pushContext( this );
  }


  void Context::takeFocus()
  {
    Manager::getInstance()->popContext();
  }

}

