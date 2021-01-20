
#ifndef REGOLITH_UTILITIES_BOUNDING_BOX_H_
#define REGOLITH_UTILITIES_BOUNDING_BOX_H_

#include "Regolith/Utilities/Vector.h"


namespace Regolith
{

  // Structure to store the 
  struct BoundingBox
  {
    // Basic dimensions
    float width;
    float height;

    // Vertices position (0th vertex is the box's position)
    Vector points[4];

    // Edge normals (Up, right, down, left)
    static const Vector normals[4];

    // Position, width, height
    void configure( Vector, float, float );
  };

}

#endif // REGOLITH_UTILITIES_BOUNDING_BOX_H_

