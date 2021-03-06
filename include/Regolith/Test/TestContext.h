
#ifndef REGOLITH_TEST_TEST_CONTEXT_H_
#define REGOLITH_TEST_TEST_CONTEXT_H_

#include "Regolith/Contexts/Context.h"
#include "Regolith/GamePlay/Timers.h"
#include "Regolith/GamePlay/Signal.h"


namespace Regolith
{

  /*
   * Provides a context that does nothing for the integration test to use
   */
  class TestContext : public Context
  {
    private:
      std::string _name;
      OpenContextGroupSignal _cgSignal;
//      ContextGroup* _cgLoad;
      OpenContextSignal _childSignal;
//      Context** _childLoad;
      CountdownTimer _timer;
      CountdownTimer _childTimer;
      CountdownTimer _deathTimer;

    protected:
      virtual void onStart() override;
      virtual void onStop() override;
      virtual void onPause() override {}
      virtual void onResume() override {}

      // Updates the camera position
      virtual Vector updateCamera( float ) const override { return Vector(); }

      // Called for each object that is flagged to have global physics
      virtual void updatePhysics( PhysicalObject*, float ) const {}

      // Update the state of the context
      virtual void updateContext( float ) override;

      // Called at the end of the render loop to do any context-specific rendering (e.g. transitions)
      virtual void renderContext( Camera& ) override {}

    public:
      // Trivial Constructor
      TestContext();

      // Trivial Destructor
      ~TestContext();


      // Trivial configure - call the base-class variant
      virtual void configure( Json::Value&, ContextGroup& ) override;


      // Title Scenes take ownership of the display.
      virtual bool overridesPreviousContext() const override { return false; }

//////////////////////////////////////////////////
      // Requirements for the ControllableInterface - input action handling

      // Register context-wide actions with the handler
      virtual void registerActions( InputHandler& ) override {}

      // Interfaces for input
      // Handled and mapped actions
      virtual void inputAction( const InputAction& ) override {}
      virtual void booleanAction( const InputAction&, bool ) override {}
      virtual void floatAction( const InputAction&, float ) override {}
      virtual void vectorAction( const InputAction&, const Vector& ) override {}
      virtual void mouseAction( const InputAction&, bool, const Vector& ) override {}

      
//////////////////////////////////////////////////
      // Requirements for a component - event handling

      // Register game-wide events with the manager
//      virtual void registerEvents( InputManager& ) override;

      // Regolith events
//      virtual void eventAction( const RegolithEvent&, const SDL_Event& ) override;

  };

}

#endif // REGOLITH_TEST_TEST_CONTEXT_H_

