
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

    protected:

    public:
      // Create invalid instance
      Camera();

      // Set the scene size and camera window size
      Camera( int, int, int, int );

      void configure( int, int, int, int );

      void setPosition( int, int );

      void setSize( int, int );

      void move( int, int );

      int getX() { return _x; }
      int getY() { return _y; }

      // Places the object's rectang inside the window
      SDL_Rect place( const SDL_Rect& );

  };

}

#endif // __REGOLITH__CAMERA_H__

