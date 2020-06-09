
#ifndef REGOLITH_MANAGERS_AUDIO_HANDLER_H_
#define REGOLITH_MANAGERS_AUDIO_HANDLER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Managers/AudioManager.h"
#include "Regolith/Utilities/ProxyMap.h"


namespace Regolith
{

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
    private:
      MusicState _state;

      ChannelPauses _channelPauses;

    protected:
      void _pauseAll();
      void _resumeAll();
      void _stopAll();

    public:
      // Constructor
      AudioHandler();

      // Destructor
      ~AudioHandler();


      // Tell the handler it has control of this effect
      void configure();

      // Allocates a channel for the provided sound
      void requestChannel( RawSound* );


      // Set the scene-music using the manager
      void setSong( RawMusic* );

      // Stop the music playing
      void stopSong();

      // Play a sound effect in the local context
      void playSound( RawSound* );


      // Pause all sound effects
      void pause();

      // Play/continue playing all sound effects
      void play();

      // Stop all sound effects
      void stop();
  };

}

#endif // REGOLITH_MANAGERS_AUDIO_HANDLER_H_

