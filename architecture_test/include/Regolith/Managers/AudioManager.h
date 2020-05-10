
#ifndef REGOLITH_AUDIO_MANAGER_H_
#define REGOLITH_AUDIO_MANAGER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Utilities/NamedVector.h"



namespace Regolith
{
  // Forward declarations
  class AudioManager;
  class AudioHandler;


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Global Manager Class

  /*
   * The global audio manager.
   * This class loads and handles all memory relating to music and sound effects.
   * In the future I may look at a dynamic loading algorithm, however for now we just
   * load every file straight into memory
   */
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

      NamedVector<Mix_Music, false> _musics;
      NamedVector<Mix_Chunk, false> _effects;

      float _volumeMusic;
      float _volumeChunk;

      static Mix_Music* nextTrack;


     protected :
      // Add music file using a json object
      void addMusic( Json::Value& );

      // Add an effect using a json object
      void addEffect( Json::Value& );

    public :
      // Contstructor
      AudioManager( unsigned int, int, int, Uint16 format = MIX_DEFAULT_FORMAT );

      // Destructor
      ~AudioManager();


      // Build the audio handler and load all the files
      void configure( Json::Value&, Json::Value& );



      // Return the ID number for the request track/sound effect
      unsigned int getMusicID( std::string name ) { return _musics.getID( name ); }
      unsigned int getEffectID( std::string name ) { return _effects.getID( name ); }



      // Set the volumes
      void setVolumeMusic( float );
      void setVolumeEffects( float );

      // Get the current volumes
      float getVolumeMusic() { return _volumeMusic; }
      float getVolumeEffects() { return _volumeChunk; }



      // Play a new music track
      void playTrack( unsigned int );
      void playTrack( std::string n ) { playTrack( _musics.getID( n ) ); }

      // Play a sound effect
      void playChunk( unsigned int );
      void playChunk( std::string n ) { playChunk( _effects.getID( n ) ); }


      // Get/Set the fade time for music
      int getFadeTime() const { return _fadeTime; }
      void setFadeTime( int t ) { _fadeTime = t; }

  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Handler for specific context

  /*
   * This class acts like the SDL Mix Group.
   * It records all of the sound effects required by a specific context and organises the playing and 
   * pausing of them as different contexts gain and lose focus
   */
  class AudioHandler
  {
    typedef std::vector<int> ChannelLookup;
    private:
      AudioManager* _manager;

      ChannelLookup _channels;

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


      // Tell the handler it has control of this effect
      void registerChunk( unsigned int );


      // Return a pointer to the audio manager
      AudioManager* getManager() { return _manager; }


      // Get the id for a track from the manager
      unsigned int getMusicID( std::string name ) { return _manager->getMusicID( name ); }

      // Get the id number for a sound effect from the handler
      unsigned int getEffectID( std::string name ) { return _manager->getEffectID( name ); }


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


