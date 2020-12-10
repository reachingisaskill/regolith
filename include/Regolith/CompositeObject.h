
#ifndef REGOLITH_COMPOSITE_OBJECT_H_
#define REGOLITH_COMPOSITE_OBJECT_H_

#include "Regolith/Global/Global.h"
#include "Regolith/GamePlay/PhysicalObject.h"


namespace Regolith
{

  class CompositeObject : public PhysicalObject
  {
////////////////////////////////////////////////////////////////////////////////
    private:
      // Map of all the children
      PhysicalObjectMap _children;


////////////////////////////////////////////////////////////////////////////////
    protected :
      // Copy constructor - protected so only way to duplicate objects is through the "clone" function
      CompositeObject( const CompositeObject& );


////////////////////////////////////////////////////////////////////////////////
      // Creation and destruction
    public:
      // Default constructor
      CompositeObject();

      // Destructor
      virtual ~CompositeObject();

      // Function to create to an copied instance at the specified position
      // ALL derived classes my override this function with the copy-constructor for that class!
      virtual CompositeObject* clone() const { return new CompositeObject( *this ); }


////////////////////////////////////////////////////////////////////////////////
      // Configuration

      // Perform the basic configuration
      void configure( Json::Value&, ContextGroup& ) override;

      // Resets the object to it's initial configuration to allow reusing of objects
      virtual void reset();


////////////////////////////////////////////////////////////////////////////////
      // Specifc functions for enabling physics and rendering on the object

      // Returns the vector of children that are active for the current state
      const PhysicalObjectVector& getChildren() const;

      // Perform an update to all the animations
      virtual void update( float ) override;

      // For the camera to request the current renderable texture
      virtual const Texture& getTexture() const;

      // For the collision handler to request the current hitboxes
      virtual const Collision& getCollision() const;
  };

}

#endif // REGOLITH_COMPOSITE_OBJECT_H_

