
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
    DEBUG_LOG( "Starting Menu" );
  }


  void MenuContext::onStop()
  {
    audioHandler()->stopSong();
  }


  void MenuContext::configure( Json::Value& json_data )
  {
    Context::configure( json_data );
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

