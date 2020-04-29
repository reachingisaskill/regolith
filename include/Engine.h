
#ifndef __REGOLITH__ENGINE_H__
#define __REGOLITH__ENGINE_H__

#include "Definitions.h"

#include "Singleton.h"
#include "Controllable.h"
#include "InputManager.h"
#include "Scene.h"
#include "Window.h"
#include "Timer.h"


namespace Regolith
{


  class Engine : public Singleton< Engine >
  {
    friend class Singleton< Engine >;

    private:

      SDL_Renderer* _theRenderer;
      InputManager* _inputManager;
      Window* _currentWindow;
      Scene* _currentScene;
      Camera* _currentCamera;

      Timer _frameTimer;
      SDL_Color _defaultColor;
      bool _quit;

    public:
      Engine();

      virtual ~Engine();

      void configure( SDL_Renderer*, Window*, InputManager* );

      void run();

      void setScene( Scene* );

      void setCamera( Camera* );

      void quit() { _quit = true; }

  };
  
}

#endif // __REGOLITH__ENGINE_H__

