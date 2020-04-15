
#include "Timer.h"

namespace Regolith
{

  Stopwatch::Stopwatch():
    _startTime( 0 ),
    _pausedCount( 0 ),
    _currentState( TIMER_STOPPED )
  {
  }


  void Stopwatch::start()
  {
    Uint32 ticks = SDL_GetTicks();

    switch( _currentState )
    {
      case TIMER_STOPPED :
        _startTime = ticks;
        break;

      case TIMER_STARTED : // Do nothing
        break;

      case TIMER_PAUSED :
        _pausedCount += ticks - _pauseTime;
        break;
    }
    
    _currentState = TIMER_STARTED;
  }


  void Stopwatch::pause()
  {
    Uint32 ticks = SDL_GetTicks();
    switch( _currentState )
    {
      case TIMER_STOPPED : // Do nothing
        break;

      case TIMER_STARTED:
        _pauseTime = ticks;
        _currentState = TIMER_PAUSED;
        break;

      case TIMER_PAUSED : // Do Nothing
        break;
    }
  }


  Uint32 Stopwatch::stop()
  {
    Uint32 time = this->time();

    _currentState = TIMER_STOPPED;
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
      case TIMER_STOPPED :
        return 0;
        break;

      case TIMER_PAUSED :
        return ( ticks - _startTime ) - (_pausedCount + (ticks - _pauseTime));
        break;

      case TIMER_STARTED :
        return ( ticks - _startTime ) - _pausedCount;
        break;
    }

    return 0;
  }

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Minimal timer class


  Timer::Timer() :
    _startTime( SDL_GetTicks() )
  {
  }


  Uint32 Timer::lap()
  {
    Uint32 ticks = SDL_GetTicks();
    Uint32 time = ticks - _startTime;
    _startTime = ticks;
    return time;
  }

}

