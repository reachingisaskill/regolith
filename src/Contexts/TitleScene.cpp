
#include "Regolith/Contexts/TitleScene.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  TitleScene::TitleScene() :
    Context(),
    _defaultMusic( 0 ),
    _firstContext( 0 )
  {
    // Don't want to be able to pause a title scene when it looses focus
    setPauseable( false );
  }


  TitleScene::~TitleScene()
  {
  }


  void TitleScene::onStart()
  {
    if ( _defaultMusic != 0 )
      audioHandler()->setSong( _defaultMusic );

    Manager::getInstance()->openContext( _firstContext );
  }


  void TitleScene::onStop()
  {
    audioHandler()->stopSong();
  }


  void TitleScene::configure( Json::Value& json_data, ContextGroup& handler )
  {
    // Call the base class variant first
    Context::configure( json_data, handler );

    Utilities::validateJson( json_data, "default_music", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "menu", Utilities::JSON_TYPE_STRING );

    // Set the default music
    std::string default_music = json_data["default_music"].asString();
    INFO_STREAM << "Setting default music: " << default_music;
    _defaultMusic = audioHandler()->getMusicID( default_music );

    // Load the menu context - required for a title!
    std::string menu_name = json_data["menu"].asString();
    INFO_STREAM << "Setting title scene menu to: " << menu_name;
    _firstContext = Manager::getInstance()->getContextManager().requestContext( menu_name );
  }


  void TitleScene::validate() const
  {
    if ( Manager::getInstance()->getContextManager().getContext( _firstContext ) == nullptr )
    {
      Exception ex( "TitleScene::validate() const", "Menu context not configured" );
      ex.addDetail( "Name", Manager::getInstance()->getContextManager().getContextName( _firstContext ) );
      throw ex;
    }
  }


  void TitleScene::registerEvents( InputManager& manager )
  {
//    manager.registerEventRequest( this, REGOLITH_EVENT_QUIT );
    manager.registerEventRequest( this, REGOLITH_EVENT_CONTEXT_END );
  }


  void TitleScene::eventAction( const RegolithEvent& regolith_event, const SDL_Event& /*sdl_event*/ )
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

