
#ifndef REGOLITH_MANAGERS_AUDIO_HANDLER_H_
#define REGOLITH_MANAGERS_AUDIO_HANDLER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Managers/AudioManager.h"


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
    typedef std::vector<int> ChannelLookup;
    private:
      AudioManager& _manager;

      ChannelLookup _channels;

      MusicState _state;

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
      void registerChunk( unsigned int );


      // Return a pointer to the audio manager
      AudioManager& getManager() { return _manager; }


      // Get the id for a track from the manager
      unsigned int getMusicID( std::string name ) { return _manager.getMusicID( name ); }

      // Get the id number for a sound effect from the handler
      unsigned int getEffectID( std::string name ) { return _manager.getEffectID( name ); }


      // Set the scene-music using the manager
      void setSong( unsigned int n );

      // Play a sound effect in the local context
      void triggerEffect( unsigned int n );


      // Pause all sound effects
      void pause();

      // Play/continue playing all sound effects
      void play();

      // Stop all sound effects
      void stop();
  };

}

#endif // REGOLITH_MANAGERS_AUDIO_HANDLER_H_

