
#ifndef REGOLITH_CONTEXTS_NULL_H_
#define REGOLITH_CONTEXTS_NULL_H_

#include "Regolith/Architecture/Context.h"
#include "Regolith/Architecture/Interactable.h"
#include "Regolith/GameObjects/MusicTrack.h"


namespace Regolith
{

  /*
   * Provides a context that does nothing for the integration test to use
   */
  class NullContext : public Context
  {
    private:
      DataHandler* _testHandler;

    protected:
      virtual void onStart() override;
      virtual void onStop() override {}
      virtual void onPause() override {}
      virtual void onResume() override {}

      virtual void updateContext( float ) override {}

    public:
      // Trivial Constructor
      NullContext();

      // Trivial Destructor
      ~NullContext();


      // Trivial configure - call the base-class variant
      virtual void configure( Json::Value&, ContextGroup& ) override;

      virtual void validate() const override {}


      // Title Scenes take ownership of the display.
      virtual bool overridesPreviousContext() const override { return true; }


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
      virtual void registerEvents( InputManager& ) override {}

      // Regolith events
      virtual void eventAction( const RegolithEvent&, const SDL_Event& ) override {}

  };

}

#endif // REGOLITH_CONTEXTS_NULL_H_

