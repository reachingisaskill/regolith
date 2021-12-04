#define TESTASS_APPROX_LIMIT 1.0E-6

#include "Regolith.h"
#include "Regolith/Links/LinkCollisionManager.h"
#include "Regolith/Collisions/SpriteCollision.h"

#include "testass.h"
#include "logtastic.h"

#include <iostream>
#include <string>


using namespace Regolith;


////////////////////////////////////////////////////////////////////////////////
  // Json Data for test 1
const std::string json_test_1 = "{ \"hit_boxes\" : [ [ { \"position\" : [ 10, 20 ], \"width\" : 100, \"height\": 200, \"type\" : \"test0\" } ], \
                                                     [ { \"position\" : [ 10, 20 ], \"width\" : 1,   \"height\": 2,   \"type\" : \"test1\" }, \
                                                       { \"position\" : [ 20, 30 ], \"width\" : 10,  \"height\": 20,  \"type\" : \"test2\" }, \
                                                       { \"position\" : [ 30, 40 ], \"width\" : 100, \"height\": 200, \"type\" : \"test3\" } ], \
                                                     [ { \"shape\" : \"polygon\", \"points\" : [[3 , 4], [10, 7], [9, 15], [3, 12], [1, 7]],  \"type\" : \"test1\" } ] ] }";

const std::string json_test_2 = "{ \"hit_boxes\" : [ [ { \"shape\" : \"polygon\", \"points\" : [[0, 0], [10, 0], [8, 10], [7, 3]], \"type\" : \"test0\" } ] ] }";

////////////////////////////////////////////////////////////////////////////////

