
#include "regolith.h"
#include "Test/TestData.h"

#include "logtastic.h"
#include "testass.h"


using namespace Regolith;

int main( int, char** )
{
  logtastic::init();
  logtastic::setLogFileDirectory( "./test_data/logs/" );
  logtastic::addLogFile( "tests_game_data.log" );
  logtastic::start( "Regolith - Game Data Tests", REGOLITH_VERSION_NUMBER );

  testass::control::init( "Regolith", "Game Data" );
  testass::control::get()->setVerbosity( testass::control::verb_short );

////////////////////////////////////////////////////////////////////////////////////////////////////

  TestData* data = new TestData();

  GameData::CallBack callback1 = data->registerEvent( INCREMENT_1 );
  GameData::CallBack callback2 = data->registerEvent( INCREMENT_2 );
  GameData::CallBack callback3 = data->registerEvent( INCREMENT_3 );
  GameData::CallBack callback4 = data->registerEvent( DECREMENT_1 );
  GameData::CallBack callback5 = data->registerEvent( DOUBLE_2 );
  GameData::CallBack callback6 = data->registerEvent( DECREMENT_3 );


  ASSERT_EQUAL( data->counter1, 0 );
  ASSERT_EQUAL( data->counter2, 0 );
  ASSERT_EQUAL( data->counter3, 0 );

  callback1.trigger();
  ASSERT_EQUAL( data->counter1, 1 );
  ASSERT_EQUAL( data->counter2, 0 );
  ASSERT_EQUAL( data->counter3, 0 );

  callback1.trigger();
  ASSERT_EQUAL( data->counter1, 2 );
  ASSERT_EQUAL( data->counter2, 0 );
  ASSERT_EQUAL( data->counter3, 0 );

  callback2.trigger();
  callback2.trigger();
  callback2.trigger();
  ASSERT_EQUAL( data->counter1, 2 );
  ASSERT_EQUAL( data->counter2, 3 );
  ASSERT_EQUAL( data->counter3, 0 );

  callback2.trigger();
  callback2.trigger();
  callback3.trigger();
  callback4.trigger();
  callback4.trigger();
  callback6.trigger();
  callback6.trigger();
  callback1.trigger();
  callback5.trigger();
  ASSERT_EQUAL( data->counter1, 1 );
  ASSERT_EQUAL( data->counter2, 10 );
  ASSERT_EQUAL( data->counter3, -1 );


////////////////////////////////////////////////////////////////////////////////////////////////////

  if ( ! testass::control::summarize() )
  {
    testass::control::printReport( std::cout );
  }

  testass::control::kill();
  logtastic::stop();
  return 0;
}

