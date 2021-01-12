
#include "Regolith/Audio/Music.h"
#include "Regolith/Handlers/DataHandler.h"
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
    validateJson( json_data, "music_track", JsonType::STRING );
    validateJson( json_data, "play_count", JsonType::INTEGER );

    std::string music_name = json_data["music_track"].asString();
    _rawMusic = handler.getRawMusic( music_name );

    _playCount = (unsigned int) json_data["play_count"].asInt();
  }

}

