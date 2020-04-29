
#ifndef __REGOLITH__CONTEXT_H__
#define __REGOLITH__CONTEXT_H__

#include "Definitions.h"
#include "InputManager.h"


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
  class Context
  {
    private:
      Context* _parent;
      InputHandler _theInput;

    public:
      Context() : _parent( nullptr ), _theInput() {}

      // Set the parent context
      void setParentContext( Context* p ) { _parent = p; }

      // Get the parent context
      Context* getParentContext() { return _parent; }

      // Return the input handler for this context
      InputHandler* inputHandler() { return &_theInput; }

      // Raise an event in the current context
      virtual void raiseContextEvent( ContextEvent ) = 0;

      // Class tells the manager that it is the current context
      virtual void giveFocus();
  };

}

#endif // __REGOLITH__CONTEXT_H__

