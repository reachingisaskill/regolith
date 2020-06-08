
#include "Regolith/Contexts/LoadScreen.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  LoadScreen::LoadScreen() :
    Context(),
    _theMusic( 0 )
  {
  }


  LoadScreen::~LoadScreen()
  {
  }


  void LoadScreen::onStart()
  {
    if ( _theMusic != 0 )
      owner()->getAudioHandler().setSong( _theMusic );
  }


  void LoadScreen::onStop()
  {
    if ( _theMusic != 0 )
      owner()->getAudioHandler().stopSong();
  }


  void LoadScreen::updateContext( float /*time*/ )
  {
    if ( Manager::getInstance()->getContextManager().isLoaded() )
    {
      Manager::getInstance()->getContextManager().startContextGroup();
    }
  }


  void LoadScreen::configure( Json::Value& json_data, ContextGroup& handler )
  {
    json_data["data_handler"] = std::string( "global" );

    // Call the base class variant first
    Context::configure( json_data, handler );

    Utilities::validateJson( json_data, "default_music", Utilities::JSON_TYPE_STRING );

    // Set the default music
    std::string default_music = json_data["default_music"].asString();
    INFO_STREAM << "Setting default music: " << default_music;
    _theMusic = owner()->getAudioHandler().getMusicID( default_music );
  }

}

