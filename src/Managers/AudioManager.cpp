
#include "Regolith/Managers/AudioManager.h"
#include "Regolith/Utilities/Exception.h"
#include "Regolith/Utilities/JsonValidation.h"

#include <cmath>


namespace Regolith
{

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Forward declaration
  void playNextTrack();

  // Static member variables
  AudioManager::MusicQueue AudioManager::_musicQueue;
  Mix_Music* AudioManager::_currentTrack = nullptr;


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Audio Manager member functions

  AudioManager::AudioManager() :
    _frequency( 0 ),
    _format( 0 ),
    _channels( 0 ),
    _chunkSize( 0 ),
    _fadeTime( 200 ),
    _playbackChannelCounter( 0 ),
    _volumeMusic( 1.0 ), // Defaults to full volume
    _volumeChunk( 1.0 ) // Defaults to full volume
  {
  }


  AudioManager::~AudioManager()
  {
  }


  void AudioManager::clear()
  {
    INFO_LOG( "AudioManager::clear : Clearing the audio manager" );

    Mix_HookMusicFinished( nullptr );
    _musicQueue.clear();

    Mix_CloseAudio();
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Volume functions

  void AudioManager::setVolumeMusic( float v )
  {
    // Make sure it is within range
    if ( v > 1.0 || v < 0.0 )
    {
      WARN_STREAM << "AudioManager::setVolumeMusic : Music Volume: " << v << " is out of range [0,1]. It will be truncated.";
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
      WARN_STREAM << "AudioManager::setVolumeEffects : Effects Volume: " << v << " is out of range [0,1]. It will be truncated.";
      v = std::fabs( v );
      v = ( (v >1.0) ? 1.0 : v );
    }

    // Set the mixer volume
    Mix_Volume( -1, (int) (v * MIX_MAX_VOLUME) );

    // Set the member variable
    _volumeChunk = v;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Music interface

  void AudioManager::queueTrack( Mix_Music* music, unsigned int N )
  {
    if ( Mix_PlayingMusic() == 1 )
    {
      _musicQueue.push( std::make_pair( music, N ) );
    }
    else
    {
      Mix_PlayMusic( music, (signed int)N );
    }

    Mix_HookMusicFinished( playNextTrack );
  }


  void AudioManager::playTrack( Mix_Music* music, unsigned int N )
  {
    // Stop if playing
    if ( Mix_PlayingMusic() == 1 )
    {
      Mix_HaltMusic();
    }

    // Jump the queue and play this
    Mix_PlayMusic( music, (signed int)N );
  }


  void AudioManager::clearQueue()
  {
    _musicQueue.clear();
  }


  void AudioManager::stopTrack()
  {
    Mix_FadeOutMusic( _fadeTime );
    Mix_HookMusicFinished( nullptr );
  }


  void AudioManager::stopNextTrack()
  {
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  //Configuration

  void AudioManager::configure( Json::Value& json_data )
  {
    Utilities::validateJson( json_data, "sample_frequency", Utilities::JSON_TYPE_INTEGER );
//    Utilities::validateJson( json_data, "format", Utilities::JSON_TYPE_INTEGER );
    Utilities::validateJson( json_data, "audio_channels", Utilities::JSON_TYPE_INTEGER );
    Utilities::validateJson( json_data, "chunk_size", Utilities::JSON_TYPE_INTEGER );
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

    INFO_STREAM << "AudioManager::configure : Initialised Audio Device: " << _frequency << "Hz, " << _channels << " channels, " << _chunkSize << "byte chunks.";

    _volumeMusic = json_data["music_volume"].asFloat();
    _volumeChunk = json_data["effect_volume"].asFloat();
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // "Asynchronous" play next function

  void playNextTrack()
  {
    // Static reference to the music queue
    static AudioManager::MusicQueue& queue = AudioManager::_musicQueue;
    static Mix_Music*& current = AudioManager::_currentTrack;
    AudioManager::QueueElement element;

    if ( queue.empty() )
    {
      Mix_PlayMusic( current, 0 );
    }
    else
    {
      queue.pop( element );
      current = element.first;
      Mix_PlayMusic( current, (signed int)element.second );
    }

  }


//    Condition<Mix_Music*>& musicUpdate = Manager::getInstance()->getThreadManager().MusicUpdate;
//    GuardLock lk( musicUpdate.mutex );
//
//    DEBUG_LOG( "playNextTrack : Playing next track" );
//    if ( musicUpdate.data != nullptr )
//    {
//      Mix_PlayMusic( musicUpdate.data, -1 );
//      musicUpdate.data = nullptr;
//    }
//  }


}

