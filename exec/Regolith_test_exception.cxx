
#include "Regolith.h"
#include "Regolith/Utilities/Exception.h"

#include "testass.h"
#include "logtastic.h"

#include <iostream>


using namespace Regolith;


void testFunc1();
void testFunc2();
void testFunc3();

int main( int, char** )
{
  logtastic::init();
  logtastic::setLogFileDirectory( "./test_data/logs/" );
  logtastic::addLogFile( "tests_exception.log" );
  logtastic::setPrintToScreenLimit( logtastic::off );
  logtastic::start( "Regolith - Exception Tests", REGOLITH_VERSION_NUMBER );

  testass::control::init( "Regolith", "Exceptions" );
  testass::control::get()->setVerbosity( testass::control::verb_short );

////////////////////////////////////////////////////////////////////////////////////////////////////

  SECTION( "Basics" );
  {
    Exception ex( "TestFunc()", "Test Error" );
    std::string what = ex.what();

    ASSERT_EQUAL( what, "Test Error in TestFunc()" );

    ASSERT_TRUE( ex.isRecoverable() );

    ASSERT_EQUAL( ex.numberDetails(), (size_t)0 );

    double var1 = 10.0;
    bool var2 = true;
    long int var3 = 1;
    const char* var4 = "hello";
    ex.addDetail< double >( "var1", var1 );
    ex.addDetail< bool >( "var2", var2 );
    ex.addDetail< long int >( "var3", var3 );
    ex.addDetail< const char* >( "var4", var4 );

    ASSERT_EQUAL( ex.numberDetails(), (size_t)4 );

    std::string expected( "Recoverable exception occured: Test Error in TestFunc()\n   + var1 = 10\n   + var2 = 1\n   + var3 = 1\n   + var4 = hello\n" );

    ASSERT_EQUAL( ex.elucidate(), expected );
  }

////////////////////////////////////////////////////////////////////////////////////////////////////

  SECTION( "Throwing and catching" );
  {
    std::string expected( "Non-recoverable exception occured: Test Error in TestFunc()\n   + var1 = 10\n   + var2 = 1\n   + var3 = 1\n   + var4 = hello\n" );

    try
    {
      testFunc1();
    }
    catch( Exception& ex )
    {
      ex.addDetail( "var4", "hello" );
      ASSERT_EQUAL( ex.numberDetails(), (size_t)4 );
      ASSERT_EQUAL( ex.elucidate(), expected );
      ASSERT_FALSE( ex.isRecoverable() );
    }
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



void testFunc1()
{
  try
  {
    testFunc2();
  }
  catch ( Exception& ex )
  {
    ex.addDetail( "var2", true );
    ex.addDetail( "var3", 1l );
    throw ex;
  }
}

void testFunc2()
{
  try
  {
    testFunc3();
  }
  catch ( Exception& ex )
  {
    ex.addDetail( "var1", 10.0 );
    throw ex;
  }
}

void testFunc3()
{
  throw Exception( "TestFunc()", "Test Error", false );
}

