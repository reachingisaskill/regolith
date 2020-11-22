
#ifndef REGOLITH_ARCHITECTURE_PHYSICAL_OBJECT_H_
#define REGOLITH_ARCHITECTURE_PHYSICAL_OBJECT_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/GameObject.h"
#include "Regolith/GamePlay/Texture.h"
#include "Regolith/GamePlay/Collision.h"

namespace Regolith
{

  /*
   * Defines the interface for "physical objects" any object that can be cloned, moved, seen or collided with.
   * Must therefore have a position and an area.
   */
  class PhysicalObject : virtual public GameObject
  {
////////////////////////////////////////////////////////////////////////////////
    // Info required for each state
    public:
      class StateDetails
      {
        private:
          float _count;
          unsigned int _numberFrames;

        public:
          // Public member variables
          unsigned int id;
          float updatePeriod;
          Texture texture;
          Collision collision;
          unsigned int currentFrame;
//          PhysicalObjectVector children;

          // Constructor
          StateDetails();

          // Update the frame number
          void update( float );

      };

      typedef std::map< std::string, StateDetails > StateMap;


////////////////////////////////////////////////////////////////////////////////
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

      // Defines the size of the drawable area for the object. Should ALWAYS contain all the collision boxes.
      float _width;
      float _height;

      // Physics variables
      Vector _velocity;
      Vector _forces;

      // Used to configure the collision logic
      CollisionTeam _collisionTeam;

//      // Map of all the children
//      PhysicalObjectMap _children;

      // Map of all the possible states
      StateMap _stateMap;

      // Reference to the current state
      StateDetails& _currentState;

      // When reset is called return to this state
      StateDetails& _startState;


    protected :
      // Copy constructor - protected so only way to duplicate objects is through the "clone" function
      PhysicalObject( const PhysicalObject& );

////////////////////////////////////////////////////////////////////////////////
      // Property modifiers

      // For derived classes to update the mass. Sets both mass and it's inverse
      void setMass( float );

      // For derived classes to set the velocity
      void setVelocity( Vector v ) { _velocity = v; }

      // For derived classes to impose a force
      void addForce( Vector f ) { _forces += f; }


////////////////////////////////////////////////////////////////////////////////
      // State interface functions

      // Gets a reference to a state
      StateDetails& getState( std::string name ) { return _stateMap[ name ]; }

      // Sets the current state to the reference
      void setState( StateDetails& state ) { _currentState = state; }

//      // Return the vector of children used in the current state
//      PhysicalObjectVector& getChildren() { return _currentState.children; }

      // Return the texture being used in the current state
//      Texture& getTexture() { return _currentState.texture; }


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
      virtual PhysicalObject* clone() const { return new PhysicalObject( *this ); }


////////////////////////////////////////////////////////////////////////////////
      // Configuration

      // Perform the basic configuration
      void configure( Json::Value&, ContextGroup&, DataHandler& ) override;

      // Resets the object to it's initial configuration to allow reusing of objects
      virtual void reset();


      // Tells the caller that derived classes come from a physical object.
      // Warning: overriding this function change the value may prevent items from being added to context layers
      bool isPhysical() const override { return true; }


      // Tells the caller that the object is moveable. i.e. the velocity may be non-zero
      virtual bool hasMovement() const override { return _mass >= epsilon; }

      // Tells the caller that the object can be rendered.
      virtual bool hasTexture() const override { return false; }

      // Tells the caller that the is animated for every frame
      virtual bool hasAnimation() const override { return false; }


////////////////////////////////////////////////////////////////////////////////
      // Controlling object permanence within the context

      // Function to determine if existing instance should be remove from the context
      bool isDestroyed() const { return _destroyMe; }

      // set the flag to remove the object
      void destroy() { _destroyMe = true; }


////////////////////////////////////////////////////////////////////////////////
      // Specifc functions for enabling physics and rendering on the object

//      // Returns the vector of children that are active for the current state
//      const PhysicalObjectVector& getChildren() const { return _currentState.children; }


      // For the camera to request the current renderable texture
      const Texture& getTexture() const { return _currentState.texture; }


      // Perform the time integration for movement of this object
      virtual void step( float );

      // Perform an update to all the animations
      virtual void update( float );

      // Perform collision resolution on this object
      virtual bool collides( PhysicalObject* );


////////////////////////////////////////////////////////////////////////////////
      // Object property accessors and modifiers

      // Return the current state of the object
      unsigned int getState() const { return _currentState.id; }
      
      // Return the assigned collision team
      CollisionTeam getCollisionTeam() const { return _collisionTeam; }


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

