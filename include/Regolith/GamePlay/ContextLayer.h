
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


      // Map all objects organised by collision team
      LayerGraph layerGraph;


////////////////////////////////////////////////////////////////////////////////
      // Layer details

      // Return the position of the layer
      constexpr const Vector& getPosition() const { return _position; }

      // Return movement speed wrt camera
      constexpr const Vector& getMovementScale() const { return _movementScale; }

      // Return width & height
      constexpr const float& getWidth() const { return _width; }
      constexpr const float& getHeight() const { return _height; }

  };

}

#endif // REGOLITH_ARCHITECTURE_CONTEXT_LAYER_H_

