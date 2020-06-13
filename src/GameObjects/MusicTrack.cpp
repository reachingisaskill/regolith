
#include "Regolith/GameObjects/MusicTrack.h"
#include "Regolith/Utilities/JsonValidation.h"
#include "Regolith/Managers/DataHandler.h"


namespace Regolith
{

  MusicTrack::MusicTrack() :
    _track( nullptr )
  {
  }


  MusicTrack::~MusicTrack()
  {
  }


  void MusicTrack::configure( Json::Value& json_data, ContextGroup&, DataHandler& handler )
  {
    // Nothing to configure before this class
//    GameObject::configure( json_data, handler );

    Utilities::validateJson( json_data, "track_name", Utilities::JSON_TYPE_STRING );

    _track = handler.getRawMusic( json_data["track_name"].asString() );
  }

}

