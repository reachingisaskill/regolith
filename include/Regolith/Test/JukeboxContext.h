
#ifndef REGOLITH_TEST_JUKEBOX_CONTEXT_H_
#define REGOLITH_TEST_JUKEBOX_CONTEXT_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Contexts/Context.h"
#include "Regolith/GamePlay/Timers.h"
#include "Regolith/Audio/Playlist.h"
#include "Regolith/Test/StatusString.h"


namespace Regolith
{

  class JukeboxContext : public Context
  {
    private:
      Playlist* _playlist;

      StatusString* _status;

      CountdownTimer _skipTimer1;
      CountdownTimer _skipTimer2;
      CountdownTimer _pauseTimer;
      CountdownTimer _resumeTimer;
      CountdownTimer _stopTimer;
      CountdownTimer _startTimer;
      CountdownTimer _stopRepeatTimer;
      CountdownTimer _closeTimer;

    protected:
      // Hooks for context state changes
      virtual void onStart() override;
//      virtual void onStop() override { this->setClosed( true ); }

      // Called during the update loop for frame-dependent context actions
      virtual void updateContext( float ) override;

      // Called after the physics processes to update the current position that the camera renders from
      virtual Vector updateCamera( float ) const override { return zeroVector; }

      // Called for each object that is flagged to have global physics
      virtual void updatePhysics( PhysicalObject*, float ) const override {}


    public:
      JukeboxContext();

      virtual ~JukeboxContext();


      // Configure the context
      void configure( Json::Value&, ContextGroup& ) override;


      // Sits at the bottom of the stack and plays music
      virtual bool overridesPreviousContext() const override { return true; }

  };

}

#endif // REGOLITH_TEST_JUKEBOX_CONTEXT_H_

