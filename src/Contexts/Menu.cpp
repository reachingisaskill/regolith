
#include "Regolith/Contexts/Menu.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Utilities/JsonValidation.h"

#include "logtastic.h"


namespace Regolith
{

  MenuContext::MenuContext() :
    Context()
  {
    // Don't want to be able to pause a title scene when it looses focus
    setPauseable( false );
  }


  MenuContext::~MenuContext()
  {
  }


  void MenuContext::onStart()
  {
  }


  void MenuContext::onStop()
  {
  }


  void MenuContext::configure( Json::Value& json_data, ContextGroup& handler )
  {
    Context::configure( json_data, handler );
  }


  void MenuContext::registerActions( InputHandler& handler )
  {
    handler.registerInputRequest( this, INPUT_ACTION_BACK );
    handler.registerInputRequest( this, INPUT_ACTION_RETURN );
    handler.registerInputRequest( this, INPUT_ACTION_CANCEL );
  }


  void MenuContext::inputAction( const InputAction& action )
  {
    switch ( action )
    {
      case INPUT_ACTION_BACK :
      case INPUT_ACTION_RETURN :
      case INPUT_ACTION_CANCEL :
        Manager::getInstance()->closeContext();
        break;

      default :
        break;
    }
  }


  void MenuContext::booleanAction( const InputAction& action, bool value )
  {
    // Immediately call the input action function if a key is released.
    // I think this is the simplest implementation
    if ( ! value ) inputAction( action );
  }


  void MenuContext::registerEvents( InputManager& manager )
  {
//    manager.registerEventRequest( this, REGOLITH_EVENT_QUIT );
    manager.registerEventRequest( this, REGOLITH_EVENT_CONTEXT_END );
  }


  void MenuContext::eventAction( const RegolithEvent& regolith_event, const SDL_Event& /*sdl_event*/ )
  {
    switch ( regolith_event )
    {
//      case REGOLITH_EVENT_QUIT :
      case REGOLITH_EVENT_CONTEXT_END :
        Manager::getInstance()->closeContext();
        break;

      default:
        break;
    }
  }
}

