
#ifndef REGOLITH_TEST_LOAD_SCREEN_H_
#define REGOLITH_TEST_LOAD_SCREEN_H_

#include "Regolith/Contexts/Context.h"


namespace Regolith
{

  /*
   * Provides a context that does nothing for the integration test to use
   */
  class LoadScreen : public Context
  {
    private:

    protected:

      // Trigger the loading process
      virtual void onStart() override;

    public:
      // Trivial Constructor
      LoadScreen();

      // Trivial Destructor
      ~LoadScreen();


      // Title Scenes take ownership of the display.
      virtual bool overridesPreviousContext() const override { return true; }


      // Updates the camera position
      virtual Vector updateCamera( float ) const override { return zeroVector; }


      // Called for each object that is flagged to have global physics
      virtual void updatePhysics( PhysicalObject*, float ) const {}


      // Updates the internal state of the context. Nothing to do here.
      virtual void updateContext( float ) override;
  };

}

#endif // REGOLITH_TEST_LOAD_SCREEN_H_
