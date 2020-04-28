
#include "regolith.h"

#include "logtastic.h"
#include "testass.h"


using namespace Regolith;

int main( int, char** )
{
  logtastic::init();
  logtastic::setLogFileDirectory( "./test_data/logs/" );
  logtastic::addLogFile( "tests_named_vector.log" );
  logtastic::start( "Regolith - Named Vector Tests", REGOLITH_VERSION_NUMBER );

  testass::control::init( "Regolith", "Named Vector" );
  testass::control::get()->setVerbosity( testass::control::verb_short );

////////////////////////////////////////////////////////////////////////////////////////////////////

    NamedVector<int> test_vector("test");

    ASSERT_EQUAL( test_vector.mapSize(),    0u );
    ASSERT_EQUAL( test_vector.vectorSize(), 0u );

    size_t result1 = test_vector.addName( "name1" );
    ASSERT_EQUAL( result1, 0u );
    result1 = test_vector.addName( "name1" );
    ASSERT_EQUAL( result1, 0u );
    result1 = test_vector.addName( "name1" );
    ASSERT_EQUAL( test_vector.mapSize(),    1u );
    ASSERT_EQUAL( test_vector.vectorSize(), 1u );

    size_t result2 = test_vector.addName( "name2" );
    size_t result3 = test_vector.addName( "name3" );
    ASSERT_EQUAL( test_vector.mapSize(),    3u );
    ASSERT_EQUAL( test_vector.vectorSize(), 3u );
    ASSERT_EQUAL( result2, 1u );
    ASSERT_EQUAL( result3, 2u );

    ASSERT_EQUAL( test_vector.getID( "name1" ), 0u );
    ASSERT_EQUAL( test_vector.getID( "name2" ), 1u );
    ASSERT_EQUAL( test_vector.getID( "name3" ), 2u );

    ASSERT_NULL( test_vector.get( 0u ) );
    ASSERT_NULL( test_vector.get( 1u ) );
    ASSERT_NULL( test_vector.get( 2u ) );

    ASSERT_NULL( test_vector[ 0u ] );
    ASSERT_NULL( test_vector[ 1u ] );
    ASSERT_NULL( test_vector[ 2u ] );

    test_vector.addObject( new int( 10 ), "name1" );
    test_vector.addObject( new int( 11 ), "name2" );
    test_vector.addObject( new int( 12 ), "name3" );

    ASSERT_EQUAL( test_vector.mapSize(),    3u );
    ASSERT_EQUAL( test_vector.vectorSize(), 3u );

    ASSERT_NOT_NULL( test_vector[ 0u ] );
    ASSERT_NOT_NULL( test_vector[ 1u ] );
    ASSERT_NOT_NULL( test_vector[ 2u ] );

    ASSERT_EQUAL( *test_vector.get( 0 ), 10 );
    ASSERT_EQUAL( *test_vector.get( 1 ), 11 );
    ASSERT_EQUAL( *test_vector.get( 2 ), 12 );

    ASSERT_EQUAL( *test_vector[ 0 ], 10 );
    ASSERT_EQUAL( *test_vector[ 1 ], 11 );
    ASSERT_EQUAL( *test_vector[ 2 ], 12 );

    test_vector.addObject( new int( 13 ), "name4" );
    ASSERT_EQUAL( test_vector.getID( "name4" ), 3u );
    ASSERT_EQUAL( *test_vector.get( 3 ), 13 );

    ASSERT_EQUAL( test_vector.mapSize(),    4u );
    ASSERT_EQUAL( test_vector.vectorSize(), 4u );

    test_vector.addObject( new int( 20 ), "name1" );
    ASSERT_EQUAL( *test_vector.get( 0 ), 20 );

    ASSERT_EQUAL( test_vector.mapSize(),    4u );
    ASSERT_EQUAL( test_vector.vectorSize(), 4u );

////////////////////////////////////////////////////////////////////////////////////////////////////

  if ( ! testass::control::summarize() )
  {
    testass::control::printReport( std::cout );
  }

  testass::control::kill();
  logtastic::stop();
  return 0;
}

