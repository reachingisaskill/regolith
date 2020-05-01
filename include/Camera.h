
#ifndef REGOLITH_CAMERA_H_
#define REGOLITH_CAMERA_H_

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
      int _limitX;
      int _limitY;
      int _x;
      int _y;
      // Details when flying
      int _velocityX;
      int _velocityY;
      int _speed;
      // Details when following
      Drawable* _theObject;
      int _offsetX;
      int _offsetY;

      // Parameters to rescale widths and heights if the window changes size
      static float _scaleX;
      static float _scaleY;
      static int _windowWidth;
      static int _windowHeight;

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

      int getX() { return _x; }
      int getY() { return _y; }

      int getWidth() { return _windowWidth; }
      int getHeight() { return _windowHeight; }

      // Places the object's rectang inside the window
      SDL_Rect place( const SDL_Rect& );

      // Functions for flying mode
      // Update the objects behaviour based on the provided timestep
      virtual void registerEvents( InputManager* ) {}

      // Update the objects behaviour based on the provided timestep
      virtual void registerActions( InputHandler* );

      // Interfaces for input
      virtual void eventAction( const RegolithEvent&, const SDL_Event& ) {}
      virtual void booleanAction( const InputAction&, bool );
      virtual void floatAction( const InputAction&, float ) {}
      virtual void vectorAction( const InputAction&, const Vector& ) {}


      // Function to follow an element
      // Update the objects behaviour based on the provided timestep
      virtual void followMe( Drawable* );


      // Change the scale parameters based on the new window size
      static void updateScale( int, int );
  };

}

#endif // REGOLITH_CAMERA_H_

