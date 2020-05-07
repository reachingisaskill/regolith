
#include "Managers/AudioManager.h"
#include "Components/Exception.h"
#include "Components/Utilities.h"

#include "logtastic.h"

#include <cmath>


namespace Regolith
{

  // Definitions and local funcions
  Mix_Music* AudioManager::nextTrack = nullptr;

  // This function takes the next track stored in the audio manager and plays it when the previous one has finished
  void playNextTrack();

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Audio Manager member functions

  AudioManager::AudioManager( unsigned int freq, int chan, int chun, Uint16 format ) :
    _frequency( freq ),
    _format( format ),
    _channels( chan ),
    _chunkSize( chun ),
    _fadeTime( 2000 ),
    _playbackChannelCounter( 0 ),
    _musics( "music_files" ),
    _effects( "chunk_files" ),
    _volumeMusic( 1.0 ), // Defaults to full volume
    _volumeChunk( 1.0 ) // Defaults to full volume
  {
    if ( Mix_OpenAudio( _frequency, _format, _channels, _chunkSize ) == -1 )
    {
      FAILURE_STREAM << "Could not open audio device with the required configuration: " << _frequency << "Hz, " << _channels << " channels, " << _chunkSize << "byte chunks.";
      Exception ex( "AudioManager::AudioManager()", "Failed to initialise audio device" );
      ex.addDetail( "Mix Error", Mix_GetError() );
      ex.addDetail( "Frequency", _frequency );
      ex.addDetail( "Channels", _channels );
      ex.addDetail( "Chunk Size", _chunkSize );
      ex.addDetail( "Format", _format );
      throw ex;
    }

    INFO_STREAM << "Initialised Audio Device: " << _frequency << "Hz, " << _channels << " channels, " << _chunkSize << "byte chunks.";
  }


  AudioManager::~AudioManager()
  {
    // Drop the hook in case something happens
    Mix_HookMusicFinished( nullptr );
    nextTrack = nullptr;

    // Free all the memory used by the music files
    for ( unsigned int i = 0; i < _musics.vectorSize(); ++i )
    {
      Mix_FreeMusic( _musics[i] );
    }
    _musics.clear();

    // free all the chunks
    for ( unsigned int i = 0; i < _effects.size(); ++i )
    {
      Mix_FreeChunk( _effects[i] );
    }
    _effects.clear();


    Mix_CloseAudio();
  }


  void AudioManager::addMusic( Json::Value& json_data )
  {
    Utilities::validateJson( json_data, "name", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "path", Utilities::JSON_TYPE_STRING );

    std::string name = json_data["name"].asString();
    std::string path = json_data["path"].asString();

    Mix_Music* new_music = Mix_LoadMUS( path.c_str() );

    if ( new_music == nullptr )
    {
      std::string error = Mix_GetError();
      FAILURE_STREAM << "Could load music file: " << path;
      FAILURE_STREAM << "Mix Error: " << error;
      Exception ex( "AudioManager::addMusic()", "Failed to load music file" );
      ex.addDetail( "Name", name );
      ex.addDetail( "Path", path );
      ex.addDetail( "Mix Error", error );
      throw ex;
    }

    _musics.addObject( new_music, name );
  }


  void AudioManager::addEffect( Json::Value& json_data )
  {
    Utilities::validateJson( json_data, "name", Utilities::JSON_TYPE_STRING );
    Utilities::validateJson( json_data, "path", Utilities::JSON_TYPE_STRING );

    std::string name = json_data["name"].asString();
    std::string path = json_data["path"].asString();

    Mix_Chunk* new_chunk = Mix_LoadWAV( path.c_str() );

    if ( new_chunk == nullptr )
    {
      std::string error = Mix_GetError();
      FAILURE_STREAM << "Could load effect file: " << path;
      FAILURE_STREAM << "Mix Error: " << error;
      Exception ex( "AudioHandler::addEffect()", "Failed to load effect file" );
      ex.addDetail( "Name", name );
      ex.addDetail( "Path", path );
      ex.addDetail( "Mix Error", error );
      throw ex;
    }

    _effects.addObject( new_chunk, name );
  }


  void AudioManager::configure( Json::Value& music_files, Json::Value& effect_files )
  {
    INFO_LOG( "Audio Manager: Loading music files" );
    Json::ArrayIndex music_size = music_files.size();
    for ( Json::ArrayIndex i = 0; i < music_size; ++i )
    {
      this->addMusic( music_files[i] );
    }

    INFO_LOG( "Audio Manager: Loading sound effect files" );
    Json::ArrayIndex effect_size = effect_files.size();
    for ( Json::ArrayIndex i = 0; i < effect_size; ++i )
    {
      this->addEffect( effect_files[i] );
    }

    INFO_LOG( "Reserving Mixer Channels" );
    size_t result = Mix_AllocateChannels( _effects.size() );

    if ( result != _effects.size() )
    {
      std::string error = Mix_GetError();
      FAILURE_STREAM << "Could allocate the requested number of channels: " << _effects.size() << ". Received : " << result;
      FAILURE_STREAM << "SDL Mixer Error: " << error;
      Exception ex( "AudioHandler::configure()", "Could not allocate requested channels" );
      ex.addDetail( "Requested", _effects.size() );
      ex.addDetail( "Received", result );
      ex.addDetail( "Mix Error", error );
      throw ex;
    }
  }


