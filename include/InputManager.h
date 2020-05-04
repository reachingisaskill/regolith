
#ifndef REGOLITH_INPUT_HANDLER_H_
#define REGOLITH_INPUT_HANDLER_H_

#include "Definitions.h"
#include "InputMapping.h"
#include "NamedVector.h"

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
      // Named vector of input handlers. Manager owns their memory
      NamedVector<InputHandler, true> _inputHandlers;

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


      // Register the request from a drawable object to be called when a given event is raised
      void registerEventRequest( Controllable*, RegolithEvent );

      // Return the set of objects that requested the input action
      ControllableSet& getRegisteredObjects( RegolithEvent );


      // Register an action to a specific input code on a specific input event type (hardware!)
      void registerEvent( InputEventType, unsigned int, RegolithEvent );

      // Return the event current registered to current event type
      RegolithEvent getRegisteredEvent( InputEventType, unsigned int );


      // Requests an input handler. If it exists, return the pointer, otherwise create it.
      InputHandler* requestHandler( std::string );
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Input Handler class

  /*
   * Defines the input mapping for scene-related funcionality: INPUT_ACTIONS
   * is passed to the manager to provide access to the callback functions available in the current context
   */
  class InputHandler
  {
    friend class InputManager;
    private:
      // Map the array of mapping object - one for each input event type
      // This class owns this memory
      InputMapping* _inputMaps[ INPUT_TYPE_TOTAL ];

      // Array of sets of controllable objects - one for each input action
      ControllableSet _actionMaps[ INPUT_ACTION_TOTAL ];

    public :
      InputHandler();

      ~InputHandler();

      // Return the set of objects that requested the input action
      ControllableSet& getRegisteredObjects( InputAction action ) { return _actionMaps[ action ]; }

      // Register the request from a drawable object to be called when a given event is raised
      void registerInputRequest( Controllable*, InputAction );



      // Register a user action (event+mapping id) to an input action
      void registerInputAction( InputEventType, unsigned int, InputAction );

      // Return the registered action for a given event type and mapping id
      InputAction getRegisteredInputAction( InputEventType, unsigned int );

  };

}

#endif // REGOLITH_INPUT_HANDLER_H_

