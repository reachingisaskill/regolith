
#ifndef REGOLITH_ARCHITECTURE_PHYSICAL_OBJECT_H_
#define REGOLITH_ARCHITECTURE_PHYSICAL_OBJECT_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/GameObject.h"

namespace Regolith
{
////////////////////////////////////////////////////////////////////////////////
  // Forward declares
  struct Contact;

  /*
   * Defines the interface for "physical objects" any object that can be cloned, moved, seen or collided with.
   * Must therefore have a position and an area.
   */
  class PhysicalObject : virtual public GameObject
  {

////////////////////////////////////////////////////////////////////////////////
    private:
      // Flag that this object is to be removed from the scene
      bool _destroyMe;

      // Flag to indicate this object can be moved by physics processes
      bool _hasMoveable;
      // Flag to indicate this object responds to global physics
      bool _hasPhysics;

      // Position with respect to the parent object
      Vector _position;
      // Rotation with respect to the parent object
      float _rotation;
      // Flip flag
      SDL_RendererFlip _flipFlag;

      // Center point of the object. Required to calculate rotations. (The CoM)
      Vector _center;
      SDL_Point _centerPoint; // Camera requires an SDL_Point so cache this value

      // Used to determine collision and movement properties
      float _mass;
      float _inverseMass;
      float _inertiaDensity;
      float _inverseInertiaDensity;
      float _elasticity;

      // Defines the size of the drawable area for the object. Should ALWAYS contain all the collision boxes.
      float _width;
      float _height;

      // Physics variables
      Vector _velocity;
      Vector _forces;

      // Rotation physics variables
      float _angularVel;
      float _torques;


      // Used to organise objects and configure the collision logic
      CollisionTeam _collisionTeam;


    protected :
      // Copy constructor - protected so only way to duplicate objects is through the "clone" function
      PhysicalObject( const PhysicalObject& );


////////////////////////////////////////////////////////////////////////////////
      // Property modifiers
      
      // Set the width of the the object
      void setWidth( float w ) { _width = w; }

      // Set the height of the the object
      void setHeight( float h ) { _height = h; }

      // Set the rotation center for the object
      void setCenter( Vector c ) { _center = c; _centerPoint.x = c.x(); _centerPoint.y = c.y(); }

      // For derived classes to update the mass. Sets both mass and it's inverse
      void setMass( float );

      // For derived classes to update the angular inertia.
      void setInertiaDensity( float );

      // For derived classes to set the moveable flag
      void setMoveable( bool m ) { _hasMoveable = m; }

      // For derived classes to set whether this object is affected by global physics effects
      void setPhysics( bool p ) { _hasPhysics = p; }


////////////////////////////////////////////////////////////////////////////////
      // Creation and destruction
    public:
      // Default constructor
      PhysicalObject();

      // Destructor
      virtual ~PhysicalObject();

      // Movement and assignment not allowed. Must be careful with these objects - unecessary copies will get expensive during runtime
      PhysicalObject( const PhysicalObject&& ) = delete;
      PhysicalObject& operator=( const PhysicalObject& ) = delete;
      PhysicalObject& operator=( const PhysicalObject&& ) = delete;


      // Function to create to an copied instance at the specified position
      // ALL derived classes my override this function with the copy-constructor for that class!
      virtual PhysicalObject* clone() const = 0;


////////////////////////////////////////////////////////////////////////////////
      // Configuration

      // Perform the basic configuration
      void configure( Json::Value&, ContextGroup& ) override;


      // Tells the caller that derived classes come from a physical object.
      // Warning: overriding this function change the value may prevent items from being added to context layers
      bool isPhysical() const override { return true; }


      // Tells the caller that the object is moveable. i.e. the velocity may be non-zero
      virtual bool hasMovement() const { return _hasMoveable; }

      // Tells the caller that the is animated for every frame
      virtual bool hasPhysics() const { return _hasPhysics; }


////////////////////////////////////////////////////////////////////////////////
      // Controlling object permanence within the context

      // Function to determine if existing instance should be remove from the context
      bool isDestroyed() const { return _destroyMe; }

      // set the flag to remove the object
      void destroy() { _destroyMe = true; }

      // Resets the object to it's initial configuration to allow reusing of objects
      virtual void reset();


////////////////////////////////////////////////////////////////////////////////
      // Specifc functions for enabling physics


      // Perform the time integration for movement of this object
      virtual void step( float );



////////////////////////////////////////////////////////////////////////////////
      // Object property accessors and modifiers

      // Preferred methods for changing position/rotation
      void move( Vector m ) { _position += m; }
      void rotate( float r ) { _rotation += r; }


      // For derived classes to impose a force
      void addForce( Vector f ) { _forces += f; }

      // Forces an immediate acceleration of the object. Used mostly to apply impulses from collisions
      void kick( Vector& k ) { _velocity += k; }


      // For derived classes to impose a torque
      void addTorque( float t ) { _torques += t; }

      // Forces an immediate rotational acceleration of the object. Used mostly to apply angular impulses from collisions
      void spin( float s ) { _angularVel += s; }



      // Return the assigned collision team
      CollisionTeam getCollisionTeam() const { return _collisionTeam; }


      // Mass variable accessors.
      float getMass() const { return _mass; }
      float getInverseMass() const { return _inverseMass; }

      // Moment of inertia accessors
      float getInertia() const { return _inertiaDensity*_mass; }
      float getInverseInertia() const { return _inverseInertiaDensity*_inverseMass; }

      // Rotation center
      const Vector& center() const { return _center; }
      Vector getCenter() const { return _center; }
      const SDL_Point& getCenterPoint() const { return _centerPoint; }

      // Collision physics accessors
      float getElasticity() const { return _elasticity; }

      // Position set/get
      const Vector& position() const { return _position; }
      Vector getPosition() const { return _position; }
      void setPosition( Vector p ) { _position = p; }

      // Velocity
      const Vector& velocity() const { return _velocity; }
      Vector getVelocity() const { return _velocity; }
      void setVelocity( Vector v ) { _velocity = v; }

      // Rotation set/get
      const float& rotation() const { return _rotation; }
      float getRotation() const { return _rotation; }
      void setRotation( float r ) { _rotation = r; }

      // Velocity
      const float& angularVelocity() const { return _angularVel; }
      float getAngularVelocity() const { return _angularVel; }
      void setAngularVelocity( float a ) { _angularVel = a; }

      // Flip state
      SDL_RendererFlip getFlipFlag() const { return _flipFlag; }
      void setFlipFlag( SDL_RendererFlip f ) { _flipFlag = f; }

      // Bounding box dimensions
      const float& getWidth() const { return _width; }
      const float& getHeight() const { return _height; }
  };

}

#endif // REGOLITH_ARCHITECTURE_PHYSICAL_OBJECT_H_

