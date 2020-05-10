
#ifndef REGOLITH_MANAGERS_ENGINE_H_
#define REGOLITH_MANAGERS_ENGINE_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/Component.h"
#include "Regolith/Architecture/Context.h"
#include "Regolith/Managers/InputManager.h"
#include "Regolith/Components/Window.h"
#include "Regolith/Components/Timer.h"


namespace Regolith
{

  typedef std::deque< Context* > ContextStack;


  class Engine : public Component
  {
    class StackOperation;

    typedef std::queue< StackOperation > StackOperationQueue;

    private:
      SDL_Renderer* _theRenderer;
      Window& _window;
      InputManager& _inputManager;
      SDL_Color& _defaultColor;
      ContextStack _contextStack;

      StackOperationQueue _stackOperationQueue;

      ContextStack::reverse_iterator _visibleStackStart;
      ContextStack::reverse_iterator _visibleStackEnd;
      Timer _frameTimer;
      bool _quit;
      bool _pause;

    protected:
      void performStackOperations()

    public:
      Engine( SDL_Renderer*, Window&, InputManager&, SDL_Color& );

      virtual ~Engine();

      void setRenderer( SDL_Renderer* rend ) { _renderer = rend; }

      void run();

      void quit() { _quit = true; }

      Context* currentContext() { return _contextStack.front(); }

      void stackOperation( StackOperation op ) { _stackOperationQueue.push( op ); }


      // Fulfill the interface for a component
      // Register game-wide events with the manager
      virtual void registerEvents( InputManager* );

      // Interfaces for input
      virtual void eventAction( const RegolithEvent&, const SDL_Event& );



      class StackOperation
      {
        enum Operation : char
        {
          POP,
          PUSH,
          RESET,
          TRANSFER
        };

        Context* context;
        Operation operation;

        StackOperation( Operation op, Context* c = nullptr ) : context( c ), operation( op ) {}
      };
  };
  
}

#endif // REGOLITH_MANAGERS_ENGINE_H_

