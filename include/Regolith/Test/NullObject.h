
#ifndef REGOLITH_TEST_NULL_OBJECT_H_
#define REGOLITH_TEST_NULL_OBJECT_H_

#include "Regolith/Global/Global.h"
#include "Regolith/Architecture/GameObject.h"


namespace Regolith
{
  class Camera;
  class DataHandler;

  class NullObject : public GameObject
  {
    private:
      std::vector<int> _someData;

    protected:

    public:
      NullObject();

      virtual ~NullObject();

      // Configure the object
      virtual void configure( Json::Value&, ContextGroup&, DataHandler& ) override;

  };

}

#endif // REGOLITH_TEST_NULL_OBJECT_H_

