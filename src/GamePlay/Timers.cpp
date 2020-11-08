
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

  FrameTimer::FrameTimer() :
    _startTime( SDL_GetTicks() )
  {
  }


  Uint32 FrameTimer::lap()
  {
    Uint32 ticks = SDL_GetTicks();
    Uint32 time = ticks - _startTime;
    _startTime = ticks;
    return time;
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

