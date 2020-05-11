
#ifndef REGOLITH_GAMEPLAY_CAMERA_H_
#define REGOLITH_GAMEPLAY_CAMERA_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/AnimatedInterface.h"
#include "Regolith/Architecture/ControllableInterface.h"


namespace Regolith
{
  class PhysicalObject;


  class Camera : public AnimatedInterface
  {
    private:
      float _layerWidth;
      float _layerHeight;
      float _limitX;
      float _limitY;
      float _x;
      float _y;
      // TBD
      float _zoom;

      // Details when following
      PhysicalObject* _theObject;
      float _offsetX;
      float _offsetY;

      // References to the global window scaling ratios
      const float& _scaleX;
      const float& _scaleY;


    protected:
      void checkPosition();

    public:
      // Create invalid instance
      Camera();

      // Destructor
      virtual ~Camera() {}


      // Configure the camera using layer width/height
      void configure( int, int );


      // Set the position of the camera
      void setPosition( Vector );



      // Function to update the camera's behaviour on every frame
      virtual void update( float ) override;

      float getX() const { return _x; }
      float getY() const { return _y; }


      float getWidth() { return _layerWidth; }
      float getHeight() { return _layerHeight; }



      // Places the object's rectang inside the window
      SDL_Rect place( const SDL_Rect& ) const;



      // Function to follow an element
      // Update the objects behaviour based on the provided timestep
      virtual void followMe( PhysicalObject* );
  };

}

#endif // REGOLITH_GAMEPLAY_CAMERA_H_

