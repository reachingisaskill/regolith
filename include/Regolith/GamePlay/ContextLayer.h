
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
      // Caches

      // List of all objects in the scene
      PhysicalObjectList _sceneGraph;

      // Map of all teams.
      TeamMap _teams;


////////////////////////////////////////////////////////////////////////////////
    public:
      // Constuct
      ContextLayer();
      // Clear all the caches
      ~ContextLayer();

      // Configure with position, movement scale, width and height
      void configure( Context*, Vector, Vector, float, float );


////////////////////////////////////////////////////////////////////////////////
      // Frame update functions

      // Apply physics processes and collision
      void update( float );

      // Recursively render the scene
      void render( Camera& );


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
      // Object spawning

      // Spawn a copy of the provided object at the requested position 
      void spawn( PhysicalObject*, const Vector& );

  };

}

#endif // REGOLITH_ARCHITECTURE_CONTEXT_LAYER_H_

