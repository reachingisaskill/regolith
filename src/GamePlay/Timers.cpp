
#include "Regolith/GamePlay/Timers.h"

namespace Regolith
{

  Stopwatch::Stopwatch():
    _startTime( 0 ),
    _pausedCount( 0 ),
    _currentState( Stopwatch::STOPPED )
  {
  }


  void Stopwatch::start()
  {
    Uint32 ticks = SDL_GetTicks();

    switch( _currentState )
    {
      case Stopwatch::STOPPED :
        _startTime = ticks;
        break;

      case Stopwatch::STARTED : // Do nothing
        break;

      case Stopwatch::PAUSED :
        _pausedCount += ticks - _pauseTime;
        break;
    }
    
    _currentState = Stopwatch::STARTED;
  }


  void Stopwatch::pause()
  {
    Uint32 ticks = SDL_GetTicks();
    switch( _currentState )
    {
      case Stopwatch::STOPPED : // Do nothing
        break;

      case Stopwatch::STARTED:
        _pauseTime = ticks;
        _currentState = Stopwatch::PAUSED;
        break;

      case Stopwatch::PAUSED : // Do Nothing
        break;
    }
  }


  Uint32 Stopwatch::stop()
  {
    Uint32 time = this->time();

    _currentState = Stopwatch::STOPPED;
    return time;
  }


  Uint32 Stopwatch::restart()
  {
    Uint32 time = this->stop();
    this->start();
    return time;
  }


  Uint32 Stopwatch::time() const
  {
    Uint32 ticks = SDL_GetTicks();

    switch( _currentState )
    {
      case Stopwatch::STOPPED :
        return 0;
        break;

      case Stopwatch::PAUSED :
        return ( ticks - _startTime ) - (_pausedCount + (ticks - _pauseTime));
        break;

      case Stopwatch::STARTED :
        return ( ticks - _startTime ) - _pausedCount;
        break;
    }

    return 0;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Minimal timer class

  FrameTimer::FrameTimer( unsigned int n ) :
    _startTime( SDL_GetTicks() ),
    _frameCount( n ),
    _frameSum( 0.0 ),
    _fpsSum( 0.0 ),
    _avgCount( 0.0 ),
    _avgfps( 0.0 ),
    _maxfps( 0.0 ),
    _minfps( 1.0E36 )
  {
  }


  float FrameTimer::lap()
  {
    Uint32 ticks = SDL_GetTicks();
    float time = ticks - _startTime;
    _startTime = ticks;

    _fpsSum += 1000.0 / time;
    _frameSum += 1.0;
    if ( _frameSum >= _frameCount )
    {
      float fps = _fpsSum/_frameSum;

      if ( fps < 1.0E6 )
      {
        _avgfps = ( _avgfps * _avgCount + fps ) / ( _avgCount + 1.0 );
        _avgCount += 1.0;
        if ( fps > _maxfps ) _maxfps = fps;
        if ( fps < _minfps ) _minfps = fps;
      }

      _frameSum = 0.0;
      _fpsSum = 0.0;
    }

    return (float) time;
  }


  void FrameTimer::resetFPSCount()
  {
    _frameSum = 0.0;
    _fpsSum = 0.0;
    _avgCount = 0.0;
    _avgfps = 0.0;
    _minfps = 1.0E36;
    _maxfps = 0.0;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Countdown timer

  CountdownTimer::CountdownTimer() :
    _repeatLimit( 0 ),
    _repeatCount( 0 ),
    _target( 0.0 ),
    _time( 0.0 )
  {
  }


  void CountdownTimer::configure( float time, unsigned int R )
  {
    _target = time;
    _time = _target;
    _repeatLimit = R;
    _repeatCount = 0;
  }


  bool CountdownTimer::trigger( float delta_t )
  {
    if ( _repeatCount >= _repeatLimit ) return false;

    _time -= delta_t;

    if ( _time < 0.0 )
    {
      _repeatCount += 1;
      _time += _target; // Precise timing!
      return true;
    }

    return false;
  }


  void CountdownTimer::reset()
  {
    _time = _target;
    _repeatCount = 0;
  }


}

