
#ifndef REGOLITH_CONTEXT_H_
#define REGOLITH_CONTEXT_H_

#include "Managers/Definitions.h"
#include "Architecture/Controllable.h"
#include "Managers/InputManager.h"
#include "Managers/AudioManager.h"


namespace Regolith
{
  // Forward Declarations
  class Manager;

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
  class Context : public Controllable
  {
    // Contexts should not be copyable
    Context( const Context& ) = delete;
    Context& operator=( const Context& ) = delete;

    private:
      // Pointer to the Input manager owned input handler
      InputHandler _theInput;
      AudioHandler _theAudio;

      std::map< std::string, unsigned int > _contextEvents;

    public:
      explicit Context( std::string );

      virtual ~Context() {}


      // Flag to see it needs updating with the current time
      virtual bool isAnimated() const = 0;

      // Update the animations, etc
      virtual void update( Uint32 ) = 0;


      // Flag to tell the engine to render the context - it can be seen
      virtual bool isVisible() const = 0;

      // Render the current context
      virtual void render() = 0;

      

      // Return the input handler for this context
      InputHandler* inputHandler() { return &_theInput; }

      // Return the audio handler for this context
      AudioHandler* audioHandler() { return &_theAudio; }


//      // Takes the focus from the current context and passes it to the new context
//      void transferFocus( Context* newContext );
//
//      // Takes the focus from the current context and passes it to the new context
//      void openContext( Context* newContext );

      // Called by the manager once a context is pushed onto the front of the context stack
      void giveFocus();

      // Called by the manager to tell the derived class that it that focus has returned from a subsequent context
      virtual void returnFocus();

      // Called by the manager immediately before a context is popped from the front of the context stack
      void takeFocus();


      // Returns the context event for a given name
      // This is how derived types allocate possible context events for buttons, etc to use
      ContextEvent getContextEventID( std::string ) const;

      // Return the ID number of registered (or default) context event
      void registerContextEvent( std::string );

      // Raise an event in the current context
      virtual void raiseContextEvent( ContextEvent ) = 0;
  };

}

#endif // REGOLITH_CONTEXT_H_

