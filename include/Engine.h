
#ifndef __REGOLITH__ENGINE_H__
#define __REGOLITH__ENGINE_H__

#include "Definitions.h"

#include "Singleton.h"
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
      Window* _currentWindow;
      Scene* _currentScene;
      Camera* _currentCamera;

      Timer _frameTimer;
      SDL_Color _defaultColor;

    public:
      Engine();

      virtual ~Engine();

      void configure( SDL_Renderer*, Window* );

      void run();

      void setScene( Scene* );

      void setCamera( Camera* );

      void resolveUserEvent( GameEvent );
  };
  
}

#endif // __REGOLITH__ENGINE_H__

