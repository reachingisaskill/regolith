
#ifndef REGOLITH_AUDIO_PLAYLIST_H_
#define REGOLITH_AUDIO_PLAYLIST_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Audio/Music.h"
#include "Regolith/Managers/DataHandler.h"


namespace Regolith
{

  class Playlist
  {
    typedef std::list< Music > MusicList;
    private:
      MusicList _musics;

    public:
      Playlist();

      ~Playlist();


      // Configure the playlist
      void configure( Json::Value&, DataHandler& );

      // Clear the contents
      void clear();


      // Clears the current AudioManager music queue and pushes the play list
      void play();

      // Push an item to the back of the playlist
      void push( Music );

  };

}

#endif // REGOLITH_AUDIO_PLAYLIST_H_

