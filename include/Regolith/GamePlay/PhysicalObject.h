
#ifndef REGOLITH_GAMEPLAY_PHYSICAL_OBJECT_H_
#define REGOLITH_GAMEPLAY_PHYSICAL_OBJECT_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/GameObject.h"
#include "Regolith/Architecture/Texture.h"
#include "Regolith/GamePlay/Collision.h"

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
      // Flag to indicate this object has a texture to render to the window
      bool _hasTexture;
      // Flag to indicate this object has animation
      bool _hasAnimation;
      // Flag to indicate this object responds to global physics
      bool _hasPhysics;

      // Position with respect to the parent object
      Vector _position;
      // Rotation with respect to the parent object
      float _rotation;
      // Flip flag
      SDL_RendererFlip _flipFlag;

      // Used to determine collision and movement properties
      float _mass;
      float _inverseMass;
      float _elasticity;

      // Defines the size of the drawable area for the object. Should ALWAYS contain all the collision boxes.
      float _width;
      float _height;

      // Physics variables
      Vector _velocity;
      Vector _forces;

      // Used to configure the collision logic
      CollisionTeam _collisionTeam;


    protected :
      // Copy constructor - protected so only way to duplicate objects is through the "clone" function
      PhysicalObject( const PhysicalObject& );

////////////////////////////////////////////////////////////////////////////////
      // Property modifiers

      // For derived classes to update the mass. Sets both mass and it's inverse
      void setMass( float );


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
      virtual bool hasMovement() const override { return _hasMoveable; }

      // Tells the caller that the object can be rendered.
      virtual bool hasTexture() const override { return _hasTexture; }

      // Tells the caller that the is animated for every frame
      virtual bool hasAnimation() const override { return _hasAnimation; }

      // Tells the caller that the is animated for every frame
      virtual bool hasPhysics() const override { return _hasPhysics; }


////////////////////////////////////////////////////////////////////////////////
      // Controlling object permanence within the context

      // Function to determine if existing instance should be remove from the context
      bool isDestroyed() const { return _destroyMe; }

      // set the flag to remove the object
      void destroy() { _destroyMe = true; }

      // Resets the object to it's initial configuration to allow reusing of objects
      virtual void reset();


////////////////////////////////////////////////////////////////////////////////
      // Specifc functions for enabling physics and rendering on the object


      // For the camera to request the current renderable texture
      virtual Texture& getTexture() = 0;

      // For the collision handler to request the current hitboxes
      virtual const Collision& getCollision() = 0;

      // Perform an update to all the animations
      virtual void update( float ) = 0;

      // Perform the time integration for movement of this object
      virtual void step( float );

      // Call back function for when this object collides with another
      virtual void onCollision( Contact&, PhysicalObject* );


////////////////////////////////////////////////////////////////////////////////
      // Object property accessors and modifiers


      // For derived classes to impose a force
      void addForce( Vector f ) { _forces += f; }

      // Forces an immediate movement of the object. Used mostly to prevent overlap during collisions.
      void kick( Vector& j ) { _velocity += j; }


      // Return the assigned collision team
      CollisionTeam getCollisionTeam() const { return _collisionTeam; }


      // Mass variable accessors.
      const float& getMass() const { return _mass; }
      const float& getInverseMass() const { return _inverseMass; }

      // Collision physics accessors
      const float& getElasticity() const { return _elasticity; }

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

      // Preferred methods for changing position/rotation
      void move( Vector m ) { _position += m; }
      void rotate( float r ) { _rotation += r; }

      // Bounding box dimensions
      const float& getWidth() const { return _width; }
      const float& getHeight() const { return _height; }
  };

}

#endif // REGOLITH_GAMEPLAY_PHYSICAL_OBJECT_H_

