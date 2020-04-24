
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
    typedef std::set< Drawable* > DrawableSet;

    // Make this class non-copyable
    InputHandler( const InputHandler& ) = delete;
    InputHandler& operator=( const InputHandler& ) = delete;

    private:
      // Map the InputEventType to the InputMapping objects
      // This class owns this memory
      InputMapping_base* _inputMaps[ INPUT_EVENT_TOTAL ];

      // Map the input actions to the list of drawable objects that request callbacks
      DrawableSet _actionMaps[ INPUT_ACTION_TOTAL ];

      // Cache the SDL event
      SDL_Event _theEvent;


    public:
      InputHandler();

      virtual ~InputHandler();

      void handleEvents();

      void handleEvent( SDL_Event& );

      void addInputMap( InputEventType, InputMapping_base* );

      void registerInputRequest( Drawable*, InputAction );
  };
}

#endif // __REGOLITH__INPUT_HANDLER_H__

