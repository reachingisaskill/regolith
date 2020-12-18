
#ifndef REGOLITH_TEST_SIMPLE_OBJECT_H_
#define REGOLITH_TEST_SIMPLE_OBJECT_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/DrawableObject.h"
#include "Regolith/Architecture/CollidableObject.h"
#include "Regolith/GamePlay/Collision.h"
#include "Regolith/Textures/Spritesheet.h"


namespace Regolith
{

  class SimpleObject : public DrawableObject, public CollidableObject
  {

////////////////////////////////////////////////////////////////////////////////
    private:
      // Texture of the object
      Spritesheet _texture;

      // Collision for the object
      Collision _collision;

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
      virtual PhysicalObject* clone() const override { return (PhysicalObject*) new SimpleObject( *this ); }


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

#endif // REGOLITH_TEST_SIMPLE_OBJECT_H_

