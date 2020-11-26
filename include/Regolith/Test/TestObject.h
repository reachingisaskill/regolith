
#ifndef REGOLITH_TEST_TEST_OBJECT_H_
#define REGOLITH_TEST_TEST_OBJECT_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/GameObject.h"


namespace Regolith
{
  class Camera;
  class DataHandler;

  class TestObject : public GameObject
  {
    private:
      std::vector<int> _someData;

    protected:

    public:
      TestObject();

      virtual ~TestObject();

      // Configure the object
      virtual void configure( Json::Value&, ContextGroup& ) override;

  };

}

#endif // REGOLITH_TEST_TEST_OBJECT_H_

