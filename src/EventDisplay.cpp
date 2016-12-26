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
#include <JPetReader/JPetReader.h>

namespace jpet_event_display
{
EventDisplay::EventDisplay(const std::string& inFile, const std::string& inFileType, std::unique_ptr<TRint> theApp, const std::string& geomFile):
  fApplication(std::move(theApp))
{
}

void EventDisplay::run()
{
  //gErrorIgnoreLevel = kFatal; /// switch off ROOT warnings and errors
  DATE_AND_TIME();
  INFO("J-PET Event Display started");
  INFO("*********************");
  auto canvas = std::make_shared<TCanvas>("MyCanvas", "Test Canvas", 10, 10, 900, 500);
  GeometryVisualizator visualizator(canvas);
  visualizator.loadGeometry("JPET_geom.root");
  visualizator.drawOnlyGeometry();
  std::map<int, std::vector<int> > selection;
  std::vector<int> channels;
  channels.push_back(10);
  channels.push_back(11);
  channels.push_back(12);
  channels.push_back(13);
  selection[0] = channels;
  visualizator.drawStrips(selection);
  canvas->Draw();
  gEnv = new TEnv(".rootrc");
  fApplication->Run();
}
}
