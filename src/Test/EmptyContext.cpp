
#include "Regolith/Test/EmptyContext.h"


namespace Regolith
{

  EmptyContext::EmptyContext() :
    Context(),
    _death()
  {
    DEBUG_LOG( "EmptyContext::EmptyContext : Empty Context Created" );
  }


  EmptyContext::~EmptyContext()
  {
    DEBUG_LOG( "EmptyContext::~EmptyContext : Empty Context Destroyed" );
  }


  void EmptyContext::onStart()
  {
    _death.configure( 10000, 1 );
  }


  void EmptyContext::updateContext( float time )
  {
    if ( _death.trigger( time ) )
    {
      this->stopContext();
    }
  }


}

