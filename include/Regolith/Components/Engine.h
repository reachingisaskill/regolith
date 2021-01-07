
#ifndef REGOLITH_COMPONENTS_ENGINE_H_
#define REGOLITH_COMPONENTS_ENGINE_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/Component.h"
#include "Regolith/Managers/InputManager.h"
#include "Regolith/GamePlay/Timers.h"

#include <mutex>


namespace Regolith
{

  class Context;
  class ContextGroup;
  class DataHandler;

  typedef std::deque< Context* > ContextStack;


  class Engine : public Component
  {
////////////////////////////////////////////////////////////////////////////////
    // Friend declarations
    template < class T, class R > friend class Link;

////////////////////////////////////////////////////////////////////////////////
    // Sub classes and types
    public:
      class StackOperation;

    private:
      typedef std::queue< StackOperation > StackOperationQueue;


    private:
      // Stack of all contexts that are curent rendereable
      ContextStack _contextStack;

      // Queue up pointers to the next context or context group to load. Context groups take priority
      Context* _openContext;
      Context* _openContextStack;
      ContextGroup* _openContextGroup;
      ContextGroup* _currentContextGroup;

      // Only draw the context stack between these two iterators
      ContextStack::reverse_iterator _visibleStackStart;
      ContextStack::reverse_iterator _visibleStackEnd;

      // Count the frame times. Provide update time for loops and estimate FPS.
      FrameTimer _frameTimer;

      // Store the pause state
      bool _pause;


      // Mutex to control access to contexts
      mutable std::mutex _renderMutex;


    protected:
      // Function which checks the current context stack and performs the queued operations
      bool performStackOperations();


    public:
      // Create the engine with the required references in place
      Engine();

      // Just in case I decided to inherit from here in the future...
      virtual ~Engine();

      // Start the engine running. In order to stop it the quit() function must be used.
      void run();


////////////////////////////////////////////////////////////////////////////////
      // Context stack manipulation

      // Tells the engine to push the context pointer to the top of the stack. Must be a member of the current context group
      void openContext( Context* );

      // Tells the engine to push the context pointer to the top of the stack, once it has cleared.
      // This context must be a member of the current context group.
      void openContextStack( Context* );

      // Tells the engine that this is the new context group entry point. Current stack MUST close itself!
      void openContextGroup( ContextGroup* );


      // Returns a pointer to the current context with focus
      Context* currentContext() { return _contextStack.front(); }

      // Returns a pointer to the current context group that owns the contents of the stack
      ContextGroup* currentContextGroup() { return _currentContextGroup; }



      // Return the current estimated FPS of the engine
      float getFPS() const { return _frameTimer.getAvgFPS(); }


      // Fulfill the interface for a component
      // Register game-wide events with the manager
      virtual void registerEvents( InputManager& ) override;

      // Interfaces for input
      virtual void eventAction( const RegolithEvent&, const SDL_Event& );

  };
  



//      /*
//       * Stack operation class.
//       * This allows a queue of operations to be created in parallel to the update/render loop.
//       * The operations are cached until they can be safely performed without accidentally invalidating the
//       * stack iterators.
//       */
//      struct StackOperation
//      {
//        enum Operation : char
//        {
//          POP,
//          PUSH,
//          RESET,
//          TRANSFER
//        };
//
//        Context* context;
//        Operation operation;
//
//        StackOperation( Operation op, Context* c = nullptr ) : context( c ), operation( op ) {}
//      };

}

#endif // REGOLITH_COMPONENTS_ENGINE_H_

