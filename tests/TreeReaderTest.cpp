#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TreeReaderTest
#include <cstddef>
#include <iostream>
#include <vector>
#include <boost/test/unit_test.hpp>
#include <TError.h> /// gErrorIgnoreLevel
#include <TObjString.h>

#include "../src/TreeReader.h"


BOOST_AUTO_TEST_SUITE (TreeReaderTestSuite)


BOOST_AUTO_TEST_CASE (default_constructor)
{
  TreeReader reader;
  BOOST_REQUIRE(std::string(reader.getCurrentEvent().GetName())==std::string("Empty event"));
  BOOST_REQUIRE(!reader.isOpen());
  BOOST_REQUIRE(!reader.nextEvent());
  BOOST_REQUIRE(!reader.firstEvent());
  BOOST_REQUIRE(!reader.lastEvent());
  BOOST_REQUIRE(!reader.nthEvent(0));
  BOOST_REQUIRE(!reader.nthEvent(1));
  BOOST_REQUIRE(!reader.nthEvent(-1));
  BOOST_REQUIRE_EQUAL(reader.getCurrentEventNumber(), -1);
  BOOST_REQUIRE_EQUAL(reader.getNbOfAllEvents(), 0);
}

BOOST_AUTO_TEST_CASE (bad_file)
{
  gErrorIgnoreLevel = 6000; /// we turn off the ROOT error messages
  TreeReader reader;
  /// not a ROOT file
  BOOST_REQUIRE(!reader.openFileAndLoadData("testData/bad_file.txt", "tree"));
  BOOST_REQUIRE(!reader.isOpen());
  BOOST_REQUIRE(std::string(reader.getCurrentEvent().GetName())==std::string("Empty event"));
  BOOST_REQUIRE(!reader.nextEvent());
  BOOST_REQUIRE(!reader.firstEvent());
  BOOST_REQUIRE(!reader.lastEvent());
  BOOST_REQUIRE(!reader.nthEvent(0));
  BOOST_REQUIRE(!reader.nthEvent(1));
  BOOST_REQUIRE(!reader.nthEvent(-1));
  BOOST_REQUIRE_EQUAL(reader.getCurrentEventNumber(), -1);
  BOOST_REQUIRE_EQUAL(reader.getNbOfAllEvents(), 0);
}

BOOST_AUTO_TEST_CASE (good_file_with_constructor)
{
  TreeReader reader("testData/timewindows.root");
  BOOST_REQUIRE(reader.isOpen());
  BOOST_REQUIRE(std::string(reader.getCurrentEvent().GetName())==std::string("JPetTimeWindow"));
  BOOST_REQUIRE(reader.nextEvent());
  BOOST_REQUIRE(reader.firstEvent());
  BOOST_REQUIRE(reader.lastEvent());
  BOOST_REQUIRE(reader.nthEvent(0));
  BOOST_REQUIRE(reader.nthEvent(5));
  BOOST_REQUIRE_EQUAL(reader.getCurrentEventNumber(), 5);
  BOOST_REQUIRE_EQUAL(reader.getNbOfAllEvents(), 10);
}
BOOST_AUTO_TEST_CASE (good_file_getObject)
{
  TreeReader reader("testData/timewindows.root");
  BOOST_REQUIRE(!reader.getObjectFromFile("nonExistentObj"));
  BOOST_REQUIRE(reader.getObjectFromFile("tree"));
}

BOOST_AUTO_TEST_CASE (good_file_openFileAndLoadData)
{
  TreeReader reader;
  BOOST_REQUIRE(reader.openFileAndLoadData("testData/timewindows.root","tree"));
  BOOST_REQUIRE(reader.isOpen());
  BOOST_REQUIRE(std::string(reader.getCurrentEvent().GetName())==std::string("JPetTimeWindow"));
  BOOST_REQUIRE(reader.firstEvent());
  BOOST_REQUIRE(reader.nextEvent());
  BOOST_REQUIRE(reader.lastEvent());
  BOOST_REQUIRE(reader.nthEvent(0));
  BOOST_REQUIRE(reader.nthEvent(5));
  BOOST_REQUIRE_EQUAL(reader.getCurrentEventNumber(), 5);
  BOOST_REQUIRE_EQUAL(reader.getNbOfAllEvents(), 10);
}

BOOST_AUTO_TEST_CASE (good_file_closeFile)
{
  TreeReader reader;
  BOOST_REQUIRE(reader.openFileAndLoadData("testData/timewindows.root","tree"));
  BOOST_REQUIRE(reader.isOpen());
  reader.closeFile();
  BOOST_REQUIRE(!reader.isOpen());
  BOOST_REQUIRE(std::string(reader.getCurrentEvent().GetName())==std::string("Empty event"));
  BOOST_REQUIRE(!reader.nextEvent());
  BOOST_REQUIRE(!reader.firstEvent());
  BOOST_REQUIRE(!reader.lastEvent());
  BOOST_REQUIRE(!reader.nthEvent(0));
  BOOST_REQUIRE(!reader.nthEvent(1));
  BOOST_REQUIRE(!reader.nthEvent(-1));
  BOOST_REQUIRE_EQUAL(reader.getCurrentEventNumber(), -1);
  BOOST_REQUIRE_EQUAL(reader.getNbOfAllEvents(), 0);
}

BOOST_AUTO_TEST_SUITE_END()
