
#include "Regolith/Contexts/TitleScene.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  TitleScene::TitleScene() :
    Context(),
    _defaultMusic( 0 )
  {
    // Don't want to be able to pause a title scene when it looses focus
    setPauseable( false );
  }


  TitleScene::~TitleScene()
  {
  }


  void TitleScene::onStart()
  {
    audioHandler()->setSong( _defaultMusic );
  }


  void TitleScene::onStop()
  {
    audioHandler()->stopSong();
  }


  void TitleScene::configure( Json::Value& json_data )
  {
    Utilities::validateJson( json_data, "default_music", Utilities::JSON_TYPE_STRING );

    // Set the default music
    std::string default_music = json_data["default_music"].asString();
    INFO_STREAM << "Setting default music: " << default_music;
    _defaultMusic = audioHandler()->getMusicID( default_music );

    // Call the base class variant to finish the configuration
    Context::configure( json_data );
  }


  void TitleScene::registerEvents( InputManager& manager )
  {
    manager.registerEventRequest( this, REGOLITH_EVENT_QUIT );
    manager.registerEventRequest( this, REGOLITH_EVENT_CONTEXT_END );
  }


  void TitleScene::eventAction( const RegolithEvent& regolith_event, const SDL_Event& /*sdl_event*/ )
  {
    switch ( regolith_event )
    {
      case REGOLITH_EVENT_QUIT :
      case REGOLITH_EVENT_CONTEXT_END :
        Manager::getInstance()->closeContext();
        break;

      default:
        break;
    }
  }
}

