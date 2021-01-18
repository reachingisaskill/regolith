
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
    _fadeTrigger( 10000 ),
    _fadeTime( 2000 ),
    _fadeColour( { 0, 0, 0, 0 } ),
    _kineticEnergy( 0.0 )
  {
    DEBUG_LOG( "EmptyContext::EmptyContext : Empty Context Created" );
  }


  EmptyContext::~EmptyContext()
  {
    DEBUG_LOG( "EmptyContext::~EmptyContext : Empty Context Destroyed" );
  }


  void EmptyContext::onStart()
  {
    _death.configure( _fadeTrigger+_fadeTime, 1 );
    _fading = false;
  }


  void EmptyContext::updateContext( float time )
  {
    _time += time;

    if ( _death.trigger( time ) )
    {
      this->stopContext();
    }

    if ( _time > _fadeTrigger )
    {
      _fading = true;

      _fadeColour.a = std::min( 255*(_time-_fadeTrigger)/_fadeTime, 255.0f );

      DEBUG_STREAM << "EmptyContext::updateContext : Fade Alpha = " << (int)_fadeColour.a;
    }

    INFO_STREAM << "EmptyContext::updateContext : Total Kinetic Energy = " << _kineticEnergy;
    _kineticEnergy = 0.0;
  }


  void EmptyContext::renderContext( Camera& camera )
  {
    if ( _fading )
    {
      camera.fillWindow( _fadeColour );
    }
  }

  void EmptyContext::updatePhysics( PhysicalObject* object, float /*time*/ ) const
  {
    _kineticEnergy += 0.5 * object->getMass() * object->getVelocity().square();
    _kineticEnergy += 0.5 * object->getInertia() * object->getAngularVelocity() * object->getAngularVelocity();
  }
}

