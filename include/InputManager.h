
#ifndef __REGOLITH__INPUT_HANDLER_H__
#define __REGOLITH__INPUT_HANDLER_H__

#include "Definitions.h"

#include "InputMapping.h"

#include <set>


namespace Regolith
{

  class InputManager;
  class InputHandler;
  typedef std::set< Controllable* > ControllableSet;

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Input Manager class

  /*
   * Defines the entry point for the input handling system.
   * Stores the global key mapping configuration and engine/game wide event call backs: INPUT_EVENTS
   * Additional callbacks are registered to the Scene's individual input handler
   */
  class InputManager
  {
    // Make this class non-copyable
    InputManager( const InputManager& ) = delete;
    InputManager& operator=( const InputManager& ) = delete;

    private:
      // Map the InputEventType to the InputMapping objects
      // This class owns this memory
      InputMapping* _inputMaps[ INPUT_TYPE_TOTAL ];

      // Map the input actions to the list of drawable objects that request callbacks
      ControllableSet _eventMaps[ REGOLITH_EVENT_TOTAL ];

      // Cache the SDL event
      SDL_Event _theEvent;


    public:
      InputManager();

      virtual ~InputManager();

      // Iterate through all the SDL events and call the registered event handlers
      void handleEvents( InputHandler* );

      // Hande a specific SDL event object
      void handleEvent( SDL_Event&, InputHandler* );

      // Set a new input map object
      void addInputMap( InputEventType, InputMapping* );

      // Register the request from a drawable object to be called when a given event is raised
      void registerInputRequest( Controllable*, RegolithEvent );

      // Return the set of objects that requested the input action
      ControllableSet& getRegisteredObjects( RegolithEvent );

      // Register an action to a specific input code on a specific input event type (hardware!)
      void registerBehaviour( InputEventType, unsigned int, InputBehaviour );

      // Return the event current registered to current event type
      InputBehaviour getRegisteredBehaviour( InputEventType, unsigned int );
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Input Handler class

  /*
   * Defines the input mapping for scene-related funcionality: INPUT_ACTIONS
   * is passed to the manager to provide access to the callback functions available in the current context
   */
  class InputHandler
  {
    private:
      ControllableSet _actionMaps[ INPUT_ACTION_TOTAL ];

    public :
      InputHandler();


      // Return the set of objects that requested the input action
      ControllableSet& getRegisteredObjects( InputAction action ) { return _actionMaps[ action ]; }

      // Register the request from a drawable object to be called when a given event is raised
      void registerInputRequest( Controllable*, InputAction );

  };

}

#endif // __REGOLITH__INPUT_HANDLER_H__

