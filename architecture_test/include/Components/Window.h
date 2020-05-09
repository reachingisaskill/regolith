
#ifndef REGOLITH_COMPONENTS_WINDOW_H_
#define REGOLITH_COMPONENTS_WINDOW_H_

#include "Global/Global.h"
#include "Architecture/Component.h"

#include <string>


namespace Regolith
{

  /*
   * Window wrapper class.
   * Inherits a singleton patter to make access to window parameters through static members more simple.
   * If multiple windows are required in future applications, make this a value templated class, on the 
   * integer template parameter: <int WINDOW_NUMBER>.
   */
  class Window : public Component, public Singleton< Window >
  {
    friend class Singleton<Window>;

    private:
      SDL_Window* _theWindow;
      std::string _title;
      int _width;
      int _height;
      int _resolutionWidth;
      int _resolutionHeight;
      bool _mouseFocus;
      bool _keyboardFocus;
      bool _minimized;
      bool _fullscreen;

      static float _scaleX;
      static float _scaleY;

    protected:
      // Empty private initialisation
      Window();

    public:
      // On destruction delete the window pointer
      virtual ~Window() { this->free(); }


      // Configure the window return the SDL renderer
      SDL_Renderer* init( int, int );


      // Get the current window dimensions
      int getWidth() const { return _width; }
      int getHeight() const { return _height; }

      // Get the resolution of the displayed data
      int getResolutionWidth() const { return _width; }
      int getResolutionHeight() const { return _height; }



      // Boolean functions to interrogate the current window state
      bool hasMouseFocus() const { return _mouseFocus; }
      bool hasKeyboardFocus() const { return _keyboardFocus; }
      bool isMinimized() const { return _minimized; }
      void toggleFullScreen();



      // Component event handling functionality
      // Register game-wide events with the manager
      virtual void registerEvents( InputManager* );

      // Regolith events
      virtual void eventAction( const RegolithEvent&, const SDL_Event& );


      // Static member functions to return the current scale factors for rendering
      static float renderScaleX() { return _scaleX; }
      static float renderScaleY() { return _scaleY; }
  };

}

#endif // REGOLITH_COMPONENTS_WINDOW_H_

