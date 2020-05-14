
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
      Vector _position;
      float _layerWidth;
      float _layerHeight;
      Vector _limit;
      // TBD
//      float _zoom;

      // Details when following
      PhysicalObject* _theObject;
      Vector _offset;

      // Scale the movement rate
      Vector _velocityScale;

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


      // Configure the camera using layer width/height and movement scale
      void configure( int, int, Vector );


      // Set the position of the camera
      void setPosition( Vector );



      // Function to update the camera's behaviour on every frame
      virtual void update( float ) override;

      const Vector& getPosition() const { return _position; }


      float getWidth() const { return _layerWidth; }
      float getHeight() const { return _layerHeight; }



      // Places the object's rectang inside the window
      SDL_Rect place( const SDL_Rect& ) const;



      // Function to follow an element
      // Update the objects behaviour based on the provided timestep
      virtual void followMe( PhysicalObject* );
  };

}

#endif // REGOLITH_GAMEPLAY_CAMERA_H_

