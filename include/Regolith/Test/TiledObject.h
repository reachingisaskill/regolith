
#ifndef REGOLITH_TEST_TILED_OBJECT_H_
#define REGOLITH_TEST_TILED_OBJECT_H_

#include "Regolith/Global/Global.h"
#include "Regolith/ObjectInterfaces/DrawableObject.h"
#include "Regolith/ObjectInterfaces/CollidableObject.h"
#include "Regolith/Collisions/TilesheetCollision.h"
#include "Regolith/Textures/Tilesheet.h"


namespace Regolith
{

  class TiledObject : public DrawableObject, public CollidableObject
  {

////////////////////////////////////////////////////////////////////////////////
    private:
      // Texture of the object
      Tilesheet _texture;

      // Collision for the object
      TilesheetCollision _collision;

    protected :
      // Copy constructor - protected so only way to duplicate objects is through the "clone" function
      TiledObject( const TiledObject& );


////////////////////////////////////////////////////////////////////////////////
      // Creation and destruction
    public:
      // Default constructor
      TiledObject();

      // Destructor
      virtual ~TiledObject();

      // Function to create to an copied instance at the specified position
      // ALL derived classes my override this function with the copy-constructor for that class!
      virtual PhysicalObject* clone() const override { return (PhysicalObject*) new TiledObject( *this ); }


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

#endif // REGOLITH_TEST_TILED_OBJECT_H_

