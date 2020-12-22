
#include "Regolith/Managers/AudioManager.h"
#include "Regolith/Audio/Music.h"
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
  AudioManager::QueueElement AudioManager::_currentTrack = std::make_pair( nullptr, 0 );


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Audio Manager member functions

  AudioManager::AudioManager() :
    _frequency( 0 ),
    _format( 0 ),
    _channels( 0 ),
    _chunkSize( 0 ),
    _fadeTime( 0 ),
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

    if ( Mix_PlayingMusic() == 1 )
    {
      Mix_HaltMusic();
    }
    _musicQueue.clear();

    Mix_HookMusicFinished( nullptr );

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

  void AudioManager::queueTrack( Music* music, unsigned int N )
  {
    _musicQueue.push( std::make_pair( music, N ) );

    // If nothing's playing
    if ( Mix_PlayingMusic() == 0 )
    {
      // Trigger the next track algorithm manually
      playNextTrack();
    }
  }


  void AudioManager::playTrack( Music* music, unsigned int N )
  {
    // Clear the queue, this takes precedent
    _musicQueue.clear();

    // Push this music to the queue
    _musicQueue.push( std::make_pair( music, N ) );

    // If music is playing
    if ( Mix_PlayingMusic() == 1 )
    {
      // Tell it to stop
      Mix_FadeOutMusic( _fadeTime );
    }
    else // Not playing
    {
      // Trigger the next track algorithm manually
      playNextTrack();
    }
  }


  void AudioManager::clearQueue()
  {
    _musicQueue.clear();
  }


  void AudioManager::stopTrack()
  {
    _musicQueue.clear();

    if ( Mix_PlayingMusic() == 1 )
    {
//      Mix_FadeOutMusic( _fadeTime );
      Mix_HaltMusic();
    }
  }


  void AudioManager::nextTrack()
  {
    if ( Mix_PlayingMusic() == 1 )
    {
      Mix_HaltMusic();
    }
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

    if ( Utilities::validateJson( json_data, "fade_time", Utilities::JSON_TYPE_INTEGER, false ) )
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

    this->setVolumeMusic( json_data["music_volume"].asFloat() );
    this->setVolumeEffects( json_data["effect_volume"].asFloat() );

    // Set this once and leave it
    Mix_HookMusicFinished( playNextTrack );
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // "Asynchronous" play next function

  void playNextTrack()
  {
    // Static reference to the music queue
    static AudioManager::MusicQueue& queue = AudioManager::_musicQueue;
    static AudioManager::QueueElement current = AudioManager::_currentTrack;

    // Nothing currently playing
    if ( current.first == nullptr )
    {
      // If queue has elements
      if ( ! queue.empty() )
      {
        // Update current music
        queue.pop( current );
      }
    }

    // If there is something to do
    if ( current.first != nullptr )
    {
      // If repeats to come
      if ( current.second > 1 )
      {
        --current.second;
        Mix_PlayMusic( current.first->getMIXMusic(), 1 );

        // If this is the last play through
        if ( current.second == 1 )
        {
          if ( ! queue.empty() ) // Elements in queue
          {
            // Update current track 
            queue.pop( current );
          }
          else
          {
            // Nothing to play
            current.first = nullptr;
            current.second = 0;
          }
        }
      }
      else // Playing on repeat
      {
        Mix_PlayMusic( current.first->getMIXMusic(), 1 );
      }
    }


    Mix_HookMusicFinished( playNextTrack );
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

