
#ifndef REGOLITH_TEST_TEST_DATA_H_
#define REGOLITH_TEST_TEST_DATA_H_

#include "Regolith/GamePlay/GameData.h"


namespace Regolith
{


  enum TestGameEvent
  {
    INCREMENT_1,
    INCREMENT_2,
    INCREMENT_3,
    DECREMENT_1,
    DOUBLE_2,
    DECREMENT_3,
    TEST_GAME_EVENT_MAX
  };


  class TestData : public GameData
  {
    public :
      int counter1;
      int counter2;
      int counter3;


      TestData();

      virtual ~TestData();

      virtual void handleEvent( unsigned int );
  };

////////////////////////////////////////////////////////////////////////////////////////////////////

}

#endif // REGOLITH_TEST_TEST_DATA_H_

