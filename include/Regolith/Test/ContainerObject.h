
#ifndef REGOLITH_TEST_CONTAINER_OBJECT_H_
#define REGOLITH_TEST_CONTAINER_OBJECT_H_

#include "Regolith/Global/Global.h"
#include "Regolith/ObjectInterfaces/CollidableObject.h"
#include "Regolith/Collisions/Collision.h"


namespace Regolith
{

  class ContainerObject : public CollidableObject
  {

////////////////////////////////////////////////////////////////////////////////
    private:
      // Collision for the object
      Collision _collision;

    protected :
      // Copy constructor - protected so only way to duplicate objects is through the "clone" function
      ContainerObject( const ContainerObject& );


////////////////////////////////////////////////////////////////////////////////
      // Creation and destruction
    public:
      // Default constructor
      ContainerObject();

      // Destructor
      virtual ~ContainerObject();

      // Function to create to an copied instance at the specified position
      // ALL derived classes my override this function with the copy-constructor for that class!
      virtual PhysicalObject* clone() const { return (PhysicalObject*) new ContainerObject( *this ); }


////////////////////////////////////////////////////////////////////////////////
      // Configuration

      // Perform the basic configuration
      void configure( Json::Value&, ContextGroup& ) override;

      // Resets the object to it's initial configuration to allow reusing of objects
      virtual void reset() override;

////////////////////////////////////////////////////////////////////////////////
      // Specifc functions for enabling physics and rendering on the object

      // For the collision handler to request the current hitboxes
      virtual Collision& getCollision() override;

      // call back function for when this object collides with another
      virtual void onCollision( Contact&, CollidableObject* ) override;
  };

}

#endif // REGOLITH_TEST_CONTAINER_OBJECT_H_

