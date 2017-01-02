#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE DataProcessorTest
#include <boost/test/unit_test.hpp>

#include "../src/DataProcessor.h"

using namespace jpet_event_display;

BOOST_AUTO_TEST_SUITE(FirstSuite)

BOOST_AUTO_TEST_CASE( DataProcessorTest )
{ 
  BOOST_REQUIRE(1==0); 
}

BOOST_AUTO_TEST_SUITE_END()