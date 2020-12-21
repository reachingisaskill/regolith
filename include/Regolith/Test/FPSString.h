
#ifndef REGOLITH_TEST_FPS_STRING_H_
#define REGOLITH_TEST_FPS_STRING_H_

#include "Regolith/ObjectInterfaces/AnimatedObject.h"
#include "Regolith/ObjectInterfaces/DrawableObject.h"
#include "Regolith/Textures/ShortText.h"


namespace Regolith
{

  class FPSString : public AnimatedObject, public DrawableObject
  {
    private:
      ShortText _texture;

      std::string _defaultString;
      std::string _string;

      float _timeElapsed;
      unsigned int _numberFrames;


    public:
      FPSString();

      virtual ~FPSString();


      // Clone this class
      virtual PhysicalObject* clone() const { return (PhysicalObject*) new FPSString( *this ); }


      // Perform the basic configuration
      void configure( Json::Value&, ContextGroup& ) override;


      // Reset the FPS tracker
      void reset();


      // For the camera to request the current renderable texture
      virtual Texture& getTexture() { return _texture; }



      // Perform an update to all the animations
      virtual void update( float );

  };

}

#endif // REGOLITH_TEST_FPS_STRING_H_

