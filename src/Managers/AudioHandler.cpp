//#define LOGTASTIC_DEBUG_OFF

#include "Regolith/Managers/AudioHandler.h"
#include "Regolith/Managers/ThreadManager.h"


namespace Regolith
{

  // This function takes the next track stored in the audio manager and plays it when the previous one has finished
  void playNextTrack();

////////////////////////////////////////////////////////////////////////////////////////////////////

  AudioHandler::AudioHandler( AudioManager& manager ) :
    _manager( manager ),
    _state( MUSIC_STATE_STOPPED ),
    _channelPauses()
  {
  }


  AudioHandler::~AudioHandler()
  {
  }


  void AudioHandler::configure()
  {
    int size = _channelPauses.size();
    int result = Mix_AllocateChannels( size );

    if ( result != size )
    {
      std::string error = Mix_GetError();
      Exception ex( "AudioHandler::configure()", "Could not allocate requested channels" );
      ex.addDetail( "Requested", size );
      ex.addDetail( "Received", result );
      ex.addDetail( "Mix Error", error );
      throw ex;
    }
  }


  void AudioHandler::requestChannel( RawSound* sound )
  {
    _channelPauses.push_back( 0 );
    sound->channel = _channelPauses.size()-1;
  }


  void AudioHandler::_pauseAll()
  {
    unsigned int numChannels = _channelPauses.size();
    for ( unsigned int i = 0; i < numChannels; ++i )
    {
      _channelPauses[i] += 1;
      Mix_Pause( i );
    }
  }


  void AudioHandler::_resumeAll()
  {
    unsigned int numChannels = _channelPauses.size();
    for ( unsigned int i = 0; i < numChannels; ++i )
    {
      int& number = _channelPauses[i];
      if ( number == 0 ) continue;
      else
      {
        number -= 1;
        if ( number == 0 )
        {
          Mix_Resume( i );
        }
      }
    }
  }


  void AudioHandler::_stopAll()
  {
    unsigned int numChannels = _channelPauses.size();
    for ( unsigned int i = 0; i < numChannels; ++i )
    {
      Mix_HaltChannel( i );
    }
  }


  void AudioHandler::pause()
  {
    DEBUG_LOG( "AudioHandler::pause : Pausing audio handler" );
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
    DEBUG_LOG( "AudioHandler::play : Playing audio handler" );
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
    DEBUG_LOG( "AudioHandler::stop : Stopping audio handler" );

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


  void AudioHandler::playSong( RawMusic* music, unsigned int N )
  {
    _manager.playTrack( music->music, N );
  }


  void AudioHandler::queueSong( RawMusic* music, unsigned int N )
  {
    _manager.queueTrack( music->music, N );
  }


  void AudioHandler::stopSong()
  {
    _manager.stopTrack();
  }


  void AudioHandler::playSound( RawSound* sound )
  {
    if ( Mix_Playing( sound->channel ) ) // Sound already playing
    {
      return;
    }

    // Play the chunk exactly once on the first free channel
    Mix_PlayChannel( sound->channel, sound->sound, 0 );
  }

}