int main( int, char** )
{
  // Configure the logger first
  logtastic::init();
  logtastic::setLogFileDirectory( "./test_data/logs/" );
  logtastic::setLogFile( "tests_collision.log" );
  logtastic::setPrintToScreenLimit( logtastic::off );

  // Get link to the manager
  Manager::createInstance();
  Regolith::Link<Regolith::CollisionManager, Regolith::TestType> link_manager = Manager::getInstance()->getCollisionManager< Regolith::TestType >();

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

  SpriteCollision collision;


////////////////////////////////////////////////////////////////////////////////////////////////////

  SECTION( "Frame Configuration" );
  {
    ASSERT_EQUAL( collision.getNumberFrames(), (unsigned int) 0 );

    if ( ! json_reader->parse( json_test_1.c_str(), json_test_1.c_str() + json_test_1.size(), &json_data, &error_string ) )
    {
      ERROR_LOG( "Error occured parsing Json String" );
      ERROR_STREAM << error_string;
      error_string.clear();
    }


    collision.configure( json_data );

    ASSERT_EQUAL( collision.getNumberFrames(), (unsigned int) 3 );
    ASSERT_EQUAL( collision.size(), (unsigned int) 1 );

    collision.setFrameNumber( 1 );
    ASSERT_EQUAL( collision.size(), (unsigned int) 3 );

    collision.setFrameNumber( 2 );
    ASSERT_EQUAL( collision.size(), (unsigned int) 1 );
  }

////////////////////////////////////////////////////////////////////////////////////////////////////

  SECTION( "Hit Box Configuration - Frame 0" );
  {
    collision.setFrameNumber( 0 );

    Collision::iterator it = collision.begin();
    Collision::iterator end = collision.end();

    ASSERT_EQUAL( it->number, (unsigned int)4 );
    ASSERT_APPROX_EQUAL( it->points[0].x(), 10 );
    ASSERT_APPROX_EQUAL( it->points[0].y(), 20 );
    ASSERT_APPROX_EQUAL( it->points[1].x(), 110 );
    ASSERT_APPROX_EQUAL( it->points[1].y(), 20 );
    ASSERT_APPROX_EQUAL( it->points[2].x(), 110 );
    ASSERT_APPROX_EQUAL( it->points[2].y(), 220 );
    ASSERT_APPROX_EQUAL( it->points[3].x(), 10 );
    ASSERT_APPROX_EQUAL( it->points[3].y(), 220 );
    ASSERT_EQUAL( it->collisionType, (unsigned int) 0 );

    ASSERT_FALSE( it == end );
    ++it;
    ASSERT_TRUE( it == end );

  }

  SECTION( "Hit Box Configuration - Frame 1" );
  {
    collision.setFrameNumber( 1 );
    Collision::iterator it = collision.begin();
    Collision::iterator end = collision.end();
    ASSERT_FALSE( it == end );

    ASSERT_EQUAL( it->number, (unsigned int)4 );
    ASSERT_APPROX_EQUAL( it->points[0].x(), 10 );
    ASSERT_APPROX_EQUAL( it->points[0].y(), 20 );
    ASSERT_APPROX_EQUAL( it->points[1].x(), 11 );
    ASSERT_APPROX_EQUAL( it->points[1].y(), 20 );
    ASSERT_APPROX_EQUAL( it->points[2].x(), 11 );
    ASSERT_APPROX_EQUAL( it->points[2].y(), 22 );
    ASSERT_APPROX_EQUAL( it->points[3].x(), 10 );
    ASSERT_APPROX_EQUAL( it->points[3].y(), 22 );
    ASSERT_EQUAL( it->collisionType, (unsigned int) 1 );

    ++it;
    ASSERT_FALSE( it == end );

    ASSERT_EQUAL( it->number, (unsigned int)4 );
    ASSERT_APPROX_EQUAL( it->points[0].x(), 20 );
    ASSERT_APPROX_EQUAL( it->points[0].y(), 30 );
    ASSERT_APPROX_EQUAL( it->points[1].x(), 30 );
    ASSERT_APPROX_EQUAL( it->points[1].y(), 30 );
    ASSERT_APPROX_EQUAL( it->points[2].x(), 30 );
    ASSERT_APPROX_EQUAL( it->points[2].y(), 50 );
    ASSERT_APPROX_EQUAL( it->points[3].x(), 20 );
    ASSERT_APPROX_EQUAL( it->points[3].y(), 50 );
    ASSERT_EQUAL( it->collisionType, (unsigned int) 2 );

    ++it;
    ASSERT_FALSE( it == end );

    ASSERT_EQUAL( it->number, (unsigned int)4 );
    ASSERT_APPROX_EQUAL( it->points[0].x(), 30 );
    ASSERT_APPROX_EQUAL( it->points[0].y(), 40 );
    ASSERT_APPROX_EQUAL( it->points[1].x(), 130 );
    ASSERT_APPROX_EQUAL( it->points[1].y(), 40 );
    ASSERT_APPROX_EQUAL( it->points[2].x(), 130 );
    ASSERT_APPROX_EQUAL( it->points[2].y(), 240 );
    ASSERT_APPROX_EQUAL( it->points[3].x(), 30 );
    ASSERT_APPROX_EQUAL( it->points[3].y(), 240 );
    ASSERT_EQUAL( it->collisionType, (unsigned int) 3 );

    ++it;
    ASSERT_TRUE( it == end );
  }

  SECTION( "Hit Box Configuration - Frame 2" );
  {
    collision.setFrameNumber( 2 );
    Collision::iterator it = collision.begin();
    Collision::iterator end = collision.end();
    ASSERT_FALSE( it == end );

    ASSERT_APPROX_EQUAL( it->points[0].x(), 3 );
    ASSERT_APPROX_EQUAL( it->points[0].y(), 4 );
    ASSERT_APPROX_EQUAL( it->points[1].x(), 10 );
    ASSERT_APPROX_EQUAL( it->points[1].y(), 7 );
    ASSERT_APPROX_EQUAL( it->points[2].x(), 9 );
    ASSERT_APPROX_EQUAL( it->points[2].y(), 15 );
    ASSERT_APPROX_EQUAL( it->points[3].x(), 3 );
    ASSERT_APPROX_EQUAL( it->points[3].y(), 12 );
    ASSERT_APPROX_EQUAL( it->points[4].x(), 1 );
    ASSERT_APPROX_EQUAL( it->points[4].y(), 7 );
    ASSERT_EQUAL( it->collisionType, (unsigned int) 1 );

    ++it;
    ASSERT_TRUE( it == end );
  }

////////////////////////////////////////////////////////////////////////////////////////////////////
  // Concave shape should fail
  SECTION( "Hit Box Concave Test" );
  {

    if ( ! json_reader->parse( json_test_2.c_str(), json_test_2.c_str() + json_test_2.size(), &json_data, &error_string ) )
    {
      ERROR_LOG( "Error occured parsing Json String" );
      ERROR_STREAM << error_string;
      error_string.clear();
    }

    std::string error_string( "" );
    try
    {
      collision.configure( json_data );
    }
    catch ( Exception& ex )
    {
      error_string = ex.what();
    }
    ASSERT( error_string == std::string("Polygon is not convex. Vertecies must be provided in a clockwise order for a convex shape only. in SpriteCollision::configure()") );
    
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

