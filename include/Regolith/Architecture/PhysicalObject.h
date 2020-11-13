
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

      // Physics variables
      Vector _velocity;
      Vector _forces;

      // Used to configure the collision logic
      CollisionTeam _collisionTeam;
      CollisionType _collisionType;

      // Map of all the children
      PhysicalObjectMap _children;

      // Current state of the object. States allow different subsets of children to be interogated at a time
      unsigned int _state;

      // Vector of vectors of different subsets of children. One for each state.
      StateVector _stateSubSets;

      // Vector of children active in the current state
      PhysicalObjectVector& _currentChildren;


    protected :
      // Copy constructor - protected so only way to duplicate objects is through the "clone" function
      PhysicalObject( const PhysicalObject& );

      // Return the vector of children used in the current state
      PhysicalObjectVector& getChildren() { return _currentChildren; }

////////////////////////////////////////////////////////////////////////////////
      // Property modifiers

      // For derived classes to update the mass. Sets both mass and it's inverse
      void setMass( float );

      // For derived classes to configure the collision properties
      void setCollision( CollisionTeam, CollisionType );

      // For derived classes to set the velocity
      void setVelocity( Vector v ) { _velocity = v; }

      // For derived classes to impose a force
      void addForce( Vector f ) { _forces += f; }


////////////////////////////////////////////////////////////////////////////////
      // Frame update functions

      // Draw this object
      void renderThis( SDL_Renderer*, const Camera& ) const;

      // Step this object
      void stepThis( float );


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
      virtual PhysicalObject* clone() const { return new PhysicalObject( *this ); }


////////////////////////////////////////////////////////////////////////////////
      // Configuration

      // Perform the basic configuration
      void configure( Json::Value&, ContextGroup&, DataHandler& ) override;


////////////////////////////////////////////////////////////////////////////////
      // Properties of this class

      // Tells the caller that derived classes come from a physical object.
      // Warning: overriding this function change the value may prevent items from being added to context layers
      bool isPhysical() const override { return true; }


      // Tells the caller that the object is moveable. i.e. the velocity may be non-zero
      virtual bool hasMovement() const override { return _mass >= epsilon; }

      // Tells the caller that the object can be rendered.
      virtual bool hasTexture() const override { return false; }


////////////////////////////////////////////////////////////////////////////////
      // Controlling object permanence within the context

      // Function to determine if existing instance should be remove from the context
      bool isDestroyed() const { return _destroyMe; }

      // set the flag to remove the object
      void destroy() { _destroyMe = true; }



////////////////////////////////////////////////////////////////////////////////
      // Specifc functions for rendering and integrating the object.

      // Perform the steps to render the object and all children
      virtual void render( SDL_Renderer*, const Camera& ) const;


      // Perform the time integration for movement of this object and all children
      virtual void step( float );


      // Perform collision resolution
      virtual bool collides( PhysicalObject* );


////////////////////////////////////////////////////////////////////////////////
      // Object property accessors and modifiers

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

      // Velocity
      const Vector& velocity() const { return _velocity; }
      Vector getVelocity() const { return _velocity; }
  };

}

#endif // REGOLITH_ARCHITECTURE_PHYSICAL_OBJECT_H_

