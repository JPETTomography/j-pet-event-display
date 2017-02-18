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
EventDisplay::EventDisplay(const std::string& inFile, const std::string& inFileType, 
                           const std::string& geomFile)
{
  DATE_AND_TIME();
  INFO("J-PET Event Display created");
  INFO("*********************");
  guiSignalController = new GuiSignalController();
  guiController = new GuiController(guiSignalController);
  guiSignalController->Connect("updateGUIControlls()", "jpet_event_display::GuiController", guiController, "updateGUIControlls()");
  guiSignalController->Connect("openFileDialog(TGFileInfo*, EFileDialogMode)", "jpet_event_display::GuiController", guiController, "openFileDialog(TGFileInfo*, EFileDialogMode)");
  guiController->loadGeometry(geomFile);
  guiController->openFile(inFile);
  guiController->run();
  //run(inFile, geomFile);
}

EventDisplay::~EventDisplay() { }

void EventDisplay::run(const std::string& inFile, const std::string& geomFile)
{
  /*gui.loadGeometry(geomFile);
  gui.openFile(inFile);
  std::map<int, std::vector<int> > selection;
  DataProcessor myProcessor;
  if(myProcessor.openFile(inFile.c_str())) {
    myProcessor.getParamBank();
    while(myProcessor.nextEvent()) {
    //  if()
    //  ERROR("Next event is not read correctly!");
    //} else {
      std::map<int, std::vector<int> > sel = DataProcessor::getActiveScintillators(myProcessor.getCurrentEvent());
      selection.insert(sel.begin(), sel.end());
    }
  }
  //visualizator.drawStrips(selection);
  
  gEnv = new TEnv(".rootrc");
  //gui.draw();
  gui.run();
  */
}
}
