
#ifndef REGOLITH_COMPONENTS_WINDOW_H_
#define REGOLITH_COMPONENTS_WINDOW_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/Component.h"

#include <string>


namespace Regolith
{

  /*
   * Window wrapper class.
   */
  class Window : public Component
  {
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

      float _scaleX;
      float _scaleY;

    protected:

    public:
      // Empty private initialisation
      Window();

      // On destruction delete the window pointer
      virtual ~Window();


      // Configure the window return the SDL renderer
      void init( std::string, int, int );

      // Return the SDL window pointer
      SDL_Window* getSDLWindow() const { return _theWindow; }


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
      virtual void registerEvents( InputManager& ) override;

      // Regolith events
      virtual void eventAction( const RegolithEvent&, const SDL_Event& ) override;


      // Static member functions to return the current scale factors for rendering
      const float& renderScaleX() { return _scaleX; }
      const float& renderScaleY() { return _scaleY; }
  };

}

#endif // REGOLITH_COMPONENTS_WINDOW_H_

