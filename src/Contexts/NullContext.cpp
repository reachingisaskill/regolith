
#include "Regolith/Contexts/NullContext.h"


namespace Regolith
{

  NullContext::NullContext() :
    Context()
  {
  }


  NullContext::~NullContext()
  {
  }


  void NullContext::configure( Json::Value& json_data, ContextGroup& handler )
  {
    // Call the base class variant first
    Context::configure( json_data, handler );

    // Do some stuff to waste time I guess?

  }

}

