
#ifndef REGOLITH_MANAGERS_AUDIO_MANAGER_H_
#define REGOLITH_MANAGERS_AUDIO_MANAGER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/GamePlay/Noises.h"


namespace Regolith
{
  // Forward declarations
  class AudioManager;
  class AudioHandler;

////////////////////////////////////////////////////////////////////////////////////////////////////

  /*
   * Global Manager Class
   *
   * The global audio manager.
   * This class loads and handles all memory relating to music and sound effects.
   * In the future I may look at a dynamic loading algorithm, however for now we just
   * load every file straight into memory
   */
  class AudioManager
  {
    private :
      unsigned int _frequency;
      Uint16 _format;
      int _channels;
      int _chunkSize;
      int _fadeTime;
      int _playbackChannelCounter;

      float _volumeMusic;
      float _volumeChunk;

    public :
      // Contstructor
      AudioManager();

      // Destructor
      ~AudioManager();

      // Clear the stored data
      void clear();

      // Build the audio handler and load all the files
      void configure( Json::Value& );


      // Set the volumes
      void setVolumeMusic( float );
      void setVolumeEffects( float );

      // Get the current volumes
      float getVolumeMusic() { return _volumeMusic; }
      float getVolumeEffects() { return _volumeChunk; }

      // Get/Set the fade time for music
      int getFadeTime() const { return _fadeTime; }
      void setFadeTime( int t ) { _fadeTime = t; }

  };

}

#endif // REGOLITH_MANAGERS_AUDIO_MANAGER_H_


