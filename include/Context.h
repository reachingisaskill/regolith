
#ifndef REGOLITH_CONTEXT_H_
#define REGOLITH_CONTEXT_H_

#include "Definitions.h"
#include "Controllable.h"
#include "InputManager.h"
#include "AudioManager.h"


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
    private:
      // Pointer to the Input manager owned input handler
      InputHandler* _theInput;
      AudioHandler _theAudio;

    public:
      explicit Context( InputHandler* h = nullptr );

      virtual ~Context() {}

      // This function MUST be called durng the configuration of the class if any user input is to be received.
      void registerInputHandler( InputHandler* h ) { _theInput = h; }


      // Flag to see it needs updating with the current time
      virtual bool isAnimated() const = 0;

      // Update the animations, etc
      virtual void update( Uint32 ) = 0;


      // Flag to tell the engine to render the context - it can be seen
      virtual bool isVisible() const = 0;

      // Render the current context
      virtual void render() = 0;

      

      // Return the input handler for this context
      InputHandler* inputHandler() { return _theInput; }

      // Return the audio handler for this context
      AudioHandler* audioHandler() { return &_theAudio; }


      // Called by the manager to tell the derived class that it now has focus again
      virtual void returnFocus();

      // Takes the focus from the current context and passes it to the new context
      void transferFocus( Context* newContext );

      // Pushes current context onto the context stack - now has focus
      void giveFocus();

      // Pops the class from the context stack
      void takeFocus();



      // Raise an event in the current context
      virtual void raiseContextEvent( ContextEvent ) = 0;
  };

}

#endif // REGOLITH_CONTEXT_H_

