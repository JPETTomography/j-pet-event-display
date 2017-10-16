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

#include "src/EventDisplay.h"
#include <JPetGeomMapping/JPetGeomMapping.h>
#include <JPetParamManager/JPetParamManager.h>
#include <TRint.h>
#include <boost/program_options.hpp>
#include <iostream>

int main(int argc, char** argv)
{
  using namespace jpet_event_display;
  namespace po = boost::program_options;

  std::string inFile = "large_barrel.json";
  int runNumber = 0;

  try {
    po::options_description desc("Allowed options");
    desc.add_options()("help,h", "produce help message")(
      "input,i", po::value(&inFile), "Input file")(
        "run,r", po::value(&runNumber), "run number of input file");
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

  std::cout << "Generating GeomMapping using " << inFile
            << ", run number: " << runNumber
            << ", please wait, application will start soon... "
            << "\n";

  JPetParamManager fparamManagerInstance(new JPetParamGetterAscii(inFile));
  fparamManagerInstance.fillParameterBank(runNumber);
  auto bank = fparamManagerInstance.getParamBank();

  std::shared_ptr< JPetGeomMapping > fMapper =
    std::unique_ptr< JPetGeomMapping >(new JPetGeomMapping(bank));
  std::vector< size_t > layersSize = fMapper->getLayersSizes();
  std::vector< std::pair< int, double > > layersInfo;
  const int numberOfLayers =
    layersSize.size() - 1; // -1 because last layer is reference
  for (int i = 0; i < numberOfLayers; i++) {
    layersInfo.push_back(
      std::make_pair(layersSize[i], fMapper->getRadiusOfLayer(i + 1)));
  }
  EventDisplay myDisplay;
  myDisplay.run(fMapper, numberOfLayers, 50, layersInfo);
  return 0;
}