  void AudioManager::setVolumeMusic( float v )
  {
    // Make sure it is within range
    if ( v > 1.0 || v < 0.0 )
    {
      WARN_STREAM << "Music Volume: " << v << " is out of range [0,1]. It will be truncated.";
      v = std::fabs( v );
      v = ( (v >1.0) ? 1.0 : v );
    }

    // Set the mixer volume
    Mix_VolumeMusic( (int) (v * MIX_MAX_VOLUME) );

    // Set the member variable
    _volumeMusic = v;
  }


  void AudioManager::setVolumeEffects( float v )
  {
    // Make sure it is within range
    if ( v > 1.0 || v < 0.0 )
    {
      WARN_STREAM << "Effects Volume: " << v << " is out of range [0,1]. It will be truncated.";
      v = std::fabs( v );
      v = ( (v >1.0) ? 1.0 : v );
    }

    // Set the mixer volume
    Mix_Volume( -1, (int) (v * MIX_MAX_VOLUME) );

    // Set the member variable
    _volumeChunk = v;
  }


  void AudioManager::playTrack( unsigned int i )
  {
    Mix_Music* track = _musics[i];

    if ( Mix_PlayingMusic() == 1 )
    {
      nextTrack = track;
      Mix_HookMusicFinished( playNextTrack );
      Mix_FadeOutMusic( _fadeTime );
    }
    else
    {
      Mix_PlayMusic( track, -1 );
    }
  }


  void AudioManager::playChunk( unsigned int i )
  {
    Mix_Chunk* chunk = _effects[i];

    if ( Mix_Playing( i ) ) // Sound already playing
    {
      return;
    }
    else
    {
      // Play the chunk exactly once on its reserved channel
      Mix_PlayChannel( i, chunk, 0 );
    }
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Audio Handler

  AudioHandler::AudioHandler( AudioManager* manager ) :
    _manager( manager ),
    _channels(),
    _state( MUSIC_STATE_STOPPED )
  {
  }


  AudioHandler::~AudioHandler()
  {
    _channels.clear();
  }


  void AudioHandler::registerChunk( unsigned int num )
  {
    _channels.push_back( num );
  }


  void AudioHandler::_pauseAll()
  {
    unsigned int numChannels = _channels.size();
    for ( unsigned int i = 0; i < numChannels; ++i )
    {
      Mix_Pause( _channels[i] );
    }
  }


  void AudioHandler::_resumeAll()
  {
    unsigned int numChannels = _channels.size();
    for ( unsigned int i = 0; i < numChannels; ++i )
    {
      Mix_Resume( _channels[i] );
    }
  }


  void AudioHandler::_stopAll()
  {
    unsigned int numChannels = _channels.size();
    for ( unsigned int i = 0; i < numChannels; ++i )
    {
      Mix_HaltChannel( _channels[i] );
    }
  }


  void AudioHandler::pause()
  {
    DEBUG_LOG( "Pausing audio handler" );
    switch( _state )
    {
      case MUSIC_STATE_STOPPED :
      case MUSIC_STATE_PAUSED :
        break;

      case MUSIC_STATE_PLAYING :
        _pauseAll();
        _state = MUSIC_STATE_PAUSED;
        break;

      default:
        break;
    }
  }


  void AudioHandler::play()
  {
    DEBUG_LOG( "Playing audio handler" );
    switch( _state )
    {
      case MUSIC_STATE_STOPPED :
        break;

      case MUSIC_STATE_PAUSED :
        _resumeAll();
        break;

      case MUSIC_STATE_PLAYING :
      default:
        break;
    }

    _state = MUSIC_STATE_PLAYING;
  }


  void AudioHandler::stop()
  {
    DEBUG_LOG( "Stopping audio handler" );

    switch( _state )
    {
      case MUSIC_STATE_PAUSED :
      case MUSIC_STATE_PLAYING :
        _stopAll();
        break;

      case MUSIC_STATE_STOPPED :
      default:
        break;
    }

    _state = MUSIC_STATE_STOPPED;
  }


  void AudioHandler::setSong( unsigned int n )
  {
    _manager->playTrack( n );
  }


  void AudioHandler::triggerEffect( unsigned int num )
  {
    _manager->playChunk( num );
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // "Asynchronous" play next function
  void playNextTrack()
  {
    DEBUG_LOG( "Playing next track" );
    if ( AudioManager::nextTrack != nullptr )
    {
      Mix_PlayMusic( AudioManager::nextTrack, -1 );
      AudioManager::nextTrack = nullptr;
    }
  }

}

