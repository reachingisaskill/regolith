
#ifndef __REGOLITH__CAMERA_H__
#define __REGOLITH__CAMERA_H__

#include "Definitions.h"
#include "Vector.h"
#include "Controllable.h"

namespace Regolith
{
  class Drawable;
  class InputHandler;


  class Camera : public Controllable
  {
    private:
      CameraMode _currentMode;
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
      // Details when flying
      int _velocityX;
      int _velocityY;
      int _speed;
      // Details when following
      Drawable* _theObject;
      int _offsetX;
      int _offsetY;

    protected:
      int& x() { return _x; }
      int& y() { return _y; }

      void checkPosition();

    public:
      // Create invalid instance
      Camera();

      // Set the scene size and camera window size
      Camera( int, int, int, int );

      // Destructor
      virtual ~Camera() {}

      // Configure the camera using scene width/height and view width/height
      void configure( int, int, int, int );

      // Set the current camera mode
      void setMode( CameraMode mode ) { _currentMode = mode; }

      // Set the position of the camera
      void setPosition( int, int );

      // Set the size of the viewing frame
      void setSize( int, int );

      // Move the camera by a specified amount
      void move( int, int );

      void setVelocity( int vx, int vy ) { _velocityX = vx; _velocityY = vy; }

      // Function to update the camera's behavious on every frame
      virtual void update( Uint32 );

      // Change the scale parameters based on the new window size
      void updateScale( int, int );

      int getX() { return _x; }
      int getY() { return _y; }

      int getWidth() { return _width; }
      int getHeight() { return _height; }

      // Places the object's rectang inside the window
      SDL_Rect place( const SDL_Rect& );

      // Functions for flying mode
      // Update the objects behaviour based on the provided timestep
      virtual void registerEvents( InputHandler* );

      // Interfaces for input
      virtual void eventAction( const InputAction& ) {}
      virtual void booleanAction( const InputAction&, bool );
      virtual void floatAction( const InputAction&, float ) {}
      virtual void vectorAction( const InputAction&, const Vector& ) {}


      // Function to follow an element
      // Update the objects behaviour based on the provided timestep
      virtual void followMe( Drawable* );
  };

}

#endif // __REGOLITH__CAMERA_H__

