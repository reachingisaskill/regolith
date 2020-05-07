
#ifndef REGOLITH_WINDOW_H_
#define REGOLITH_WINDOW_H_

#include "Managers/Definitions.h"
#include "Components/Vector.h"
#include "Architecture/Controllable.h"

#include <string>


namespace Regolith
{
  class InputManager;
  class InputHandler;

  class Window : public Controllable
  {
    Window( const Window& ) = delete;
    Window& operator=( const Window& ) = delete;

    private:
      SDL_Window* _theWindow;
      std::string _title;
      int _width;
      int _height;
      bool _mouseFocus;
      bool _keyboardFocus;
      bool _minimized;
      bool _fullscreen;

      SDL_Renderer* _theRenderer;

    public:
      Window( std::string title );

      Window( Window&& ); // Move construction
      Window& operator=( Window&& ); // Move assignment

      virtual ~Window() { this->free(); }

      SDL_Renderer* init( int, int );

      void free();

      int getWidth() const { return _width; }
      int getHeight() const { return _height; }

      SDL_Renderer* getRenderer() { return _theRenderer; }


      bool hasMouseFocus() const { return _mouseFocus; }
      bool hasKeyboardFocus() const { return _keyboardFocus; }
      bool isMinimized() const { return _minimized; }
      void toggleFullScreen();



      // Register game-wide events with the manager
      virtual void registerEvents( InputManager* );

      // Register context-wide actions with the handler
      virtual void registerActions( InputHandler* ) {}


      // Interfaces for input
      // Handled and mapped actions
//      virtual void booleanAction( const InputAction&, bool ) {}
//      virtual void floatAction( const InputAction&, float ) {}
//      virtual void vectorAction( const InputAction&, const Vector& ) {}
//      virtual void mouseAction( const InputAction&, bool, const Vector& ) {}

      // Regolith events
      virtual void eventAction( const RegolithEvent&, const SDL_Event& );
  };

}

#endif // REGOLITH_WINDOW_H_

