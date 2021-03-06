
#ifndef REGOLITH_ARCHITECTURE_MASS_PRODUCEABLE_H_
#define REGOLITH_ARCHITECTURE_MASS_PRODUCEABLE_H_

#include "Regolith/Global/Json.h"


namespace Regolith
{

  /*
   * This class defines the mass-producedable interface.
   * Forces classes to define a configure function to enable the templated factory pattern.
   */
  template < class ... ARGS >
  class MassProduceable
  {
    public:
      // Virtual destructor
      virtual ~MassProduceable() {}


      // Implemented by every derived class to configure the objects based on the json configuration data
      virtual void configure( Json::Value&, ARGS... ) = 0;
  };

}

#endif // REGOLITH_ARCHITECTURE_MASS_PRODUCEABLE_H_

