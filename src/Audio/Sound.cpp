
#include "Regolith/Audio/Sound.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  Sound::Sound() :
    _rawSound( nullptr ),
    _channel( 0 )
  {
  }


  void Sound::configure( Json::Value& json_data, DataHandler& handler )
  {
    validateJson( json_data, "sound_name", JsonType::STRING );
    std::string sound_name = json_data["sound_name"].asString();

    _rawSound = handler.getRawSound( sound_name );

    DEBUG_STREAM << "Sound::configure : " << "Loaded sound : " << sound_name << " @ " << _rawSound << " -> " << _rawSound->sound;
  }

}

