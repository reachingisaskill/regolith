
#ifndef REGOLITH_GAMEPLAY_CAMERA_H_
#define REGOLITH_GAMEPLAY_CAMERA_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/AnimatedInterface.h"
#include "Regolith/Architecture/ControllableInterface.h"


namespace Regolith
{
  class PhysicalObject;
  class ContextLayer;


  class Camera : public AnimatedInterface
  {
    private:
      Vector _position;
      Vector _lowerLimit;
      Vector _upperLimit;
      // TBD
//      float _zoom;

      // Details when following
      PhysicalObject* _theObject;
      Vector _offset;

      // References to the global window scaling ratios
      const float& _scaleX;
      const float& _scaleY;

      // Position within the current layer
      Vector _layerPosition;

    protected:
      inline void checkPosition();

    public:
      // Create invalid instance
      Camera();

      // Destructor
      virtual ~Camera() {}


      // Configure the camera using layer width/height and movement scale
      void configure( Vector, Vector );


      // Set the position of the camera
      void setPosition( Vector );



      // Function to update the camera's behaviour on every frame
      virtual void update( float ) override;

      const Vector& getPosition() const { return _position; }



      // Set the current layer
      void setLayer( ContextLayer* layer );

      // Places the object's rectang inside the window
      SDL_Rect place( const SDL_Rect& ) const;



      // Function to follow an element
      // Update the objects behaviour based on the provided timestep
      virtual void followMe( PhysicalObject* );
  };

}

#endif // REGOLITH_GAMEPLAY_CAMERA_H_

