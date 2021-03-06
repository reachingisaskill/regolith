
#ifndef REGOLITH_CONTEXTS_CONTEXT_LAYER_H_
#define REGOLITH_CONTEXTS_CONTEXT_LAYER_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/Utilities/BoundingBox.h"

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
      std::string _name;
      Vector _position; // Can be considered as the offset wrt to the camera
      Vector _movementScale; // Movement wrt the camera position
      BoundingBox _boundingBox;


////////////////////////////////////////////////////////////////////////////////
    public:
      // Constuct
      ContextLayer();
      // Clear all the caches
      ~ContextLayer();

      // Configure with position, movement scale, width and height
      void configure( Context*, std::string, Vector, Vector, float, float );


      // Return the name of the layer
      std::string getName() const { return _name; }


      // Map all objects organised by collision team
      LayerGraph layerGraph;


////////////////////////////////////////////////////////////////////////////////
      // Layer details

      // Return the position of the layer
      constexpr const Vector& getPosition() const { return _position; }

      // Return movement speed wrt camera
      constexpr const Vector& getMovementScale() const { return _movementScale; }

      // Return width & height
      const float& getWidth() const { return _boundingBox.width; }
      const float& getHeight() const { return _boundingBox.height; }

      const BoundingBox& getBoundingBox() const { return _boundingBox; }
  };

}

#endif // REGOLITH_CONTEXTS_CONTEXT_LAYER_H_

