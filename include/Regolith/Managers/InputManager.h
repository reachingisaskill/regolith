
#ifndef REGOLITH_MANAGERS_INPUT_MANAGER_H_
#define REGOLITH_MANAGERS_INPUT_MANAGER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/ControllableInterface.h"
#include "Regolith/Architecture/Component.h"
#include "Regolith/Managers/InputMapping.h"
#include "Regolith/Utilities/NamedVector.h"

#include <set>


namespace Regolith
{

  class InputManager;
  class InputHandler;
  typedef std::set< ControllableInterface* > ControllableSet;
  typedef std::set< Component* > ComponentSet;


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
    // Friend declarations
    template < class T, class R > friend class Link;

    // Make this class non-copyable
    InputManager( const InputManager& ) = delete;
    InputManager& operator=( const InputManager& ) = delete;

    private:
      // Named vector of individual mappings. Manager owns their memory
      NamedVector<InputMappingSet, true> _inputMappers;

      // Map the input actions to the list of drawable objects that request callbacks
      ComponentSet _eventMaps[ REGOLITH_EVENT_TOTAL ];

      // Cache the SDL event
      SDL_Event _theEvent;

      // Cache the pointer to the last used input handler. Used to simulate input actions
      InputHandler* _lastHandler;


    protected:

////////////////////////////////////////////////////////////////////////////////
      // Input Handler Access
      // Requests an input handler. If it exists, return the pointer, otherwise create it.
      InputMappingSet* requestMapping( std::string );


////////////////////////////////////////////////////////////////////////////////
      // Engine access

      // Iterate through all the SDL events and use the provided input handler to distribute user events
      void handleEvents( InputHandler* );


////////////////////////////////////////////////////////////////////////////////
      // Signal access

      // Functions to manually push events to objects without the SDL event queue
      void simulateInputAction( InputAction );
      void simulateBooleanAction( InputAction, bool );
      void simulateFloatAction( InputAction, float );
      void simulateVectorAction( InputAction, const Vector& );
      void simulateMouseAction( InputAction, bool, const Vector& );


    public:
      InputManager();

      virtual ~InputManager();


      // Initialise the Input mappings and handling configuration
      void configure( Json::Value& );




      // Register the request from a drawable object to be called when a given event is raised
      void registerEventRequest( Component*, RegolithEvent );

      // Return the set of objects that requested the input action
      ComponentSet& getRegisteredComponents( RegolithEvent );



      // Register an input action to the mapper with the proveded name
      void registerInputAction( std::string, InputEventType event_type, unsigned int code, InputAction event );

      // Get the registered input action from the request mapper, for a specific event type and event code
      InputAction getRegisteredInputAction( std::string, InputEventType event_type, unsigned int code );

  };


}

#endif // REGOLITH_MANAGERS_INPUT_MANAGER_H_

