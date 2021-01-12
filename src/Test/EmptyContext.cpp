
#include "Regolith/Test/EmptyContext.h"
#include "Regolith/GamePlay/Camera.h"

#include <cmath>


namespace Regolith
{

  EmptyContext::EmptyContext() :
    Context(),
    _death(),
    _fading( false ),
    _time( 0.0 ),
    _fadeTime( 2000 ),
    _fadeColour( { 0, 0, 0, 0 } )
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
    _fading = false;
  }


  void EmptyContext::updateContext( float time )
  {
    _time += time;

    if ( _death.trigger( time ) )
    {
      this->stopContext();
    }

    if ( _time > 8000 )
    {
      _fading = true;

      _fadeColour.a = std::min( 255*(_time-8000)/2000, 255.0f );

      DEBUG_STREAM << "EmptyContext::updateContext : Fade Alpha = " << (int)_fadeColour.a;
    }
  }


  void EmptyContext::renderContext( Camera& camera )
  {
    if ( _fading )
    {
      camera.fillWindow( _fadeColour );
    }
  }

}

