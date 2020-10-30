
#ifndef REGOLITH_GAMEPLAY_TIMER_H_
#define REGOLITH_GAMEPLAY_TIMER_H_

#include "Regolith/Global/Global.h"


namespace Regolith
{

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Stopwatch class

  class Stopwatch
  {
    public :
      enum StopwatchState
      {
        STOPPED,
        STARTED,
        PAUSED,
      };

    private:
      Uint32 _startTime;
      Uint32 _pauseTime;
      Uint32 _pausedCount;
      StopwatchState _currentState;

    public:
      Stopwatch();

      void start();

      void pause();

      Uint32 stop();

      Uint32 restart();

      Uint32 time() const;

      StopwatchState state() const { return _currentState; }
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Minimal timer class - Mostly for timing frames

  class Timer
  {
    private:
      Uint32 _startTime;

    public:
      Timer();

      Uint32 lap();
  };

}

#endif // REGOLITH_GAMEPLAY_TIMER_H_

