#define TESTASS_APPROX_LIMIT 1.0E-6

#include "Regolith.h"
#include "Regolith/Test/TestCollision.h"

#include "testass.h"
#include "logtastic.h"

#include <iostream>


using namespace Regolith;


int main( int, char** )
{
  logtastic::init();
  logtastic::setLogFileDirectory( "./test_data/logs/" );
  logtastic::addLogFile( "tests_collision.log" );
  logtastic::start( "Regolith - Collision Tests", REGOLITH_VERSION_NUMBER );

  testass::control::init( "Regolith", "Collision" );
  testass::control::get()->setVerbosity( testass::control::verb_short );

////////////////////////////////////////////////////////////////////////////////////////////////////

  SECTION( "Architecture & Basics" );
  {
    TestCollision* object1 = new TestCollision();

    object1->setPosition( Vector( 1.0, 1.0 ) );

    const Collision& col = object1->getCollision();

    ASSERT_EQUAL( col.position().x(), 0.0 );
    ASSERT_EQUAL( col.position().y(), 1.0 );
    ASSERT_EQUAL( col.width(), 2.0 );
    ASSERT_EQUAL( col.height(), 3.0 );
  }

////////////////////////////////////////////////////////////////////////////////////////////////////

  SECTION( "Axis-Aligned Bounding Boxes - Collision" );
  {
    TestCollision* object1 = new TestCollision();
    TestCollision* object2 = new TestCollision();

    object1->setPosition( Vector( 0.0, 0.0 ) );
    object2->setPosition( Vector( 5.0, 0.0 ) );

    collides( object1, object2 );
    ASSERT_FALSE( object1->collided() );
    ASSERT_FALSE( object2->collided() );
    object1->reset();
    object2->reset();


    object2->setPosition( Vector( 1.0, 0.0 ) );
    collides( object1, object2 );
    ASSERT_TRUE( object1->collided() );
    ASSERT_TRUE( object2->collided() );
    object1->reset();
    object2->reset();


    object2->setPosition( Vector( 0.0, -1.0 ) );
    collides( object1, object2 );
    ASSERT_TRUE( object1->collided() );
    ASSERT_TRUE( object2->collided() );
    ASSERT_EQUAL( object1->lastNormal(), unitVector_y ); // Default to y direction.
    ASSERT_EQUAL( object2->lastNormal(), -unitVector_y ); // Default to y direction.
    ASSERT_APPROX_EQUAL( object1->lastOverlap(), 1.0 );
    ASSERT_APPROX_EQUAL( object2->lastOverlap(), 1.0 );
    object1->reset();
    object2->reset();


    object2->setPosition( Vector( -1.9999, 0.0 ) );
    collides( object1, object2 );
    ASSERT_TRUE( object1->collided() );
    ASSERT_TRUE( object2->collided() );
    ASSERT_EQUAL( object1->lastNormal(), unitVector_x );
    ASSERT_EQUAL( object2->lastNormal(), -unitVector_x );
    ASSERT_APPROX_EQUAL( object1->lastOverlap(), 5.0E-5 );
    ASSERT_APPROX_EQUAL( object2->lastOverlap(), 5.0E-5 );
    object1->reset();
    object2->reset();

    object2->setPosition( Vector( 0.0, 3.0 ) );
    collides( object1, object2 );
    ASSERT_FALSE( object1->collided() );
    ASSERT_FALSE( object2->collided() );
    object1->reset();
    object2->reset();


    object2->setPosition( Vector( 1.9999, 0.0 ) );
    collides( object1, object2 );
    ASSERT_TRUE( object1->collided() );
    ASSERT_TRUE( object2->collided() );
    ASSERT_EQUAL( object1->lastNormal(), -unitVector_x );
    ASSERT_EQUAL( object2->lastNormal(), unitVector_x );
    ASSERT_APPROX_EQUAL( object1->lastOverlap(), 5.0E-5 );
    ASSERT_APPROX_EQUAL( object2->lastOverlap(), 5.0E-5 );
    object1->reset();
    object2->reset();


    object2->setPosition( Vector( 0.0, -2.9999 ) );
    collides( object1, object2 );
    ASSERT_TRUE( object1->collided() );
    ASSERT_TRUE( object2->collided() );
    ASSERT_EQUAL( object1->lastNormal(), unitVector_y );
    ASSERT_EQUAL( object2->lastNormal(), -unitVector_y );
    ASSERT_APPROX_EQUAL( object1->lastOverlap(), 5.0E-5 );
    ASSERT_APPROX_EQUAL( object2->lastOverlap(), 5.0E-5 );
    object1->reset();
    object2->reset();


    object2->setPosition( Vector( 0.0, -3.0 ) );
    collides( object1, object2 );
    ASSERT_FALSE( object1->collided() );
    ASSERT_FALSE( object2->collided() );
    object1->reset();
    object2->reset();


    object2->setPosition( Vector( 0.2, -2.6 ) );
    collides( object1, object2 );
    ASSERT_TRUE( object1->collided() );
    ASSERT_TRUE( object2->collided() );
    ASSERT_EQUAL( object1->lastNormal(), unitVector_y );
    ASSERT_EQUAL( object2->lastNormal(), -unitVector_y );
    ASSERT_APPROX_EQUAL( object1->lastOverlap(), 0.2 );
    object1->reset();
    object2->reset();

  }


////////////////////////////////////////////////////////////////////////////////////////////////////

  SECTION( "Axis-Aligned Bounding Boxes - Containment" );
  {
  }

////////////////////////////////////////////////////////////////////////////////////////////////////

  if ( ! testass::control::summarize() )
  {
    testass::control::printReport( std::cout );
  }

  testass::control::kill();
  logtastic::stop();
  return 0;
}

