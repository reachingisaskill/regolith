
#ifndef REGOLITH_TEST_TEST_OBJECT_H_
#define REGOLITH_TEST_TEST_OBJECT_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/PhysicalObject.h"


namespace Regolith
{
  class Camera;
  class DataHandler;

  class TestObject : public PhysicalObject
  {
    private:
      std::vector<int> _someData;

    protected:

    public:
      TestObject();

      virtual ~TestObject();

      // Configure the object
      virtual void configure( Json::Value&, ContextGroup& ) override;


      // Make sure it is clonable
      virtual PhysicalObject* clone() const override { return (PhysicalObject*) new TestObject( *this ); }

  };

}

#endif // REGOLITH_TEST_TEST_OBJECT_H_

