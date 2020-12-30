
#include "Regolith/Test/TestObject.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/Utilities/JsonValidation.h"


namespace Regolith
{

  TestObject::TestObject() :
    PhysicalObject()
  {
    INFO_LOG( "TestObject::TestObject : Creating test object" );
  }


  TestObject::~TestObject()
  {
    INFO_LOG( "TestObject::TestObject : Destroying null object" );
  }


  void TestObject::configure( Json::Value& json_data, ContextGroup& cg )
  {
//    PhysicalObject::configure( json_data, cg );

    if ( validateJson( json_data, "load_time", JsonType::INTEGER, false ) )
    {
      unsigned int sleep = json_data["load_time"].asInt();
      DEBUG_STREAM << "TestObject::configure : Configuring for " << sleep << " milliseconds.";

      std::this_thread::sleep_for( std::chrono::milliseconds( sleep ) );
    }
  }
}

