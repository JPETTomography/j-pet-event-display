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
 *  @file EventDisplay.cpp
 */

#include "EventDisplay.h"

#include <TError.h>
#include <TROOT.h>
#include <TEnv.h>
#include <TCanvas.h>
#include <JPetLoggerInclude.h>
#include "GeometryVisualizator.h"
#include "DataProcessor.h"


namespace jpet_event_display
{
EventDisplay::EventDisplay(const std::string& inFile, const std::string& inFileType, std::unique_ptr<TRint> theApp, 
                           const std::string& geomFile):
  fApplication(std::move(theApp))
{
  DATE_AND_TIME();
  INFO("J-PET Event Display created");
  INFO("*********************");
  run(inFile, geomFile);
}

void EventDisplay::run(const std::string& inFile, const std::string& geomFile)
{
  auto canvas = std::make_shared<TCanvas>("MyCanvas", "Test Canvas", 10, 10, 900, 500);
  GeometryVisualizator visualizator(canvas);
  visualizator.loadGeometry(geomFile);
  visualizator.drawOnlyGeometry();
  /// Just a temporary solution to show the idea. We will change it further.
  /// We read only second event here. 
  std::map<int, std::vector<int> > selection;
  DataProcessor myProcessor;
  if(myProcessor.openFile(inFile.c_str())) {
    if(!myProcessor.nextEvent()) {
      ERROR("Next event is not read correctly!");
    } else {
      selection = DataProcessor::getActiveScintillators(myProcessor.getCurrentEvent());
    }
  }
  //std::vector<int> channels;
  //channels.push_back(10);
  //channels.push_back(11);
  //channels.push_back(12);
  //channels.push_back(13);
  //selection[0] = channels;
  visualizator.drawStrips(selection);
  canvas->Draw();
  gEnv = new TEnv(".rootrc");
  fApplication->Run();
}
}
