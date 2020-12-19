
#ifndef REGOLITH_CONTEXTS_CONTEXT_H_
#define REGOLITH_CONTEXTS_CONTEXT_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/MassProduceable.h"
#include "Regolith/Architecture/ControllableInterface.h"
#include "Regolith/Architecture/Component.h"
#include "Regolith/Managers/InputHandler.h"
#include "Regolith/Managers/FocusHandler.h"
#include "Regolith/Managers/CollisionHandler.h"
#include "Regolith/Managers/ContextGroup.h"
#include "Regolith/Contexts/ContextLayer.h"

#include <map>


namespace Regolith
{

////////////////////////////////////////////////////////////////////////////////
  // Forward declarations
  class PhysicalObject;
  class ContextGroup;
  class Camera;

  /*
   * Defines a context interface.
   * This is any object that can hold the focus of the user input.
   * This interface class exposes functions to force the handling of specific
   * events within the current context.
   * Examples of derived classes include all scenes and dialog boxes.
   * The engine class has no knowledge of contexts! They are only found at scene-level and below.
   * The standard rendering interface is still to use the current scene, and let the scene
   * call the rendering function for the current context.
   */
  class Context : public MassProduceable<ContextGroup&>, public ControllableInterface
  {
    // Contexts should not be copyable
    Context( const Context& ) = delete;
    Context& operator=( const Context& ) = delete;

    typedef std::map< std::string, ContextLayer > ContextLayerMap;

//////////////////////////////////////////////////////////////////////////////// 
    private:
      // Context-local handlers
      ContextGroup* _owner;
      InputHandler _theInput;
      FocusHandler _theFocus;
      CollisionHandler _theCollision;

      // Pointer to an optional track to play on load
      Music _defaultTrack;

      // Details for using the camera
      Vector _cameraPosition;
      Vector _cameraOffset;

      // Flag to indicate that this context is now closed and may be popped from the context stack.
      bool _closed;

      // Flag to indicate whether the context is advancing
      bool _paused;

      // Flag to indicate whether the context should be paused when it loses focus
      bool _pauseable;

      // Named vector of all the layers owned by the current context
      ContextLayerMap _layers;


//////////////////////////////////////////////////////////////////////////////// 
    protected:
      // Set the pauseable flag
      void setPauseable( bool p ) { _pauseable = p; }

      // Hooks for context state changes
      virtual void onStart() { this->setClosed( false ); }
      virtual void onStop() { this->setClosed( true ); }
      virtual void onPause() {}
      virtual void onResume() {}


      // Sets the closed flag for the engine to pop it from the context stack.
      // Once this is set to true the context MUST be in state to be deleted. Otherwise data will be lost.
      void setClosed( bool status ) { _closed = status; }

      // Called during the update loop for frame-dependent context actions
      virtual void updateContext( float ) = 0;

      // Called after the physics processes to update the current position that the camera renders from
      virtual Vector updateCamera( float ) const = 0;

      // Called for each object that is flagged to have global physics
      virtual void updatePhysics( PhysicalObject*, float ) const = 0;

//////////////////////////////////////////////////////////////////////////////// 
    public:
      // Con/De-structor
      Context();
      virtual ~Context();

      // Configure the context
      void configure( Json::Value&, ContextGroup& ) override;

      // Return a pointer to the owning context group
      ContextGroup* owner() const { return _owner; }

      // Return the input handler for this context
      InputHandler* inputHandler() { return &_theInput; }

      // Return a reference to a specific context layer
      ContextLayer& getLayer( std::string );

//////////////////////////////////////////////////////////////////////////////// 
      // Context Stack functions

      // When this function returns true this context is flagged for removal from the context stack.
      bool closed() const { return _closed; }

      // Return the state of the pause flag
      bool isPaused() const { return _paused; }

      // Interface for context stack
      void startContext();
      void stopContext();
      void pauseContext();
      void resumeContext();


      // Returns true to pause and ignore the parent context. This prevents it from being rendered, updated, stepped, anything!
      // Use this for when child contexts/menus/etc required the full screen and comply obscure the parent
      virtual bool overridesPreviousContext() const = 0;


////////////////////////////////////////////////////////////////////////////////      
      // Engine update loop functions

      // Update the animated objects
      void update( float );


      // Render all the objects
      void render( Camera& );


//////////////////////////////////////////////////
      // Requirements for the ControllableInterface - input action handling

      // Register input actions with this context. Default is input actions
      virtual void registerActions( InputHandler& ) override {}

  };

}

#endif // REGOLITH_CONTEXTS_CONTEXT_H_

