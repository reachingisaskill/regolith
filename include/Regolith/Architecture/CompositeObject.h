
#ifndef REGOLITH_ARCHITECTURE_COMPOSITE_OBJECT_H_
#define REGOLITH_ARCHITECTURE_COMPOSITE_OBJECT_H_

#include "Regolith/Architecture/PhysicalObject.h"


namespace Regolith
{

  // Forward declaration
  class PhysicObjectVector;


  class CompositeObject : virtual public PhysicalObject
  {
    private:

    public:
      CompositeObject() {}
      virtual ~CompositeObject() {}


      // Signal that derived classes have children
      virtual bool hasChildren() const { return true; }


      // Returns the vector of children that are active for the current state
      const PhysicalObjectVector& getChildren() const = 0;

  };

}

#endif // REGOLITH_ARCHITECTURE_COMPOSITE_OBJECT_H_

