
#ifndef __REGOLITH__INPUT_HANDLER_H__
#define __REGOLITH__INPUT_HANDLER_H__

#include "Definitions.h"

#include "InputMapping.h"

#include <set>


namespace Regolith
{

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Input Handling class

  class InputHandler
  {
    // Make this class non-copyable
    InputHandler( const InputHandler& ) = delete;
    InputHandler& operator=( const InputHandler& ) = delete;

    public:
      typedef std::set< Controllable* > DrawableSet;

    private:
      // Map the InputEventType to the InputMapping objects
      // This class owns this memory
      InputMapping* _inputMaps[ INPUT_EVENT_TOTAL ];

      // Map the input actions to the list of drawable objects that request callbacks
      DrawableSet _actionMaps[ INPUT_ACTION_TOTAL ];

      // Cache the SDL event
      SDL_Event _theEvent;


    public:
      InputHandler();

      virtual ~InputHandler();

      // Iterate through all the SDL events and call the registered event handlers
      void handleEvents();

      // Hande a specific SDL event object
      void handleEvent( SDL_Event& );

      // Set a new input map object
      void addInputMap( InputEventType, InputMapping* );

      // Register the request from a drawable object to be called when a given action is raised
      void registerInputRequest( Controllable*, InputAction );

      // Return the set of objects that requested the input action
      DrawableSet getRegisteredObjects( InputAction );

      // Register an action to a specific input code on a specific input event type (hardware!)
      void registerAction( InputEventType, unsigned int, InputAction );

      InputAction getRegisteredAction( InputEventType, unsigned int );
  };
}

#endif // __REGOLITH__INPUT_HANDLER_H__

