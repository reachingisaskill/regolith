
#ifndef REGOLITH_ARCHITECTURE_MASS_PRODUCEABLE_H_
#define REGOLITH_ARCHITECTURE_MASS_PRODUCEABLE_H_

#include "Regolith/Global/Json.h"


namespace Regolith
{
  namespace Architecture
  {

    /*
     * This class defines the mass-producedable interface.
     * Forces classes to define a configure function to enable the templated factory pattern.
     */
    class MassProduceable
    {
      public:
        virtual ~MassProduceable() {}

        void configure( Json::Value& ) = 0;
    };

  }
}

#endif // REGOLITH_ARCHITECTURE_MASS_PRODUCEABLE_H_

