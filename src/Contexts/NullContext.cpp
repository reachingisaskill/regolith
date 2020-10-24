
#include "Regolith/Contexts/NullContext.h"


namespace Regolith
{

  NullContext::NullContext() :
    Context()
  {
    INFO_LOG( "Null Context Created" );
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

