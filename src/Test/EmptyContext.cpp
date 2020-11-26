
#include "Regolith/Test/EmptyContext.h"


namespace Regolith
{

  EmptyContext::EmptyContext() :
    Context()
  {
    DEBUG_LOG( "EmptyContext::EmptyContext : Empty Context Created" );
  }


  EmptyContext::~EmptyContext()
  {
    DEBUG_LOG( "EmptyContext::~EmptyContext : Empty Context Destroyed" );
  }

}

