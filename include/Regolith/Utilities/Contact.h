
#ifndef REGOLITH_UTILITIES_CONTACT_H_
#define REGOLITH_UTILITIES_CONTACT_H_

#include "Regolith/Global/Global.h"


namespace Regolith
{

  struct Contact
  {
    Contact* other;

    float inertiaRatio;
    Vector impulse;
    Vector overlap;
    Vector normal;
    CollisionType type;
  };

}

#endif // REGOLITH_UTILITIES_CONTACT_H_

