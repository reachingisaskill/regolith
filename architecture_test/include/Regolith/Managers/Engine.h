
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
      // Create the engine with the required references in place
      Engine( InputManager&, SDL_Color& );

      // Just in case I decided to inherit from here in the future...
      virtual ~Engine();

      // Set the renderer pointer - now ready to run
      void setRenderer( SDL_Renderer* rend ) { _renderer = rend; }

      // Start the engine running. In order to stop it the quit() function must be used.
      void run();

      // Set the quit flag and exit at the end of the current frame
      void quit() { _quit = true; }


      // Returns a pointer to the current context with focus
      Context* currentContext() { return _contextStack.front(); }

      // Pushes a stack operation to the queue to be excecuted after the window has been renderered.
      void stackOperation( StackOperation op ) { _stackOperationQueue.push( op ); }


      // Fulfill the interface for a component
      // Register game-wide events with the manager
      virtual void registerEvents( InputManager& );

      // Interfaces for input
      virtual void eventAction( const RegolithEvent&, const SDL_Event& );



      /*
       * Stack operation class.
       * This allows a queue of operations to be created in parallel to the update/render loop.
       * The operations are cached until they can be safely performed without accidentally invalidating the
       * stack iterators.
       */
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

