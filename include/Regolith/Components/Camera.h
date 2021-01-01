
#ifndef REGOLITH_COMPONENTS_CAMERA_H_
#define REGOLITH_COMPONENTS_CAMERA_H_

#include "Regolith/Global/Global.h"


namespace Regolith
{
  class DrawableObject;
  class Window;
  class Texture;

  class Camera
  {
    private:
      Window& _theWindow;
      SDL_Renderer* _theRenderer;
//      float _zoom;
      SDL_Color _defaultColour;

      // References to the dimensions of the displayed area
      const int& _width;
      const int& _height;

      // References to the global window scaling ratios
      const float& _scaleX;
      const float& _scaleY;

      // Store this as a class member to make the stack frame smaller
      mutable SDL_Rect _targetRect;

    protected:

    public:
      // Constructor
      Camera( Window&, SDL_Renderer*, const int&, const int &, const float&, const float& );

      // Destructor
      virtual ~Camera() {}

      // Clears the SDL renderer. Makes the camera invalid
      void clear();

      // Set the default colour used to clear the window
      void setDefaultColour( SDL_Color c ) { _defaultColour = c; }


      // Sets the renderer pointer for the camera
      void setRenderer( SDL_Renderer* ren ) { _theRenderer = ren; }


      // Resets the rendering state for the next frame
      void resetRender() const;


      // Performs the back to front buffer blit
      void draw() const;


      // Render a specific raw texture
      void renderRawTexture( RawTexture* );


      // Render a texture that has been updated
      void renderTexture( Texture& );


      // Render a physical object
      void renderDrawableObject( DrawableObject*, Vector& );


      // Destroys the sdl texture object
      void clearTexture( Texture& );
  };

}

#endif // REGOLITH_COMPONENTS_CAMERA_H_

