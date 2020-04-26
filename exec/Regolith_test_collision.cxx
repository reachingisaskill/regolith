#define TESTASS_APPROX_LIMIT 1.0E-6

#include "regolith.h"
#include "TestDrawable.h"

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
    Drawable* object1 = (Drawable*) new TestDrawable();
    Drawable* object2 = (Drawable*) new TestDrawable();

    object1->setPosition( Vector( 1.0, 1.0 ) );
    object2->setPosition( Vector( -1.0, 2.0 ) );

    ASSERT_EQUAL( object1->position().x(), 1.0 );
    ASSERT_EQUAL( object1->position().y(), 1.0 );
    ASSERT_EQUAL( object2->position().x(), -1.0 );
    ASSERT_EQUAL( object2->position().y(), 2.0 );

    Collision* col = nullptr;
    unsigned int count = object1->getCollision( col );

    ASSERT_EQUAL( count, 2u );
    ASSERT_EQUAL( col[0].position().x(), 0.0 );
    ASSERT_EQUAL( col[0].position().y(), 0.0 );
    ASSERT_EQUAL( col[0].width(), 1.0 );
    ASSERT_EQUAL( col[0].height(), 1.0 );

    ASSERT_EQUAL( col[1].position().x(), 2.0 );
    ASSERT_EQUAL( col[1].position().y(), 1.0 );
    ASSERT_EQUAL( col[1].width(), 0.5 );
    ASSERT_EQUAL( col[1].height(), 2.0 );
  }

////////////////////////////////////////////////////////////////////////////////////////////////////

  SECTION( "Axis-Aligned Bounding Boxes - Collision" );
  {
    Drawable* object1 = (Drawable*) new TestDrawable();
    Drawable* object2 = (Drawable*) new TestDrawable();

    object1->setPosition( Vector( 0.0, 0.0 ) );
    object2->setPosition( Vector( 5.0, 0.0 ) );

    Contact contact;
    bool result;

    ASSERT_FALSE( collides( object1, object2, contact ) );

    object2->setPosition( Vector( 1.0, 0.0 ) );
    ASSERT_FALSE( collides( object1, object2, contact ) );

    object2->setPosition( Vector( 0.0, 0.0 ) );
    result = collides( object1, object2, contact );
    ASSERT_TRUE( result );
    if ( result ) // Check just in case
    {
      ASSERT_EQUAL( contact.normal(), unitVector_y ); // Default to y direction.
      ASSERT_APPROX_EQUAL( contact.overlap(), 1.0 );
    }


    object2->setPosition( Vector( 0.99999, 0.0 ) );
    result = collides( object1, object2, contact );
    ASSERT_TRUE( result );
    if ( result ) // Check just in case
    {
      ASSERT_EQUAL( contact.normal(), unitVector_x );
      ASSERT_APPROX_EQUAL( contact.overlap(), 1.0E-5 );
    }

    object2->setPosition( Vector( 0.0, 2.0 ) );
    ASSERT_FALSE( collides( object1, object2, contact ) );

    object2->setPosition( Vector( 0.0, 1.8 ) );
    result = collides( object1, object2, contact );
    ASSERT_TRUE( result );
    if ( result ) // Check just in case
    {
      ASSERT_EQUAL( contact.normal(), unitVector_y );
      ASSERT_APPROX_EQUAL( contact.overlap(), 0.2 );
    }


    object2->setPosition( Vector( -0.99999, 0.0 ) );
    result = collides( object1, object2, contact );
    ASSERT_TRUE( result );
    if ( result ) // Check just in case
    {
      ASSERT_EQUAL( contact.normal(), -unitVector_x );
      ASSERT_APPROX_EQUAL( contact.overlap(), 1.0E-5 );
    }

    object2->setPosition( Vector( 0.0, -2.0 ) );
    ASSERT_FALSE( collides( object1, object2, contact ) );

    object2->setPosition( Vector( 0.0, -1.8 ) );
    result = collides( object1, object2, contact );
    ASSERT_TRUE( result );
    if ( result ) // Check just in case
    {
      ASSERT_EQUAL( contact.normal(), -unitVector_y );
      ASSERT_APPROX_EQUAL( contact.overlap(), 0.2 );
    }

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

