
#ifndef __REGOLITH__CAMERA_H__
#define __REGOLITH__CAMERA_H__

#include <SDL2/SDL.h>

namespace Regolith
{

  class Camera
  {
    private:
      int _sceneWidth;
      int _sceneHeight;
      int _width;
      int _height;
      int _limitX;
      int _limitY;
      int _x;
      int _y;
      // Parameters to rescale widths and heights if the window changes size
      float _scaleX;
      float _scaleY;

    protected:

    public:
      // Create invalid instance
      Camera();

      // Set the scene size and camera window size
      Camera( int, int, int, int );

      // Configure the camera using scene width/height and view width/height
      void configure( int, int, int, int );

      // Set the position of the camera
      void setPosition( int, int );

      // Set the size of the viewing frame
      void setSize( int, int );

      // Move the camera by a specified amount
      void move( int, int );

      // Change the scale parameters based on the new window size
      void updateScale( int, int );

      int getX() { return _x; }
      int getY() { return _y; }

      // Places the object's rectang inside the window
      SDL_Rect place( const SDL_Rect& );

  };

}

#endif // __REGOLITH__CAMERA_H__

