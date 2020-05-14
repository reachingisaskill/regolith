
#ifndef REGOLITH_ARCHITECTURE_CONTEXT_LAYER_H_
#define REGOLITH_ARCHITECTURE_CONTEXT_LAYER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/Architecture/Drawable.h"
#include "Regolith/Architecture/Moveable.h"
#include "Regolith/Architecture/Collidable.h"
#include "Regolith/Architecture/Clickable.h"
#include "Regolith/GamePlay/Camera.h"

#include <list>
#include <set>


namespace Regolith
{

  typedef std::list< Drawable* > DrawableList;
  typedef std::list< Moveable* > MoveableList;
  typedef std::list< Collidable* > CollidableList;
  typedef std::map< int, CollidableList > TeamMap;
  typedef std::set< Clickable* > ClickableSet;

  class ContextLayer
  {

    private:
      Camera _camera;
      Vector _position;

    public:
      // Constuct with position, width and height
      ContextLayer( Vector, float, float, Vector );
      // Clear all the caches
      ~ContextLayer();


      // Set the object that the camera should follow
      void setCameraFollow( PhysicalObject* obj ) { _camera.followMe( obj ); }

      // Update the camera's current position
      void update( float time ) { _camera.update( time ); }


      // List of all drawable objects
      DrawableList drawables;

      // List of all moveable objects
      MoveableList moveables;

      // Map of all teams. Note a team cannot collide with it's own members!
      TeamMap teams;

      // Set of all the objects that are "clickable"
      ClickableSet clickables;


      // Return a const reference to the camera object to allow render to place objects
      const Camera& getCamera() const { return _camera; }

      // Return the position of the layer
      const Vector& getPosition() const { return _position; }
  };

}

#endif // REGOLITH_ARCHITECTURE_CONTEXT_LAYER_H_

