
#include "Regolith/Test/TestObject.h"
#include "Regolith/Managers/DataHandler.h"


namespace Regolith
{

  TestObject::TestObject() :
    GameObject(),
    _someData(1000)
  {
    INFO_LOG( "TestObject::TestObject : Creating null object" );

    for ( unsigned int i = 0; i < 1000; ++i )
    {
      _someData[i] = i;
    }
  }


  TestObject::~TestObject()
  {
    INFO_LOG( "TestObject::TestObject : Destroying null object" );
  }


  void TestObject::configure( Json::Value&, ContextGroup&, DataHandler& )
  {
  }
}

