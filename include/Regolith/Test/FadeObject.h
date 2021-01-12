
#ifndef REGOLITH_TEST_FADE_OBJECT_H_
#define REGOLITH_TEST_FADE_OBJECT_H_

#include "Regolith/Global/Global.h"
#include "Regolith/ObjectInterfaces/DrawableObject.h"
#include "Regolith/ObjectInterfaces/AnimatedObject.h"
#include "Regolith/Textures/Primitive.h"


namespace Regolith
{

  class FadeObject : public DrawableObject, public AnimatedObject
  {

////////////////////////////////////////////////////////////////////////////////
    private:
      // Texture of the object
      Primitive _texture;

      // Time to fade out
      float _fade;
      float _delay;
      Uint8 _alpha;
      float _time;

    protected :
      // Copy constructor - protected so only way to duplicate objects is through the "clone" function
      FadeObject( const FadeObject& );


////////////////////////////////////////////////////////////////////////////////
      // Creation and destruction
    public:
      // Default constructor
      FadeObject();

      // Destructor
      virtual ~FadeObject();

      // Function to create to an copied instance at the specified position
      // ALL derived classes my override this function with the copy-constructor for that class!
      virtual PhysicalObject* clone() const override { return (PhysicalObject*) new FadeObject( *this ); }


////////////////////////////////////////////////////////////////////////////////
      // Configuration

      // Perform the basic configuration
      void configure( Json::Value&, ContextGroup& ) override;

      // Resets the object to it's initial configuration to allow reusing of objects
      virtual void reset() override;


////////////////////////////////////////////////////////////////////////////////
      // Specifc functions for enabling physics and rendering on the object

      // For the camera to request the current renderable texture
      virtual Texture& getTexture() override;

      // Perform an update to all the animations
      virtual void update( float ) override;
  };

}

#endif // REGOLITH_TEST_FADE_OBJECT_H_

