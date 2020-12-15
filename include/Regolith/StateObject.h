
#ifndef REGOLITH_STATE_OBJECT_H_
#define REGOLITH_STATE_OBJECT_H_

#include "Regolith/Global/Global.h"
#include "Regolith/GamePlay/PhysicalObject.h"
#include "Regolith/Textures/Spritesheet.h"


namespace Regolith
{

  class StateObject : public PhysicalObject
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
          Spritesheet texture;
          Collision collision;
          unsigned int currentFrame;
//          StateObjectVector children;

          // Constructor
          StateDetails();

          // Update the frame number
          void update( float );

          // Reset the animation
          void reset();

      };

      typedef std::map< std::string, StateDetails > StateMap;


////////////////////////////////////////////////////////////////////////////////
    private:

      // Map of all the possible states
      StateMap _stateMap;

      // When reset is called return to this state
      std::string _startState;
      StateDetails* _startStatePointer;

      // Reference to the current state
      StateDetails* _currentState;


    protected :
      // Copy constructor - protected so only way to duplicate objects is through the "clone" function
      StateObject( const StateObject& );

////////////////////////////////////////////////////////////////////////////////
      // State interface functions

      // Gets a pointer to a state
      StateDetails* getState( std::string name ) { return &_stateMap[ name ]; }

      // Sets the current state to the reference
      void setState( StateDetails* state ) { _currentState = state; _currentState->reset(); }


////////////////////////////////////////////////////////////////////////////////
      // Creation and destruction
    public:
      // Default constructor
      StateObject();

      // Destructor
      virtual ~StateObject();

      // Function to create to an copied instance at the specified position
      // ALL derived classes my override this function with the copy-constructor for that class!
      virtual StateObject* clone() const { return new StateObject( *this ); }


////////////////////////////////////////////////////////////////////////////////
      // Configuration

      // Perform the basic configuration
      void configure( Json::Value&, ContextGroup& ) override;

      // Resets the object to it's initial configuration to allow reusing of objects
      virtual void reset();


////////////////////////////////////////////////////////////////////////////////
      // Specifc functions for enabling physics and rendering on the object

      // Perform an update to all the animations
      virtual void update( float ) override;

      // For the camera to request the current renderable texture
      virtual Texture& getTexture() override { return _currentState->texture; }

      // For the collision handler to request the current hitboxes
      virtual Collision& getCollision() override { return _currentState->collision; }


////////////////////////////////////////////////////////////////////////////////
      // Object property accessors and modifiers

      // Return the current state of the object
      unsigned int getState() const { return _currentState->id; }
  };

}

#endif // REGOLITH_STATE_OBJECT_H_

