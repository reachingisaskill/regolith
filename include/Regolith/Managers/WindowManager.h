
#ifndef REGOLITH_COMPONENTS_WINDOW_H_
#define REGOLITH_COMPONENTS_WINDOW_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/Component.h"
#include "Regolith/Links/Link.h"
#include "Regolith/Handlers/Camera.h"

#include <string>


namespace Regolith
{

  /*
   * WindowManager wrapper class.
   */
  class WindowManager : public Component
  {
    // Allow links to access the private members
    template < class T, class R > friend class Link;

    // Camera is a friend of the window
    friend class Camera;

    private:
//////////////////////////////////////////////////////////////////////////////// 
      // Member variables
      // Keep track of whether a renderer already exists
      bool _exists;

      // SDL WindowManager pointer
      SDL_Window* _theWindow;
      SDL_Renderer* _theRenderer;

      // Basic details
      std::string _title;
      SDL_Color _defaultColour;

      int _width;
      int _height;
      int _resolutionWidth;
      int _resolutionHeight;
      bool _vsyncOn;

      // Flags to track the window status
      bool _mouseFocus;
      bool _keyboardFocus;
      bool _minimized;
      bool _fullscreen;

      // Windowed mode scaling due to resize.
      float _scaleX;
      float _scaleY;

      // The camera object - The interface for rendering
      Camera _camera;

    protected:

      // Destroys the window and renderer objects
      void destroy();

      // Create the window and return the camera
      Camera& create();


    public:
//////////////////////////////////////////////////////////////////////////////// 
      // Con/Destruction

      // Empty private initialisation
      WindowManager();

      // On destruction delete the window pointer
      virtual ~WindowManager();


//////////////////////////////////////////////////////////////////////////////// 
      // Configuration

      // Configure the window
      void configure( Json::Value& );


//////////////////////////////////////////////////////////////////////////////// 
      // Accessors and modifiers

      // Get the current window dimensions
      int getWidth() const { return _width; }
      int getHeight() const { return _height; }

      // Get the resolution of the displayed data
      int getResolutionWidth() const { return _width; }
      int getResolutionHeight() const { return _height; }

      // Get the current window scaling factors
      float getScaleX() { return _scaleX; }
      float getScaleY() { return _scaleY; }

      // Update the title
      void setTitle( std::string );

//////////////////////////////////////////////////////////////////////////////// 
      // WindowManager state accessors

      // Boolean functions to interrogate the current window state
      bool hasMouseFocus() const { return _mouseFocus; }
      bool hasKeyboardFocus() const { return _keyboardFocus; }
      bool isMinimized() const { return _minimized; }
      void toggleFullScreen();


//////////////////////////////////////////////////////////////////////////////// 
      // Event handling for Regolith Components

      // Component event handling functionality
      // Register game-wide events with the manager
      virtual void registerEvents( InputManager& ) override;

      // Regolith events
      virtual void eventAction( const RegolithEvent&, const SDL_Event& ) override;
  };

}

#endif // REGOLITH_COMPONENTS_WINDOW_H_

