
#include "Regolith/Managers/AudioManager.h"
#include "Regolith/Utilities/Exception.h"
#include "Regolith/Utilities/JsonValidation.h"

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

  AudioManager::AudioManager() :
    _frequency( 0 ),
    _format( 0 ),
    _channels( 0 ),
    _chunkSize( 0 ),
    _fadeTime( 200 ),
    _playbackChannelCounter( 0 ),
    _musics( "music_files" ),
    _effects( "chunk_files" ),
    _volumeMusic( 1.0 ), // Defaults to full volume
    _volumeChunk( 1.0 ) // Defaults to full volume
  {
    _musics.addObject( nullptr, "null" ); // Set the zeroth element to be null
  }


  AudioManager::~AudioManager()
  {
  }


  void AudioManager::clear()
  {
    INFO_LOG( "Clearing the audio manager" );
    // Drop the hook in case something happens
    Mix_HookMusicFinished( nullptr );
    nextTrack = nullptr;

    // Free all the memory used by the music files
    for ( unsigned int i = 0; i < _musics.vectorSize(); ++i )
    {
      if ( _musics[i] != nullptr )
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


  void AudioManager::configure( Json::Value& json_data )
  {
    Utilities::validateJson( json_data, "sample_frequency", Utilities::JSON_TYPE_INTEGER );
//    Utilities::validateJson( json_data, "format", Utilities::JSON_TYPE_INTEGER );
    Utilities::validateJson( json_data, "audio_channels", Utilities::JSON_TYPE_INTEGER );
    Utilities::validateJson( json_data, "chunk_size", Utilities::JSON_TYPE_INTEGER );
    Utilities::validateJson( json_data, "music_files", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJson( json_data, "effect_files", Utilities::JSON_TYPE_ARRAY );
    Utilities::validateJson( json_data, "music_volume", Utilities::JSON_TYPE_FLOAT );
    Utilities::validateJson( json_data, "effect_volume", Utilities::JSON_TYPE_FLOAT );

    _frequency = json_data["sample_frequency"].asInt();
//    _format = json_data["format"].asInt();
    _format = MIX_DEFAULT_FORMAT;
    _channels = json_data["audio_channels"].asInt();
    _chunkSize = json_data["chunk_size"].asInt();

    if ( Utilities::validateJson( json_data, "chunk_size", Utilities::JSON_TYPE_INTEGER, false ) )
      _fadeTime = json_data["fade_time"].asInt();

    if ( Mix_OpenAudio( _frequency, _format, _channels, _chunkSize ) == -1 )
    {
      Exception ex( "AudioManager::AudioManager()", "Failed to initialise audio device" );
      ex.addDetail( "Mix Error", Mix_GetError() );
      ex.addDetail( "Frequency", _frequency );
      ex.addDetail( "Channels", _channels );
      ex.addDetail( "Chunk Size", _chunkSize );
      ex.addDetail( "Format", _format );
      throw ex;
    }

    INFO_STREAM << "Initialised Audio Device: " << _frequency << "Hz, " << _channels << " channels, " << _chunkSize << "byte chunks.";

    _volumeMusic = json_data["music_volume"].asFloat();
    _volumeChunk = json_data["effect_volume"].asFloat();


    Json::Value& music_files = json_data["music_files"];
    Json::Value& effect_files = json_data["effect_files"];


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


  void AudioManager::stopMusic()
  {
    if ( Mix_PlayingMusic() == 1 )
    {
      Mix_FadeOutMusic( _fadeTime );
    }
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

