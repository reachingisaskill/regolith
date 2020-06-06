
#ifndef REGOLITH_CONTEXTS_MENU_H_
#define REGOLITH_CONTEXTS_MENU_H_

#include "Regolith/Architecture/Context.h"

namespace Regolith
{

  class MenuContext : public Context
  {
    private:
      
    protected:
      // Hooks for context state changes
      virtual void onStart() override;
      virtual void onStop() override;
      virtual void onPause() override {}
      virtual void onResume() override {}

    public:
      // Trivial Constructor
      MenuContext();

      // Trivial Destructor
      virtual ~MenuContext();

      // Trivial configure - call the base-class variant
      virtual void configure( Json::Value&, ContextGroup& ) override;


      // Title Scenes take ownership of the display.
      virtual bool overridesPreviousContext() const override { return false; }


//////////////////////////////////////////////////
      // Requirements for the ControllableInterface - input action handling

      // Register context-wide actions with the handler
      virtual void registerActions( InputHandler& ) override;

      // Interfaces for input
      // Handled and mapped actions
      virtual void inputAction( const InputAction& ) override;
      virtual void booleanAction( const InputAction&, bool ) override;
      virtual void floatAction( const InputAction&, float ) override {}
      virtual void vectorAction( const InputAction&, const Vector& ) override {}
      virtual void mouseAction( const InputAction&, bool, const Vector& ) override {}

      
//////////////////////////////////////////////////
      // Requirements for a component - event handling

      // Register game-wide events with the manager
      virtual void registerEvents( InputManager& ) override;

      // Regolith events
      virtual void eventAction( const RegolithEvent&, const SDL_Event& ) override;
  };

}

#endif // REGOLITH_CONTEXTS_MENU_H_

