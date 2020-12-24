
#include "Regolith/Audio/Music.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  Music::Music() :
    _rawMusic( nullptr ),
    _playCount( 0 )
  {
  }


  void Music::configure( Json::Value& json_data, DataHandler& handler )
  {
    Utilities::validateJson( json_data, "music_track", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "play_count", Utilities::JSON_TYPE_INTEGER );

    std::string music_name = json_data["music_track"].asString();
    _rawMusic = handler.getRawMusic( music_name );

    _playCount = (unsigned int) json_data["play_count"].asInt();
  }

}

