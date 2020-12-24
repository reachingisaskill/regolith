
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

    protected:
      // Configure the death timer.
      virtual void onStart() override;

    public:
      // Trivial Constructor
      EmptyContext();

      // Trivial Destructor
      ~EmptyContext();


      // Title Scenes take ownership of the display.
      virtual bool overridesPreviousContext() const override { return true; }


      // Updates the camera position
      virtual Vector updateCamera( float ) const override { return Vector(); }


      // Called for each object that is flagged to have global physics
      virtual void updatePhysics( PhysicalObject*, float ) const {}


      // Updates the internal state of the context. Nothing to do here.
      virtual void updateContext( float ) override;
  };

}

#endif // REGOLITH_TEST_EMPTY_CONTEXT_H_
