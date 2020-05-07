
#ifndef REGOLITH_INPUT_HANDLER_H_
#define REGOLITH_INPUT_HANDLER_H_

#include "Managers/Definitions.h"
#include "Managers/InputMapping.h"
#include "Components/NamedVector.h"

#include <set>


namespace Regolith
{

  class InputManager;
  class InputHandler;
  typedef std::set< Controllable* > ControllableSet;


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Input Mapping Set

  /*
   * Defines a class that contains a unique set of input mappers for each context type
   */

  class InputMappingSet
  {
    InputMappingSet( const InputMappingSet& ) = delete;
    InputMappingSet& operator=( const InputMappingSet& ) = delete;
    public:
      InputMapping* mapping[ INPUT_TYPE_TOTAL ];

      InputMappingSet();
      ~InputMappingSet();
  };

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
//      NamedVector<InputHandler, true> _inputHandlers;
      NamedVector<InputMappingSet, true> _inputMappers;

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



      // Register an input action to the mapper with the proveded name
      void registerInputAction( std::string, InputEventType event_type, unsigned int code, InputAction event );

      // Get the registered input action from the request mapper, for a specific event type and event code
      InputAction getRegisteredInputAction( std::string, InputEventType event_type, unsigned int code );


      // Requests an input handler. If it exists, return the pointer, otherwise create it.
//      InputHandler* requestHandler( std::string );

      // Requests an input handler. If it exists, return the pointer, otherwise create it.
      InputMappingSet* requestMapping( std::string );


      // Functions to manually push events to objects without the SDL event queue
      void simulateInputAction( InputHandler*, InputAction );
      void simulateBooleanAction( InputHandler*, InputAction, bool );
      void simulateFloatAction( InputHandler*, InputAction, float );
      void simulateVectorAction( InputHandler*, InputAction, const Vector& );
      void simulateMouseAction( InputHandler*, InputAction, bool, const Vector& );
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
      InputMappingSet* _inputMaps;

      // Array of sets of controllable objects - one for each input action
      ControllableSet _actionMaps[ INPUT_ACTION_TOTAL ];

    public :
      // Just pass the name of the required input mapping
      explicit InputHandler( std::string );

      ~InputHandler();

      // Return the set of objects that requested the input action
      ControllableSet& getRegisteredObjects( InputAction action ) { return _actionMaps[ action ]; }

      // Register the request from a drawable object to be called when a given event is raised
      void registerInputRequest( Controllable*, InputAction );



//      // Register a user action (event+mapping id) to an input action
//      void registerInputAction( InputEventType, unsigned int, InputAction );
//
//      // Return the registered action for a given event type and mapping id
//      InputAction getRegisteredInputAction( InputEventType, unsigned int );
  };

}

#endif // REGOLITH_INPUT_HANDLER_H_

