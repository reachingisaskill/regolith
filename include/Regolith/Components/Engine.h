
#ifndef REGOLITH_COMPONENTS_ENGINE_H_
#define REGOLITH_COMPONENTS_ENGINE_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/Component.h"
#include "Regolith/Managers/InputManager.h"
#include "Regolith/GamePlay/Timer.h"

#include <mutex>
#include <condition_variable>


namespace Regolith
{

  class Context;
  class DataHandler;

  typedef std::deque< Context* > ContextStack;


  class Engine : public Component
  {
    friend void engineProcessingThread( Engine& );

    public:
      class StackOperation;

    private:
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
      bool _pause;

      std::mutex _textureRenderMutex;
      unsigned _queueRenderRate;
      /* Some queue of surfaces to render into textures */

    protected:
      void performStackOperations();

    public:
      // Create the engine with the required references in place
      Engine( InputManager&, SDL_Color& );

      // Just in case I decided to inherit from here in the future...
      virtual ~Engine();

      // Set the renderer pointer - now ready to run
      void setRenderer( SDL_Renderer* rend ) { _theRenderer = rend; }

      // Mutex-controlled rendering function. Turns surfaces into textures for a data handler.
      void renderTextures( DataHandler* ) const;

      // Start the engine running. In order to stop it the quit() function must be used.
      void run();


      // Returns a pointer to the current context with focus
      Context* currentContext() { return _contextStack.front(); }

      // Pushes a stack operation to the queue to be excecuted after the window has been renderered.
      void stackOperation( StackOperation op ) { _stackOperationQueue.push( op ); }


      // Fulfill the interface for a component
      // Register game-wide events with the manager
      virtual void registerEvents( InputManager& ) override;

      // Interfaces for input
      virtual void eventAction( const RegolithEvent&, const SDL_Event& );



      /*
       * Stack operation class.
       * This allows a queue of operations to be created in parallel to the update/render loop.
       * The operations are cached until they can be safely performed without accidentally invalidating the
       * stack iterators.
       */
      struct StackOperation
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

#endif // REGOLITH_COMPONENTS_ENGINE_H_

