
#ifndef REGOLITH_AUDIO_PLAYLIST_H_
#define REGOLITH_AUDIO_PLAYLIST_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Audio/Music.h"
#include "Regolith/Handlers/DataHandler.h"


namespace Regolith
{

  class Playlist
  {
    // Audio manager must extract music list
    friend class AudioManager;

    // Useful typedef
    typedef std::list< Music > MusicList;

    private:
      // Ordered list of tracks to play
      MusicList _musics;

    public:
      // Con/De-structors
      Playlist();
      ~Playlist();


      // Configure the playlist
      void configure( Json::Value&, DataHandler& );

      // Clear the contents
      void clear();


      // Push an item to the back of the playlist. An alternative to the Json configuration - should rarely be used.
      void push( Music );

  };

}

#endif // REGOLITH_AUDIO_PLAYLIST_H_

