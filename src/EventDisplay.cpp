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
EventDisplay::EventDisplay() { }

EventDisplay::EventDisplay(const std::string& inFile, const std::string& inFileType, 
                           const std::string& geomFile)
{
  fGUIControls->eventNo = 0;
  fGUIControls->stepNo = 0;
  fGUIControls->displayFullSig = 0;
  run(inFile, geomFile);
  fApplication->Run();
  DATE_AND_TIME();
  INFO("J-PET Event Display created");
  INFO("*********************");
}

EventDisplay::~EventDisplay() { }

void EventDisplay::run(const std::string& inFile, const std::string& geomFile)
{
  fMainWindow = std::unique_ptr<TGMainFrame>(new TGMainFrame(gClient->GetRoot()));
  fMainWindow->SetCleanup(kDeepCleanup);
  fMainWindow->Connect("CloseWindow()", "jpet_event_display::EventDisplay", this, "CloseWindow()");
  const Int_t w_max = 1024;
  const Int_t h_max = 720;
  fMainWindow->SetWMSize(w_max,h_max);  //this is the only way to set the size of the main frame
  fMainWindow->SetLayoutBroken(kTRUE);
  assert(fMainWindow != 0);

  

  fMainWindow->SetWindowName("Single Strip Event Display ver 0.1");
  fMainWindow->MapSubwindows();
  fMainWindow->Resize(fMainWindow->GetDefaultSize());
  fMainWindow->MapWindow();
}

void EventDisplay::CloseWindow() {
  gApplication->Terminate();
}
}
