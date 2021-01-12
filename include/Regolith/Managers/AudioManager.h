
#ifndef REGOLITH_MANAGERS_AUDIO_MANAGER_H_
#define REGOLITH_MANAGERS_AUDIO_MANAGER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/Component.h"
#include "Regolith/Utilities/MutexedBuffer.h"

#include <utility>

namespace Regolith
{
  // Forward declarations
  class AudioManager;
  class AudioHandler;
  class Playlist;
  class Music;

////////////////////////////////////////////////////////////////////////////////////////////////////

  /*
   * Global Manager Class
   *
   * The global audio manager.
   * This class loads and handles all memory relating to music and sound effects.
   * In the future I may look at a dynamic loading algorithm, however for now we just
   * load every file straight into memory
   */
  class AudioManager : public Component
  {
    // Allow links to access the private members
    template < class T, class R > friend class Link;

    // Typedefs for the music queuing system
    typedef std::pair< Music*, unsigned int > QueueElement;
    typedef MutexedBuffer< QueueElement > MusicQueue;

    enum class MusicOperation { PLAY, STOP, NEXT };

    // Make the play-next function a friend
    friend void playNextTrack();

    private :
      unsigned int _frequency;
      Uint16 _format;
      int _channels;
      int _chunkSize;
      int _fadeTime;
      int _playbackChannelCounter;

      float _volumeMusic;
      float _volumeChunk;

      // Queue of upcoming music tracks
      static MusicQueue _musicQueue;
      static QueueElement _currentTrack;
      static MusicOperation _operation;
      static std::mutex _operationMutex;


    protected:

      // Add a track to the back of the queue
      void queueTrack( Music* );

      // Clear the current queue and play this track instead
      void playTrack( Music* );

      // Clear the queue
      void clearQueue();

      // Stop the the audio if its playing
      void stopTrack();


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


      // Music interface

      // Play the playlist
      void load( Playlist* );

      // Pause the the audio if its playing
      void pauseTrack();

      // Resume the paused audio
      void resumeTrack();

      // Stop the infinitely repeating track
      void stopRepeatTrack();

      // Stop the current track and move on to the next one in the queue
      void nextTrack();

      // Skip to the next track at the end of the current repeat
      void nextRepeatTrack();


////////////////////////////////////////////////////////////////////////////////
      // Component Interface
      // Register game-wide events with the manager
      virtual void registerEvents( InputManager& ) override {}

      // Regolith events
      virtual void eventAction( const RegolithEvent&, const SDL_Event& ) override {}

  };

}

#endif // REGOLITH_MANAGERS_AUDIO_MANAGER_H_


