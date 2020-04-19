#define TESTASS_APPROX_LIMIT 1.0E-6

#include "regolith.h"

#include "testass.h"
#include "logtastic.h"

#include <iostream>


using namespace Regolith;

int main( int, char** )
{
  logtastic::init();
  logtastic::setLogFileDirectory( "./test_data/logs/" );
  logtastic::addLogFile( "tests_vector.log" );
  logtastic::start( "Regolith - Vector Tests", REGOLITH_VERSION_NUMBER );

  testass::control::init( "Regolith", "Vectors" );
  testass::control::get()->setVerbosity( testass::control::verb_short );

////////////////////////////////////////////////////////////////////////////////////////////////////

  SECTION( "Basics" );
  {
    Vector vec1( 0.0f, 0.0f );
    ASSERT_EQUAL( vec1.x(), 0.0f );
    ASSERT_EQUAL( vec1.y(), 0.0f );

    Vector vec2( 1.7f, 23.45665f );
    ASSERT_EQUAL( vec2.x(), 1.7f );
    ASSERT_EQUAL( vec2.y(), 23.45665f );

    Vector vec3( 3.6f );
    ASSERT_EQUAL( vec3.x(), 3.6f );
    ASSERT_EQUAL( vec3.y(), 3.6f );

    Vector vec4( 2.0f );
    ASSERT_APPROX_EQUAL( vec4.square(), 2.0*2.0 + 2.0*2.0 );

    Vector vec5( 3.0, 4.0 );
    ASSERT_APPROX_EQUAL( vec5.mod(), 5.0 );
    
    Vector vec6 = vec5.norm();
    ASSERT_APPROX_EQUAL( vec6.x(), 3.0/5.0 );
    ASSERT_APPROX_EQUAL( vec6.y(), 4.0/5.0 );
  }

////////////////////////////////////////////////////////////////////////////////////////////////////

  SECTION( "Mathematical Operations" );
  {
    Vector vec1( 1.0, 2.0 );
    Vector vec2( 5.0, 6.0 );
    Vector vec3 = vec1 + vec2;
    Vector vec4 = vec2 - vec1;
    Vector vec5 = vec2 * 3.0;
    Vector vec6 = 1.5 * vec2;
    float float1 = vec2 * vec1;
    float float2 = unitVector_x * unitVector_x;
    float float3 = unitVector_x * unitVector_y;
    float float4 = unitVector_x ^ unitVector_y;
    float float5 = unitVector_x ^ unitVector_x;
    float float6 = unitVector_y ^ unitVector_x;

    ASSERT_APPROX_EQUAL( vec3.x(), 6.0 );
    ASSERT_APPROX_EQUAL( vec3.y(), 8.0 );

    ASSERT_APPROX_EQUAL( vec4.x(), 4.0 );
    ASSERT_APPROX_EQUAL( vec4.y(), 4.0 );

    ASSERT_APPROX_EQUAL( vec5.x(), 15.0 );
    ASSERT_APPROX_EQUAL( vec5.y(), 18.0 );

    ASSERT_APPROX_EQUAL( vec6.x(), 5.0*1.5 );
    ASSERT_APPROX_EQUAL( vec6.y(), 6.0*1.5 );

    ASSERT_APPROX_EQUAL( float1, 17.0 );
    ASSERT_APPROX_EQUAL( float2, 1.0 );
    ASSERT_APPROX_EQUAL( float3, 0.0 );
    ASSERT_APPROX_EQUAL( float4, 1.0 );
    ASSERT_APPROX_EQUAL( float5, 0.0 );
    ASSERT_APPROX_EQUAL( float6, -1.0 );

    vec1 += vec2;
    ASSERT_APPROX_EQUAL( vec1.x(), 6.0 );
    ASSERT_APPROX_EQUAL( vec1.y(), 8.0 );

    vec1 -= vec2;
    ASSERT_APPROX_EQUAL( vec1.x(), 1.0 );
    ASSERT_APPROX_EQUAL( vec1.y(), 2.0 );

    vec1 *= 2.5;
    ASSERT_APPROX_EQUAL( vec1.x(), 2.5*1.0 );
    ASSERT_APPROX_EQUAL( vec1.y(), 2.5*2.0 );

    vec1 /= 5.0;
    ASSERT_APPROX_EQUAL( vec1.x(), 2.5*1.0/5.0 );
    ASSERT_APPROX_EQUAL( vec1.y(), 2.5*2.0/5.0 );
  }

////////////////////////////////////////////////////////////////////////////////////////////////////

  SECTION( "Comparison Operators" );
  {
    Vector vec1( 1.0, 2.0 );
    Vector vec2( 5.0, 6.0 );
    Vector vec3( 1.0, 2.0 );
    Vector vec4 = 0.99999 * vec1;

    ASSERT_TRUE( vec1 == vec3 );
    ASSERT_FALSE( vec1 == vec2 );
    ASSERT_TRUE( vec1 != vec2 );
    ASSERT_TRUE( vec2 > vec1 );
    ASSERT_TRUE( vec3 >= vec1 );
    ASSERT_FALSE( vec3 > vec1 );
    ASSERT_TRUE( vec4 < vec1 );
    ASSERT_TRUE( vec4 <= vec1 );
    ASSERT_TRUE( vec1 <= vec1 );
    ASSERT_FALSE( vec1 < vec1 );
  }

////////////////////////////////////////////////////////////////////////////////////////////////////

  SECTION( "Rotations" );
  {
    ASSERT_EQUAL( unitVector_x.getRotated( radians( 90.0 ) ), unitVector_y );
    ASSERT_EQUAL( unitVector_y.getRotated( radians( 90.0 ) ), -unitVector_x );

    Vector vec1( 1.0, 1.0 );
    Vector vec2( 0.0, std::sqrt( 2 ) );
    Vector vec3( std::sqrt( 2 ), 0.0 );

    ASSERT_EQUAL( vec1.getRotated( pi/4 ), vec2 );
    ASSERT_EQUAL( vec1.getRotated( -pi/4 ), vec3 );

    Vector vec4( 0.0, 2.0 );
    Vector vec5( std::cos(pi/4), 1.0-std::sin(pi/4) );
    ASSERT_EQUAL( vec1.getRotatedAbout( pi/2, unitVector_y ), vec4 );
    ASSERT_EQUAL( vec1.getRotatedAbout( -pi/4, unitVector_y ), vec5 );
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

