
#ifndef REGOLITH_ARCHITECTURE_CONTEXT_H_
#define REGOLITH_ARCHITECTURE_CONTEXT_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/MassProduceable.h"
#include "Regolith/Architecture/ControllableInterface.h"
#include "Regolith/Architecture/Component.h"
#include "Regolith/Architecture/ContextLayer.h"
#include "Regolith/Managers/InputHandler.h"
#include "Regolith/Managers/FocusHandler.h"
#include "Regolith/Managers/CollisionHandler.h"
#include "Regolith/Managers/ContextGroup.h"
#include "Regolith/Utilities/ProxyMap.h"
#include "Regolith/GamePlay/Camera.h"

#include <list>


namespace Regolith
{

  class Animated;
  class PhysicalObject;
  class ContextGroup;

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
  class Context : public MassProduceable<ContextGroup&>, public ControllableInterface, public Component
  {
    // Contexts should not be copyable
    Context( const Context& ) = delete;
    Context& operator=( const Context& ) = delete;

    friend class ContextLayer;

//////////////////////////////////////////////////////////////////////////////// 
    private:
      // Context-local audio and input handlers
      ContextGroup* _owner;
      InputHandler _theInput;
      FocusHandler _theFocus;
      CollisionHandler _theCollision;

      // Camera for the context
      Camera _theCamera;

      // Flag to indicate that this context is now closed and may be popped from the context stack.
      bool _closed;

      // Flag to indicate whether the context is advancing
      bool _paused;

      // Flag to indicate whether the context should be paused when it loses focus
      bool _pauseable;

      // Named vector of all the layers owned by the current context
      ProxyMap< ContextLayer > _layers;


//////////////////////////////////////////////////////////////////////////////// 
    protected:
      // Set the pauseable flag
      void setPauseable( bool p ) { _pauseable = p; }

      // Hooks for context state changes
      virtual void onStart() { this->setClosed( false ); }
      virtual void onStop() { this->setClosed( true ); }
      virtual void onPause() = 0;
      virtual void onResume() = 0;
      virtual void onClose() {}


      // Sets the closed flag for the engine to pop it from the context stack.
      // Once this is set to true the context MUST be in state to be deleted. Otherwise data will be lost.
      void setClosed( bool status ) { _closed = status; }

      // Called during the update loop for frame-dependent context actions
      virtual void updateContext( float ) {}

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


//////////////////////////////////////////////////////////////////////////////// 
      // Context Stack functions

      // When this function returns true this context is flagged for removal from the context stack.
      bool closed() const { return _closed; }

      // Return the state of the pause flag
      bool isPaused() const { return _paused; }

      // Interface for context stack
      void startContext() { this->onStart(); }
      void stopContext() { this->onStop(); }
      void pauseContext();
      void resumeContext();


      // Returns true to pause and ignore the parent context. This prevents it from being rendered, updated, stepped, anything!
      // Use this for when child contexts/menus/etc required the full screen and comply obscure the parent
      virtual bool overridesPreviousContext() const = 0;


////////////////////////////////////////////////////////////////////////////////      
      // Engine update loop functions

      // Update the animated objects
      void update( float );


      // Update the moveable objects
      void step( float );


      // Render all the objects
      void render( SDL_Renderer* );


      // Render all the objects
      void resolveCollisions();


////////////////////////////////////////////////////////////////////////////////
      // Object and layer interface

      // Return a layer proxy so that it may be referenced when spawining objects
      Proxy<ContextLayer> requestLayer( std::string );

      // Return a layer proxy so that it may be referenced when spawining objects
      ContextLayer& getLayer( std::string );

  };

}

#endif // REGOLITH_ARCHITECTURE_CONTEXT_H_

