
#ifndef REGOLITH_ARCHITECTURE_PHYSICAL_OBJECT_H_
#define REGOLITH_ARCHITECTURE_PHYSICAL_OBJECT_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/GameObject.h"

namespace Regolith
{

  /*
   * Defines the interface for "physical objects" any object that can be cloned, moved, seen or collided with.
   * Must therefore have a position and an area.
   */
  class PhysicalObject : virtual public GameObject
  {
    private:
      // Flag that this object is to be removed from the scene
      bool _destroyMe;
      // Position with respect to the parent object
      Vector _position;
      // Rotation with respect to the parent object
      float _rotation;

      // Used to determine collision and movement properties
      float _mass;
      float _inverseMass;

      // Bounding box dimensions
      float _width;
      float _height;

      // Used to configure the collision logic
      CollisionTeam _collisionTeam;
      CollisionType _collisionType;

      // Map of all the children
      PhysicalObjectMap _children;


    protected :
      // Copy constructor - protected so only way to duplicate objects is through the "clone" function
      PhysicalObject( const PhysicalObject& );

      // For derived classes to update the mass. Sets both mass and it's inverse
      void setMass( float );

      // For derived classes to configure the collision properties
      void setCollision( CollisionTeam, CollisionType );


    public:
      // Default constructor
      PhysicalObject();

      // Destructor
      virtual ~PhysicalObject();

      // Movement and assignment not allowed. Must be careful with these objects - unecessary copies will get expensive during runtime
      PhysicalObject( const PhysicalObject&& ) = delete;
      PhysicalObject& operator=( const PhysicalObject& ) = delete;
      PhysicalObject& operator=( const PhysicalObject&& ) = delete;



      // Perform the basic configuration
      void configure( Json::Value&, ContextGroup&, DataHandler& ) override;


      // Tells the caller that derived classes come from a physical object.
      // Warning: overriding this function change the value may prevent items from being added to context layers
      bool isPhysical() const override { return true; }


      // Function to create to an copied instance at the specified position
      virtual PhysicalObject* clone() const { return new PhysicalObject( *this ); }


      // Function to determine if existing instance should be remove from the context
      bool isDestroyed() const { return _destroyMe; }

      // set the flag to remove the object
      void destroy() { _destroyMe = true; }


      // Mass variable accessors.
      const float& getMass() const { return _mass; }
      const float& getInverseMass() const { return _inverseMass; }


      // Position set/get
      const Vector& position() const { return _position; }
      Vector getPosition() const { return _position; }
      void setPosition( Vector p ) { _position = p; }

      // Rotation set/get
      const float& rotation() const { return _rotation; }
      float getRotation() const { return _rotation; }
      void setRotation( float r ) { _rotation = r; }

      // Preferred methods for changing positin/rotation
      void move( Vector m ) { _position += m; }
      void rotate( float r ) { _rotation += r; }

      // Bounding box dimensions
      const float& getWidth() const { return _width; }
      const float& getHeight() const { return _height; }

  };

}

#endif // REGOLITH_ARCHITECTURE_PHYSICAL_OBJECT_H_

