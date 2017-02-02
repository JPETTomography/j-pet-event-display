/**
 *  @copyright Copyright 2016 The J-PET Framework Authors. All rights reserved.
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may find a copy of the License in the LICENCE file.
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#include <boost/program_options.hpp> /// for command line option parsing
#include <TRint.h>
#include <iostream>
#include "src/EventDisplay.h"


int main(int argc, char** argv)
{
  using namespace jpet_event_display;
  namespace po = boost::program_options;

  std::string inFile = "";
  std::string fileType = "JPetTimeWindow";

  try {
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help,h", "produce help message")
    ("input,i", po::value(&inFile), "Input file")
    ("type,t", po::value(&fileType), "type input file type"); /// maybe it is not necessary
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    if (vm.count("help")) {
      std::cout << desc << "\n";
      return 1;
    }
  } catch (std::exception& e) {
    std::cout << e.what() << "\n";
    return 1;
  }
  EventDisplay myDisplay(inFile, fileType, "JPET_geom.root");
  return 0;
}
