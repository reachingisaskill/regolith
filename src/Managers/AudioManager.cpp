
#include "Regolith/Managers/AudioManager.h"
#include "Regolith/Audio/Music.h"
#include "Regolith/Audio/Playlist.h"
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
  AudioManager::MusicOperation AudioManager::_operation = AudioManager::MusicOperation::PLAY;
  std::mutex AudioManager::_operationMutex;


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
  // Hidden music interface

  void AudioManager::queueTrack( Music* music )
  {
    DEBUG_LOG( "AudioManager::queueTrack : Here" );
    _musicQueue.push( std::make_pair( music, music->getPlayCount() ) );
  }


  void AudioManager::playTrack( Music* music )
  {
    DEBUG_LOG( "AudioManager::playTrack : Here" );
    // Clear the queue, this takes precedent
    _musicQueue.clear();

    // Push this music to the queue
    _musicQueue.push( std::make_pair( music, music->getPlayCount() ) );

    // If music is playing
    if ( Mix_PlayingMusic() == 1 )
    {
      // Tell it to stop
      UniqueLock lock( _operationMutex );
      _operation = MusicOperation::NEXT;
      lock.unlock();
      Mix_HaltMusic();
    }
    else // Not playing
    {
      // Trigger the next track algorithm manually
      UniqueLock lock( _operationMutex );
      _operation = MusicOperation::NEXT;
      lock.unlock();
      playNextTrack();
    }
  }


  void AudioManager::clearQueue()
  {
    DEBUG_LOG( "AudioManager::clearQueue : Here" );
    _musicQueue.clear();
  }


  void AudioManager::stopTrack()
  {
    DEBUG_LOG( "AudioManager::stopTrack : Here" );

    UniqueLock lock( _operationMutex );
    _operation = MusicOperation::STOP;
    lock.unlock();

    if ( Mix_PlayingMusic() == 1 )
    {
      Mix_HaltMusic();
    }
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Public music interface

  void AudioManager::load( Playlist* pl )
  {
    DEBUG_LOG( "AudioManager::play : Playing playlist" );

    // Empty existing queue
    _musicQueue.clear();

    // If there wasn't a playlist, nothing to push.
    // This is kind of a backdoor to clearing the music queue
    if ( pl == nullptr ) return;

    // Refill it from the playlist
    for ( Playlist::MusicList::iterator it = pl->_musics.begin(); it != pl->_musics.end(); ++it )
    {
      _musicQueue.push( std::make_pair( &(*it), it->getPlayCount() ) );
    }
  }


  void AudioManager::pauseTrack()
  {
    DEBUG_LOG( "AudioManager::pauseTrack : Here" );
    if ( Mix_PlayingMusic() == 1 )
    {
      Mix_PauseMusic();
    }
  }


  void AudioManager::resumeTrack()
  {
    DEBUG_LOG( "AudioManager::resumeTrack : Here" );

    if ( Mix_PlayingMusic() && Mix_PausedMusic() )
    {
      Mix_ResumeMusic();
    }
    else
    {
      UniqueLock lock( _operationMutex );
      _operation = MusicOperation::NEXT;
      lock.unlock();

      playNextTrack();
    }
  }


  void AudioManager::stopRepeatTrack()
  {
    DEBUG_LOG( "AudioManager::stopRepeatTrack : Here" );
    if ( Mix_PlayingMusic() == 1 )
    {
      UniqueLock lock( _operationMutex );
      _operation = MusicOperation::STOP;
      lock.unlock();
    }
  }


  void AudioManager::nextTrack()
  {
    DEBUG_LOG( "AudioManager::nextTrack : Here" );
    UniqueLock lock( _operationMutex );
    _operation = MusicOperation::NEXT;
    lock.unlock();

    if ( Mix_PlayingMusic() == 1 )
    {
      Mix_HaltMusic();
    }
    else
    {
      playNextTrack();
    }
  }


  void AudioManager::nextRepeatTrack()
  {
    DEBUG_LOG( "AudioManager::nextRepeatTrack : Here" );
    UniqueLock lock( _operationMutex );
    _operation = MusicOperation::NEXT;
    lock.unlock();
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  //Configuration

  void AudioManager::configure( Json::Value& json_data )
  {
    validateJson( json_data, "sample_frequency", JsonType::INTEGER );
//    validateJson( json_data, "format", JsonType::INTEGER );
    validateJson( json_data, "audio_channels", JsonType::INTEGER );
    validateJson( json_data, "chunk_size", JsonType::INTEGER );
    validateJson( json_data, "music_volume", JsonType::FLOAT );
    validateJson( json_data, "effect_volume", JsonType::FLOAT );

    _frequency = json_data["sample_frequency"].asInt();
//    _format = json_data["format"].asInt();
    _format = MIX_DEFAULT_FORMAT;
    _channels = json_data["audio_channels"].asInt();
    _chunkSize = json_data["chunk_size"].asInt();

    if ( validateJson( json_data, "fade_time", JsonType::INTEGER, false ) )
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
    static AudioManager::QueueElement& current = AudioManager::_currentTrack;
    static std::mutex& operation_mutex = AudioManager::_operationMutex;
    static AudioManager::MusicOperation& operation = AudioManager::_operation;

    // Copy the next operation
    UniqueLock lock( operation_mutex );

    switch ( operation )
    {
      // Keep playing the current track
      case AudioManager::MusicOperation::PLAY :
        DEBUG_LOG( "playNextTrack : Operation PLAY" );
        break;

      //Try to play the next track in the queue
      case AudioManager::MusicOperation::NEXT :
        DEBUG_LOG( "playNextTrack : Operation NEXT" );
        // If queue has elements
        if ( ! queue.empty() )
        {
          // Update current music
          queue.pop( current );
        }
        else // Nothing to do
        {
          current.first = nullptr;
          current.second = 0;
        }
        break;

      // Stop the current playback
      case AudioManager::MusicOperation::STOP :
        DEBUG_LOG( "playNextTrack : Operation STOP" );
        current.first = nullptr;
        current.second = 0;
        break;
    }

    // The default state
    operation = AudioManager::MusicOperation::PLAY;
    lock.unlock();


    // If the current track is valid
    if ( current.first != nullptr )
    {
      DEBUG_LOG( "playNextTrack : Current Valid" );
      // If repeats to come
      if ( current.second > 1 )
      {
        DEBUG_LOG( "playNextTrack : Counting down" );
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
        DEBUG_LOG( "playNextTrack : Repeating" );
        Mix_PlayMusic( current.first->getMIXMusic(), 1 );
      }
    }

  }


}

