
#include "Regolith/GameObjects/NullObject.h"
#include "Regolith/Managers/DataHandler.h"
#include "Regolith/GamePlay/Camera.h"


namespace Regolith
{

  NullObject::NullObject() :
    GameObject(),
    _someData(1000)
  {
    INFO_LOG( "NullObject::NullObject : Creating null object" );

    for ( unsigned int i = 0; i < 1000; ++i )
    {
      _someData[i] = i;
    }
  }


  NullObject::~NullObject()
  {
    INFO_LOG( "NullObject::NullObject : Destroying null object" );
  }


  void NullObject::configure( Json::Value&, ContextGroup&, DataHandler& )
  {
  }
}

