
#ifndef REGOLITH_ARCHITECTURE_DRAWABLE_OBJECT_H_
#define REGOLITH_ARCHITECTURE_DRAWABLE_OBJECT_H_

#include "Regolith/Architecture/PhysicalObject.h"
#include "Regolith/Architecture/Texture.h"


namespace Regolith
{
  // Forward declaration
  class Texture;

  class DrawableObject : virtual public PhysicalObject
  {
    private:

    public:
      DrawableObject() {}

      virtual ~DrawableObject() {}


      // Signal that derived classes are drawable
      virtual bool hasTexture() const override { return true; }


      // For the camera to request the current renderable texture
      virtual Texture& getTexture() = 0;

  };

}

#endif // REGOLITH_ARCHITECTURE_DRAWABLE_OBJECT_H_

