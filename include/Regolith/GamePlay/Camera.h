
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
      // TBD
      // Need a mechanism for confining camera positions to viewable area
//      float _zoom;

      // Details when following
      PhysicalObject* _theObject;
      Vector _offset;

      // References to the global window scaling ratios
      const float& _scaleX;
      const float& _scaleY;


    public:
      // Create invalid instance
      Camera();

      // Destructor
      virtual ~Camera() {}


      /*
       * TODO:
       * - Merge the update function into the set position function. Context tells the camera where it should be, then the camera tries to get there
       * - Camera performs the searching children logic in the scene graph
       * - Remove the AnimatedInterface inheritance?
       */

      // Set the position of the camera
      void setPosition( Vector );


      // Performs the rendering operation of the provided layer
      void render( ContextLayer* ) const;



      // Function to update the camera's behaviour on every frame
      virtual void update( float ) override;

      // Return the current position of the camera
      const Vector& position() const { return _position; }



      // Places the object's rectangle inside the window
      SDL_Rect place( const SDL_Rect& ) const;



      // Function to follow an element
      // Update the objects behaviour based on the provided timestep
      virtual void followMe( PhysicalObject* );
  };

}

#endif // REGOLITH_GAMEPLAY_CAMERA_H_

