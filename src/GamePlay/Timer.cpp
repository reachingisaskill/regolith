
#include "Regolith/GamePlay/Timer.h"

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

