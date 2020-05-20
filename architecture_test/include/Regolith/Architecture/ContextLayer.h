
#ifndef REGOLITH_ARCHITECTURE_CONTEXT_LAYER_H_
#define REGOLITH_ARCHITECTURE_CONTEXT_LAYER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/Architecture/Drawable.h"
#include "Regolith/Architecture/Moveable.h"
#include "Regolith/Architecture/Collidable.h"
#include "Regolith/Architecture/Clickable.h"

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
    friend class Camera;

    private:
      Vector _position; // Can be considered as the offset wrt to the camera
      Vector _movementScale; // Movement wrt the camera position
      float _width;
      float _height;

    public:
      // Constuct with position, movement scale, width and height
      ContextLayer( Vector, Vector, float, float );
      // Clear all the caches
      ~ContextLayer();



      // List of all drawable objects
      DrawableList drawables;

      // List of all moveable objects
      MoveableList moveables;

      // Map of all teams. Note a team cannot collide with it's own members!
      TeamMap teams;

      // Set of all the objects that are "clickable"
      ClickableSet clickables;


      // Return the position of the layer
      const Vector& getPosition() const { return _position; }

      // Return movement speed wrt camera
      const Vector& getMovementScale() const { return _movementScale; }

      // Return width & height
      const float& getWidth() const { return _width; }
      const float& getHeight() const { return _height; }


  };

}

#endif // REGOLITH_ARCHITECTURE_CONTEXT_LAYER_H_

