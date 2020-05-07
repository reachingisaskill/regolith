
#include "Test/TestData.h"


namespace Regolith
{

  TestData::TestData() :
    GameData(),
    counter1( 0 ),
    counter2( 0 ),
    counter3( 0 )
  {
    addEvent( "increment_1", INCREMENT_1 );
    addEvent( "increment_2", INCREMENT_2 );
    addEvent( "increment_3", INCREMENT_3 );
    addEvent( "increment_1", DECREMENT_1 );
    addEvent( "double_2", DOUBLE_2 );
    addEvent( "increment_3", DECREMENT_3 );
  }

  TestData::~TestData()
  {
  }


  void TestData::handleEvent( unsigned int id )
  {
    switch ( (TestGameEvent)id )
    {
      case INCREMENT_1 :
        ++counter1;
        break;

      case INCREMENT_2 :
        ++counter2;
        break;

      case INCREMENT_3 :
        ++counter3;
        break;

      case DECREMENT_1 :
        --counter1;
        break;

      case DOUBLE_2 :
        counter2 *= 2;
        break;

      case DECREMENT_3 :
        --counter3;
        break;

      default:
        break;
    };
  }

}

