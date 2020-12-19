
#ifndef REGOLITH_TEST_BONK_OBJECT_H_
#define REGOLITH_TEST_BONK_OBJECT_H_

#include "Regolith/Global/Global.h"
#include "Regolith/ObjectInterfaces/DrawableObject.h"
#include "Regolith/ObjectInterfaces/CollidableObject.h"
#include "Regolith/ObjectInterfaces/NoisyObject.h"
#include "Regolith/Collisions/Collision.h"
#include "Regolith/Audio/Sound.h"
#include "Regolith/Textures/Spritesheet.h"


namespace Regolith
{

  class BonkObject : public DrawableObject, public CollidableObject, public NoisyObject
  {

////////////////////////////////////////////////////////////////////////////////
    private:
      // Texture of the object
      Spritesheet _texture;

      // Collision for the object
      Collision _collision;

      // Noise to make in collision
      Sound _bonk;

    protected :
      // Copy constructor - protected so only way to duplicate objects is through the "clone" function
      BonkObject( const BonkObject& );


      // For derived classes to implement and register the requested sound effects with the audio handler
      virtual void registerSoundEffects( AudioHandler* ) override;


////////////////////////////////////////////////////////////////////////////////
      // Creation and destruction
    public:
      // Default constructor
      BonkObject();

      // Destructor
      virtual ~BonkObject();

      // Function to create to an copied instance at the specified position
      // ALL derived classes my override this function with the copy-constructor for that class!
      virtual PhysicalObject* clone() const override { return (PhysicalObject*) new BonkObject( *this ); }


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

      // For the collision handler to request the current hitboxes
      virtual Collision& getCollision() override;

      // Resolve the collision that was found
      void onCollision( Contact&, CollidableObject* );
  };

}

#endif // REGOLITH_TEST_BONK_OBJECT_H_

