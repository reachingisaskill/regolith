#define TESTASS_APPROX_LIMIT 1.0E-6

#include "Regolith.h"
#include "Regolith/Links/LinkManager.h"
#include "Regolith/Collisions/Collision.h"

#include "testass.h"
#include "logtastic.h"

#include <iostream>
#include <string>


using namespace Regolith;


////////////////////////////////////////////////////////////////////////////////
  // Json Data for test 1
const std::string json_test_1 = "{ \"hit_boxes\" : [ [ { \"position\" : [ 10, 20 ], \"width\" : 100, \"height\": 200, \"type\" : \"test0\" } ], \
                                                     [ { \"position\" : [ 30, 40 ], \"width\" : 10,  \"height\": 20,  \"type\" : \"test1\" } ], \
                                                     [ { \"position\" : [ 50, 60 ], \"width\" : 20,  \"height\": 10,  \"type\" : \"test1\" } ], \
                                                     [ { \"position\" : [ 10, 20 ], \"width\" : 1,   \"height\": 2,   \"type\" : \"test1\" }, \
                                                       { \"position\" : [ 20, 30 ], \"width\" : 10,  \"height\": 20,  \"type\" : \"test2\" }, \
                                                       { \"position\" : [ 30, 40 ], \"width\" : 100, \"height\": 200, \"type\" : \"test3\" } ], \
                                                     [ { \"position\" : [ 3 , 4  ], \"width\" : 10,  \"height\": 20,  \"type\" : \"test1\" } ] ] }";

////////////////////////////////////////////////////////////////////////////////

