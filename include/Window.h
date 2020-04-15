
#ifndef __REGOLITH__WINDOW_H__
#define __REGOLITH__WINDOW_H__

#include <SDL2/SDL.h>

#include <string>

namespace Regolith
{

  class Window
  {
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

      SDL_Renderer* init( int, int );

      void handleEvent( SDL_Event& );

      void free();

      int getWidth() const { return _width; }
      int getHeight() const { return _height; }


      bool hasMouseFocus() const { return _mouseFocus; }
      bool hasKeyboardFocus() const { return _keyboardFocus; }
      bool isMinimized() const { return _minimized; }


      Window( const Window& ) = delete;
      Window& operator=( const Window& ) = delete;
  };

}

#endif // __REGOLITH__WINDOW_H__

