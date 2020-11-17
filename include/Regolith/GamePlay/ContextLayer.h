
#ifndef REGOLITH_ARCHITECTURE_CONTEXT_LAYER_H_
#define REGOLITH_ARCHITECTURE_CONTEXT_LAYER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/PhysicalObject.h"

#include <list>
#include <set>


namespace Regolith
{
  class Context;

  class ContextLayer
  {
    friend class Camera;

////////////////////////////////////////////////////////////////////////////////
    private:
      Context* _owner;
      Vector _position; // Can be considered as the offset wrt to the camera
      Vector _movementScale; // Movement wrt the camera position
      float _width;
      float _height;


////////////////////////////////////////////////////////////////////////////////
    public:
      // Constuct
      ContextLayer();
      // Clear all the caches
      ~ContextLayer();

      // Configure with position, movement scale, width and height
      void configure( Context*, Vector, Vector, float, float );


////////////////////////////////////////////////////////////////////////////////
      // Caches

      // List of all drawable objects
      PhysicalObjectList drawables;

      // List of all moveable objects
      PhysicalObjectList moveables;

      // Map of all teams.
      TeamMap teams;

      // Set of all the objects that are "clickable"
      PhysicalObjectSet clickables;

      // Cache of all objects that are animated
      PhysicalObjectList animated;


////////////////////////////////////////////////////////////////////////////////
      // Layer details

      // Return the position of the layer
      const Vector& getPosition() const { return _position; }

      // Return movement speed wrt camera
      const Vector& getMovementScale() const { return _movementScale; }

      // Return width & height
      const float& getWidth() const { return _width; }
      const float& getHeight() const { return _height; }


////////////////////////////////////////////////////////////////////////////////
      // Object spawning and caching

      // Spawn a copy of the provided object at the requested position 
      void spawn( PhysicalObject*, const Vector& );

      // Cache an object in this layer
      virtual void cacheObject( GameObject* );

  };

}

#endif // REGOLITH_ARCHITECTURE_CONTEXT_LAYER_H_

