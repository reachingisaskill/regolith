
#include "Regolith/GamePlay/Sound.h"
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
    Utilities::validateJson( json_data, "sound_name", Utilities::JSON_TYPE_STRING );
    std::string sound_name = json_data["sound_name"].asString();

    _rawSound = handler.getRawSound( sound_name );
  }

}