int main( int, char** )
{
  // Configure the logger first
  logtastic::init();
  logtastic::setLogFileDirectory( "./test_data/logs/" );
  logtastic::addLogFile( "tests_collision.log" );

  // Create the manager
  Regolith::Manager* manager = Manager::createInstance();
  Regolith::Link<Regolith::Manager, Regolith::TestType> link_manager( manager );

  // Tell logging to start
  logtastic::start( "Regolith - Collision Tests", REGOLITH_VERSION_NUMBER );

  // Configure Testass
  testass::control::init( "Regolith", "Collision" );
  testass::control::get()->setVerbosity( testass::control::verb_short );

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Configure some name lookups in the manager first
  link_manager.addCollisionType( "test0", 0 );
  link_manager.addCollisionType( "test1", 1 );
  link_manager.addCollisionType( "test2", 2 );
  link_manager.addCollisionType( "test3", 3 );

  Json::CharReaderBuilder json_reader_builder;
  Json::CharReader* json_reader = json_reader_builder.newCharReader();
  Json::Value json_data;
  std::string error_string;

  Collision collision;


////////////////////////////////////////////////////////////////////////////////////////////////////

  SECTION( "Frame Configuration" );
  {
    ASSERT_EQUAL( collision.getNumberFrames(), (unsigned int) 0 );

    if ( ! json_reader->parse( json_test_1.c_str(), json_test_1.c_str() + json_test_1.size(), &json_data, &error_string ) )
    {
      ERROR_LOG( "Error occured parsins Json String" );
      ERROR_STREAM << error_string;
      error_string.clear();
    }


    collision.configure( json_data );

    ASSERT_EQUAL( collision.getNumberFrames(), (unsigned int) 5 );
    ASSERT_EQUAL( collision.getNumberHitBoxes(), (unsigned int) 1 );

    collision.setFrameNumber( 1 );
    ASSERT_EQUAL( collision.getNumberHitBoxes(), (unsigned int) 1 );

    collision.setFrameNumber( 2 );
    ASSERT_EQUAL( collision.getNumberHitBoxes(), (unsigned int) 1 );

    collision.setFrameNumber( 3 );
    ASSERT_EQUAL( collision.getNumberHitBoxes(), (unsigned int) 3 );

    collision.setFrameNumber( 4 );
    ASSERT_EQUAL( collision.getNumberHitBoxes(), (unsigned int) 1 );
  }

////////////////////////////////////////////////////////////////////////////////////////////////////

  SECTION( "Hit Box Configuration - Frame 0" );
  {
    collision.setFrameNumber( 0 );

    Collision::iterator it = collision.begin();
    Collision::iterator end = collision.end();

    ASSERT_APPROX_EQUAL( it->position.x(), 10 );
    ASSERT_APPROX_EQUAL( it->position.y(), 20 );
    ASSERT_APPROX_EQUAL( it->width, 100 );
    ASSERT_APPROX_EQUAL( it->height, 200 );
    ASSERT_EQUAL( it->type, (unsigned int) 0 );

    ASSERT_FALSE( it == end );
    ++it;
    ASSERT_TRUE( it == end );

  }

  SECTION( "Hit Box Configuration - Frame 1" );
  {

    collision.setFrameNumber( 1 );
    Collision::iterator it = collision.begin();
    Collision::iterator end = collision.end();

    ASSERT_APPROX_EQUAL( it->position.x(), 30 );
    ASSERT_APPROX_EQUAL( it->position.y(), 40 );
    ASSERT_APPROX_EQUAL( it->width, 10 );
    ASSERT_APPROX_EQUAL( it->height, 20 );
    ASSERT_EQUAL( it->type, (unsigned int) 1 );

    ASSERT_FALSE( it == end );
    ++it;
    ASSERT_TRUE( it == end );
  }

  SECTION( "Hit Box Configuration - Frame 2" );
  {

    collision.setFrameNumber( 2 );
    Collision::iterator it = collision.begin();
    Collision::iterator end = collision.end();

    ASSERT_APPROX_EQUAL( it->position.x(), 50 );
    ASSERT_APPROX_EQUAL( it->position.y(), 60 );
    ASSERT_APPROX_EQUAL( it->width, 20 );
    ASSERT_APPROX_EQUAL( it->height, 10 );
    ASSERT_EQUAL( it->type, (unsigned int) 1 );

    ASSERT_FALSE( it == end );
    ++it;
    ASSERT_TRUE( it == end );
  }

  SECTION( "Hit Box Configuration - Frame 3" );
  {
    collision.setFrameNumber( 3 );
    Collision::iterator it = collision.begin();
    Collision::iterator end = collision.end();
    ASSERT_FALSE( it == end );

    ASSERT_APPROX_EQUAL( it->position.x(), 10 );
    ASSERT_APPROX_EQUAL( it->position.y(), 20 );
    ASSERT_APPROX_EQUAL( it->width, 1 );
    ASSERT_APPROX_EQUAL( it->height, 2 );
    ASSERT_EQUAL( it->type, (unsigned int) 1 );

    ++it;
    ASSERT_FALSE( it == end );

    ASSERT_APPROX_EQUAL( it->position.x(), 20 );
    ASSERT_APPROX_EQUAL( it->position.y(), 30 );
    ASSERT_APPROX_EQUAL( it->width, 10 );
    ASSERT_APPROX_EQUAL( it->height, 20 );
    ASSERT_EQUAL( it->type, (unsigned int) 2 );

    ++it;
    ASSERT_FALSE( it == end );

    ASSERT_APPROX_EQUAL( it->position.x(), 30 );
    ASSERT_APPROX_EQUAL( it->position.y(), 40 );
    ASSERT_APPROX_EQUAL( it->width, 100 );
    ASSERT_APPROX_EQUAL( it->height, 200 );
    ASSERT_EQUAL( it->type, (unsigned int) 3 );

    ++it;
    ASSERT_TRUE( it == end );
  }

  SECTION( "Hit Box Configuration - Frame 4" );
  {
    collision.setFrameNumber( 4 );
    Collision::iterator it = collision.begin();
    Collision::iterator end = collision.end();
    ASSERT_FALSE( it == end );

    ASSERT_APPROX_EQUAL( it->position.x(), 3 );
    ASSERT_APPROX_EQUAL( it->position.y(), 4 );
    ASSERT_APPROX_EQUAL( it->width, 10 );
    ASSERT_APPROX_EQUAL( it->height, 20 );
    ASSERT_EQUAL( it->type, (unsigned int) 1 );

    ++it;
    ASSERT_TRUE( it == end );
  }

////////////////////////////////////////////////////////////////////////////////////////////////////

  if ( ! testass::control::summarize() )
  {
    testass::control::printReport( std::cout );
  }

  testass::control::kill();
  Manager::killInstance();
  logtastic::stop();
  return 0;
}

