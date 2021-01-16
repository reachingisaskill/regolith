
#ifndef REGOLITH_TEST_EMPTY_CONTEXT_H_
#define REGOLITH_TEST_EMPTY_CONTEXT_H_

#include "Regolith/Contexts/Context.h"
#include "Regolith/GamePlay/Timers.h"


namespace Regolith
{

  /*
   * Provides a context that does nothing for the integration test to use
   */
  class EmptyContext : public Context
  {
    private:
      CountdownTimer _death;
      bool _fading;
      float _time;
      float _fadeTrigger;
      float _fadeTime;
      SDL_Color _fadeColour;

    protected:
      // Configure the death timer.
      virtual void onStart() override;

      // Updates the camera position
      virtual Vector updateCamera( float ) const override { return Vector(); }

      // Called for each object that is flagged to have global physics
      virtual void updatePhysics( PhysicalObject*, float ) const {}

      // Updates the internal state of the context. Nothing to do here.
      virtual void updateContext( float ) override;

      // Called at the end of the render loop to do any context-specific rendering (e.g. transitions)
      virtual void renderContext( Camera& );

    public:
      // Trivial Constructor
      EmptyContext();

      // Trivial Destructor
      ~EmptyContext();


      // Title Scenes take ownership of the display.
      virtual bool overridesPreviousContext() const override { return true; }

  };

}

#endif // REGOLITH_TEST_EMPTY_CONTEXT_H_
