
#include "Regolith/Test/FPSString.h"
#include "Regolith/Managers/ContextGroup.h"


namespace Regolith
{

  FPSString::FPSString() :
    _texture(),
    _defaultString( "Calculating..." ),
    _string( _defaultString ),
    _timeElapsed( 0.0 ),
    _numberFrames( 0 )
  {
  }


  FPSString::~FPSString()
  {
  }


  void FPSString::configure( Json::Value& json_data, ContextGroup& cg )
  {
    PhysicalObject::configure( json_data, cg );

    _texture.configure( json_data, cg.getDataHandler() );

    _texture.writeText( _defaultString );

    this->setWidth( _texture.getWidth() );
    this->setHeight( _texture.getHeight() );

    DEBUG_LOG( "FPSString::configure : FPS string object configured" );
  }


  void FPSString::reset()
  {
    _timeElapsed = 0.0;
    _numberFrames = 0;

    _texture.writeText( _defaultString );

    this->setWidth( _texture.getWidth() );
    this->setHeight( _texture.getHeight() );
  }


  void FPSString::update( float time )
  {
    _timeElapsed += time;

    if ( ++_numberFrames > 100 )
    {
      float fps = 1000.0 * (float) _numberFrames / _timeElapsed;

      std::stringstream ss;
      ss << "FPS = " << fps;
      _string = ss.str();

      _texture.writeText( _string );

      this->setWidth( _texture.getWidth() );
      this->setHeight( _texture.getHeight() );

      _timeElapsed = 0.0;
      _numberFrames = 0;
    }
  }

}

