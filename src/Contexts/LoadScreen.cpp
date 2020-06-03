
#include "Regolith/Contexts/LoadScreen.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  LoadScreen::LoadScreen() :
    Context(),
    _theMusic( 0 ),
    _nextContext( 0 ),
    _nextContextHandler( 0 ),
    _running( false )
  {
  }


  LoadScreen::~LoadScreen()
  {
  }


  void LoadScreen::onStart()
  {
    if ( _theMusic != 0 )
      audioHandler()->setSong( _theMusic );

    _running = true;
  }


  void LoadScreen::onStop()
  {
    audioHandler()->stopSong();

    _running = false;
  }


  void LoadScreen::configure( Json::Value& json_data, ContextHandler& handler )
  {
    // Call the base class variant first
    Context::configure( json_data, handler );

    Utilities::validateJson( json_data, "default_music", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "menu", Utilities::JSON_TYPE_STRING );

    // Set the default music
    std::string default_music = json_data["default_music"].asString();
    INFO_STREAM << "Setting default music: " << default_music;
    _theMusic = audioHandler()->getMusicID( default_music );

    // Load the menu context - required for a title!
    std::string menu_name = json_data["menu"].asString();
    INFO_STREAM << "Setting title scene menu to: " << menu_name;
    _nextContext = Manager::getInstance()->getContextManager().requestContext( menu_name );
  }


  void LoadScreen::validate() const
  {
    if ( Manager::getInstance()->getContextManager().getContext( _nextContext ) == nullptr )
    {
      Exception ex( "LoadScreen::validate() const", "Following context not configured" );
      ex.addDetail( "Name", Manager::getInstance()->getContextManager().getContextName( _nextContext ) );
      throw ex;
    }
  }


  void LoadScreen::registerEvents( InputManager& manager )
  {
    manager.registerEventRequest( this, REGOLITH_EVENT_DATA_LOADED );
  }


  void LoadScreen::eventAction( const RegolithEvent& regolith_event, const SDL_Event& /*sdl_event*/ )
  {
    if ( ! _running ) return;

    switch ( regolith_event )
    {
      case REGOLITH_EVENT_DATA_LOADED :
        Manager::getInstance()->setContextStack( _nextContext );
        break;

      default:
        break;
    }
  }

}

