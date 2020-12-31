
#include "Regolith/Audio/Playlist.h"
#include "Regolith/Utilities/JsonValidation.h"
#include "Regolith/Managers/Manager.h"
#include "Regolith/Managers/AudioManager.h"


namespace Regolith
{

  Playlist::Playlist() :
    _musics()
  {
  }


  Playlist::~Playlist()
  {
    _musics.clear();
  }


  void Playlist::configure( Json::Value& json_data, DataHandler& handler )
  {
    validateJson( json_data, "tracks", JsonType::ARRAY );
    validateJsonArray( json_data["tracks"], 0, JsonType::OBJECT );

    Json::Value& tracks = json_data["tracks"];
    for ( Json::ArrayIndex i = 0; i < tracks.size(); ++i )
    {
      DEBUG_LOG( "Playlist::configure : Adding track to playlist" );
      Music new_music;
      new_music.configure( tracks[i], handler );
      _musics.push_back( new_music );
    }
  }


  void Playlist::clear()
  {
    _musics.clear();
  }


  void Playlist::play()
  {
    AudioManager& manager = Manager::getInstance()->getAudioManager();

    // Clear the queue
    manager.clearQueue();

    // Refill it from the playlist
    for ( MusicList::iterator it = _musics.begin(); it != _musics.end(); ++it )
    {
      DEBUG_LOG( "Playlist::play : Queueing track" );
      manager.queueTrack( &(*it) );
    }

    // Start playing it
    manager.nextTrack();
  }


  void Playlist::push( Music music )
  {
    _musics.push_back( music );
  }

}

