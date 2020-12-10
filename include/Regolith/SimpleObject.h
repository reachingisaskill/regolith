
#ifndef REGOLITH_SIMPLE_OBJECT_H_
#define REGOLITH_SIMPLE_OBJECT_H_

#include "Regolith/Global/Global.h"
#include "Regolith/GamePlay/PhysicalObject.h"


namespace Regolith
{

  class SimpleObject : public PhysicalObject
  {

////////////////////////////////////////////////////////////////////////////////
    private:
      // Texture of the object
      Texture _texture;

      // Collision for the object
      Collision _collision;

      // Update period for the texture and collision
      float _updatePeriod;
      float _count;
      unsigned int _numberFrames;
      unsigned int _currentFrame;

    protected :
      // Copy constructor - protected so only way to duplicate objects is through the "clone" function
      SimpleObject( const SimpleObject& );


////////////////////////////////////////////////////////////////////////////////
      // Creation and destruction
    public:
      // Default constructor
      SimpleObject();

      // Destructor
      virtual ~SimpleObject();

      // Function to create to an copied instance at the specified position
      // ALL derived classes my override this function with the copy-constructor for that class!
      virtual SimpleObject* clone() const { return new SimpleObject( *this ); }


////////////////////////////////////////////////////////////////////////////////
      // Configuration

      // Perform the basic configuration
      void configure( Json::Value&, ContextGroup& ) override;

      // Resets the object to it's initial configuration to allow reusing of objects
      virtual void reset() override;

////////////////////////////////////////////////////////////////////////////////
      // Specifc functions for enabling physics and rendering on the object

      // Perform an update to all the animations
      virtual void update( float timestep ) override;

      // For the camera to request the current renderable texture
      virtual const Texture& getTexture() const override;

      // For the collision handler to request the current hitboxes
      virtual const Collision& getCollision() const override;
  };

}

#endif // REGOLITH_SIMPLE_OBJECT_H_

