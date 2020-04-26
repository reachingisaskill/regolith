
#ifndef __REGOLITH__CAMERA_H__
#define __REGOLITH__CAMERA_H__

#include "Definitions.h"
#include "Vector.h"
#include "Controllable.h"

namespace Regolith
{
  class Drawable;
  class InputHandler;


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

      // Set the position of the camera
      void setPosition( int, int );

      // Set the size of the viewing frame
      void setSize( int, int );

      // Move the camera by a specified amount
      void move( int, int );

      // Function to update the camera's behavious on every frame
      virtual void update( Uint32 ) {}

      // Change the scale parameters based on the new window size
      void updateScale( int, int );

      int getX() { return _x; }
      int getY() { return _y; }

      int getWidth() { return _width; }
      int getHeight() { return _height; }

      // Places the object's rectang inside the window
      SDL_Rect place( const SDL_Rect& );

  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Flying Camera type

  // Registers input actions with the input handler and is directly controllable by the move actions
  class FlyingCamera : public Camera, public Controllable
  {
    private:
      int _velocityX;
      int _velocityY;
      int _speed;

    public:
      // Create invalid instance
      FlyingCamera();

      // Set the scene size and camera window size
      FlyingCamera( int, int, int, int );

      // Applies the velocity to the current position
      virtual void update( Uint32 );

      // Update the objects behaviour based on the provided timestep
      virtual void registerEvents( InputHandler* );

      // Interfaces for input
      virtual void eventAction( const InputAction& ) {}
      virtual void booleanAction( const InputAction&, bool );
      virtual void floatAction( const InputAction&, float ) {}
      virtual void vectorAction( const InputAction&, const Vector& ) {}

  };


////////////////////////////////////////////////////////////////////////////////////////////////////
  // Following Camera type

  // Follows the position of Drawable* object around the scene bounds
  class FollowingCamera : public Camera
  {
    private:
      Drawable* _theObject;
      int _offsetX;
      int _offsetY;

    public:
      // Create invalid instance
      FollowingCamera();

      // Set the scene size and camera window size
      FollowingCamera( int, int, int, int );

      // Updates the position to follow the object
      virtual void update( Uint32 );

      // Update the objects behaviour based on the provided timestep
      virtual void followMe( Drawable* );

  };

}

#endif // __REGOLITH__CAMERA_H__

