
#ifndef REGOLITH_GAMEPLAY_TIMERS_H_
#define REGOLITH_GAMEPLAY_TIMERS_H_

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

  class FrameTimer
  {
    private:
      Uint32 _startTime;

    public:
      FrameTimer();

      Uint32 lap();
  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Countdown timer for use within contexts

  class CountdownTimer
  {
    private:
      // Number of times to repeat
      unsigned int _repeatLimit;
      // Number of times repeated
      unsigned int _repeatCount;
      // Target countdown time
      float _target;
      // Integrated time elapsed
      float _time;

    public:
      // Set the countdown time and number of times to run it
      CountdownTimer();

      // Set the whole configuration
      void configure( float, unsigned int );


      // Set the time limit
      void setTime( float t ) { _target = t; }

      // Set the number of repeats
      void setRepeats( unsigned int R ) { _repeatLimit = R; }


      // Return true if countdown time has elapsed
      bool trigger( float );

      // Resets the clock and number of reps
      void reset();


      // Return the time remaining
      float remaining() const { return _time; }
  };

}

#endif // REGOLITH_GAMEPLAY_TIMERS_H_

