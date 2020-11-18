
#ifndef REGOLITH_COMPONENTS_CAMERA_H_
#define REGOLITH_COMPONENTS_CAMERA_H_

#include "Regolith/Global/Global.h"


namespace Regolith
{
  class PhysicalObject;
  class ContextLayer;


  class Camera
  {
    private:
      SDL_Renderer*& _theRenderer;
//      float _zoom;

      // References to the dimensions of the displayed area
      const int& _width;
      const int& _height;

      // References to the global window scaling ratios
      const float& _scaleX;
      const float& _scaleY;

    protected:

    public:
      // Constructor
      Camera( SDL_Renderer*&, const int&, const int &, const float&, const float& );

      // Destructor
      virtual ~Camera() {}

      // Clears the SDL renderer. Makes the camera invalid
      void clear();

      // Places the object's rectangle inside the window
      SDL_Rect place( const SDL_Rect& ) const;


      // Resets the rendering state for the next frame
      void resetRender() const;


      // Performs the back to front buffer blit
      void draw() const;


      // Render a specific raw texture
      void renderRawTexture( RawTexture* ) const;
  };

}

#endif // REGOLITH_COMPONENTS_CAMERA_H_

