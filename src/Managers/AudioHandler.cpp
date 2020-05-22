
#include "Regolith/Managers/AudioHandler.h"
#include "Regolith/Managers/Manager.h"


namespace Regolith
{

  AudioHandler::AudioHandler() :
    _manager( Manager::getInstance()->getAudioManager() ),
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
    _manager.playTrack( n );
  }


  void AudioHandler::stopSong()
  {
    _manager.stopMusic();
  }


  void AudioHandler::triggerEffect( unsigned int num )
  {
    _manager.playChunk( num );
  }

}

