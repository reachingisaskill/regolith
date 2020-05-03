
#ifndef REGOLITH_ENGINE_H_
#define REGOLITH_ENGINE_H_

#include "Definitions.h"

#include "Singleton.h"
#include "Controllable.h"
#include "InputManager.h"
#include "Scene.h"
#include "Window.h"
#include "Timer.h"


namespace Regolith
{


  class Engine : public Singleton< Engine >, public Controllable
  {
    friend class Singleton< Engine >;

    private:

      SDL_Renderer* _theRenderer;
      InputManager* _inputManager;
      Window* _currentWindow;
      Scene* _currentScene;

      Timer _frameTimer;
      SDL_Color _defaultColor;
      bool _quit;
      unsigned int _status;

    public:
      Engine();

      virtual ~Engine();

      void configure( SDL_Renderer*, Window*, InputManager* );

      void run();

      void setScene( Scene* );

      void quit() { _quit = true; }


      // Register game-wide events with the manager
      virtual void registerEvents( InputManager* );


      // Register context-wide actions with the handler
      virtual void registerActions( InputHandler* ) {}


      // Interfaces for input
      virtual void eventAction( const RegolithEvent&, const SDL_Event& );
//      virtual void booleanAction( const InputAction&, bool ) {}
//      virtual void floatAction( const InputAction&, float ) {}
//      virtual void vectorAction( const InputAction&, const Vector& ) {}
//      virtual void mouseAction( const InputAction&, bool, const Vector& ) {}

  };
  
}

#endif // REGOLITH_ENGINE_H_

