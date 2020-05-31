
#include "Regolith.h"

#include "testass.h"
#include "logtastic.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Create a test data structure

struct test_base : public Regolith::MassProduceable<>
{
  int variable;
};

struct test_derived1 : public test_base
{
  void configure( Json::Value& ) { variable = 1; }
};

struct test_derived2 : public test_base
{
  void configure( Json::Value& ) { variable = 2; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////


int main( int, char** )
{
  logtastic::init();
  logtastic::setLogFileDirectory( "./test_data/logs/" );
  logtastic::addLogFile( "tests_factory_template.log" );
  logtastic::start( "Regolith - Factory Template Tests", REGOLITH_VERSION_NUMBER );

  testass::control::init( "Regolith", "Factory Template" );
  testass::control::get()->setVerbosity( testass::control::verb_short );

////////////////////////////////////////////////////////////////////////////////////////////////////

  Regolith::FactoryTemplate< test_base > test_factory;

  test_factory.addBuilder< test_derived1 >( "derived1" );
  test_factory.addBuilder< test_derived2 >( "derived2" );

  Json::Value json1;
  json1["type"] = "derived1";

  Json::Value json2;
  json2["type"] = "derived2";

  test_base* test1 = test_factory.build( json1 );
  test_base* test2 = test_factory.build( json2 );

  ASSERT_EQUAL( test1->variable, 1 );
  ASSERT_EQUAL( test2->variable, 2 );

  delete test1;
  delete test2;

////////////////////////////////////////////////////////////////////////////////////////////////////

  if ( ! testass::control::summarize() )
  {
    testass::control::printReport( std::cout );
  }

  testass::control::kill();
  logtastic::stop();
  return 0;
}

