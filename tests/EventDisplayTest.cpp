#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE EventDisplayTest
#include <boost/test/unit_test.hpp>

#include "../src/EventDisplay.h"

BOOST_AUTO_TEST_SUITE(FirstSuite)

BOOST_AUTO_TEST_CASE( EventDisplayTests )
{ 
  BOOST_REQUIRE(1==0); 
}

BOOST_AUTO_TEST_SUITE_END()