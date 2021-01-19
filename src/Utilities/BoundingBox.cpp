
#include "Regolith/Utilities/BoundingBox.h"


namespace Regolith
{

  const Vector BoundingBox::normals[4] = { Vector( 0, -1 ), Vector( 1, 0 ), Vector( 0, 1 ), Vector( -1, 0 ) };

  void BoundingBox::configure( Vector position, float w, float h )
  {
    width = w;
    height = h;

    points[0] = position;

    points[1] = points[0];
    points[1].x() = points[1].x() + w;

    points[2] = points[1];
    points[2].y() = points[2].y() + h;

    points[3] = points[2];
    points[3].x() = points[3].x() - w;

    DEBUG_STREAM << "BoundingBox::configure : Pos = " << position << ", W = " << width << ", H = " << height;
  }

}

