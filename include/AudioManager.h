
#ifndef REGOLITH_AUDIO_MANAGER_H_
#define REGOLITH_AUDIO_MANAGER_H_

#include "Definitions.h"
#include "NamedVector.h"

#include <json/json.h>


namespace Regolith
{
  // Forward declarations
  class AudioManager;
  class AudioHandler;


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Global Manager Class
  class AudioManager
  {
    friend void playNextTrack();

    private :
      unsigned int _frequency;
      Uint16 _format;
      int _channels;
      int _chunkSize;
      int _fadeTime;
      int _playbackChannelCounter;

      NamedVector<Mix_Music> _musics;

      float _volumeMusic;
      float _volumeChunk;

      static Mix_Music* nextTrack;

    public :
      AudioManager( unsigned int, int, int, Uint16 format = MIX_DEFAULT_FORMAT );

      ~AudioManager();

      void addMusic( Json::Value& );
      unsigned int getMusicID( std::string name ) { return _musics.getID( name ); }

      // Set the volumes
      void setVolumeMusic( float );
      void setVolumeEffects( float );

      // Get the current volumes
      float getVolumeMusic() { return _volumeMusic; }
      float getVolumeEffects() { return _volumeChunk; }


      // Play a new music track
      void playTrack( unsigned int n );
      void playTrack( std::string n ) { playTrack( _musics.getID( n ) ); }


      // Get/Set the fade time for music
      int getFadeTime() const { return _fadeTime; }
      void setFadeTime( int t ) { _fadeTime = t; }


      // Allocates n channels and returns the startiing number
      int reserveChannels( int n );
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Handler for specific context

  class AudioHandler
  {
    typedef std::vector<int> ChannelLookup;
    private:
      AudioManager* _manager;

      NamedVector<Mix_Chunk> _effects;
      ChannelLookup _channels;
      int _channelGroup;

      MusicState _state;

    protected:
      void _pauseAll();
      void _resumeAll();
      void _stopAll();

    public:
      // Constructor
      AudioHandler( AudioManager* );

      // Destructor
      ~AudioHandler();


      // Configure the handler and request channels from the manager
      void configure();


      // Return a pointer to the audio manager
      AudioManager* getManager() { return _manager; }



      // Add an effect using a json object
      void addEffect( Json::Value& );


      // Get the id for a track from the manager
      unsigned int getMusicID( std::string name ) { return _manager->getMusicID( name ); }

      // Get the id number for a sound effect from the handler
      unsigned int getEffectID( std::string name ) { return _effects.getID( name ); }


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

#endif // REGOLITH_AUDIO_MANAGER_H_


