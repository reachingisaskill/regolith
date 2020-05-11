
#ifndef REGOLITH_MANAGERS_INPUT_HANDLER_H_
#define REGOLITH_MANAGERS_INPUT_HANDLER_H_

#include "Regolith/Managers/InputManager.h"

namespace Regolith
{

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
      void registerInputRequest( ControllableInterface*, InputAction );
  };

}

#endif //REGOLITH_MANAGERS_INPUT_HANDLER_H_ 

