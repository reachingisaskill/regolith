
#include "Architecture/Context.h"
#include "Managers/Manager.h"


namespace Regolith
{

  Context::Context( std::string mappingName ) :
    Controllable(),
    _theInput( mappingName ),
    _theAudio( Manager::getInstance()->getAudioManager() ),
    _contextEvents()
  {
  }


  void Context::giveFocus()
  {
    _theAudio.play();
  }


//  void Context::openContext( Context* newContext )
//  {
//    _theAudio.pause();
//    newContext->giveFocus();
//  }
//
//
//  void Context::transferFocus( Context* newContext )
//  {
//    _theAudio.stop();
//    Manager::getInstance()->popContext();
//    newContext->giveFocus();
//  }


  void Context::takeFocus()
  {
    _theAudio.stop();
  }


  void Context::returnFocus()
  {
    _theAudio.play();
  }


  void Context::registerContextEvent( std::string event_name )
  {
    if ( _contextEvents.find( event_name ) == _contextEvents.end() )
    {
      ERROR_LOG( "Context event already registered. Ignoring" );
    }
    else
    {
      _contextEvents[ event_name ] = _contextEvents.size() + (unsigned)CONTEXT_EVENT_OPTIONS;
    }
  }


  ContextEvent Context::getContextEventID( std::string event_name ) const
  {
    ContextEvent event_id = getContextEventID( event_name );
    if ( event_id == CONTEXT_EVENT_OPTIONS )
    {
      std::map< std::string, unsigned int >::const_iterator found = _contextEvents.find( event_name );
      if ( found == _contextEvents.end() )
      {
        FAILURE_STREAM << "Request for context event that does not exist: " << event_name;
        Exception ex( "Context::getContextEventID()", "Context event does not exist" );
        ex.addDetail( "Event name", event_name );
        throw ex;
      }
      event_id = (ContextEvent)((unsigned int) event_id + found->second);
    }

    return event_id;
  }

}

