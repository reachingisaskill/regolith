
#ifndef REGOLITH_MANAGERS_AUDIO_HANDLER_H_
#define REGOLITH_MANAGERS_AUDIO_HANDLER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Managers/AudioManager.h"
#include "Regolith/Utilities/ProxyMap.h"
#include "Regolith/Audio/Sound.h"
#include "Regolith/Audio/Music.h"
#include "Regolith/Audio/Playlist.h"


namespace Regolith
{
  class DataHandler;

  /*
   * Handler for specific context
   *
   * This class acts like the SDL Mix Group.
   * It records all of the sound effects required by a specific context and organises the playing and 
   * pausing of them as different contexts gain and lose focus
   */
  class AudioHandler
  {
    typedef std::vector<int> ChannelPauses;
    typedef std::map<std::string, Playlist> PlaylistMap;

    private:
      // Reference to the manager
      AudioManager& _manager;

      // Rememeber the current handler state
      MusicState _state;

      // Vector to rememeber the individual channel statuses
      ChannelPauses _channelPauses;

      // Map of all playlists
      PlaylistMap _playlists;

    protected:
      void _pauseAll();
      void _resumeAll();
      void _stopAll();

    public:
      // Constructor
      AudioHandler( AudioManager& );

      // Destructor
      ~AudioHandler();


      // Load all playlists, etc
      void configure( Json::Value&, DataHandler& );

      // Allocate audio channels for the objects
      void initialise();


      // Allocates a channel for the provided sound
      void requestChannel( Sound& );


      // Returns a reference to a playlist
      Playlist& getPlaylist( std::string );


      // Play a track immediately
      void playSong( Music* );

      // Queue a track to play next
      void queueSong( Music* );

      // Stop the music playing
      void stopSong();


      // Play a sound effect in the local context
      void playSound( Sound& );


      // Pause all sound effects
      void pause();

      // Play/continue playing all sound effects
      void play();

      // Stop all sound effects
      void stop();
  };

}

#endif // REGOLITH_MANAGERS_AUDIO_HANDLER_H_

