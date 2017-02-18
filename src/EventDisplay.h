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
 *  @file EventDisplay.h
 */

#ifndef EVENTDISPLAY_H
#define EVENTDISPLAY_H

#include <memory>
#include <string>

#include <TRint.h>

#include <RQ_OBJECT.h>

#include "GuiSignalController.h"

namespace jpet_event_display
{

struct GUIControlls
{

  Int_t eventNo;
  Int_t stepNo;
  Int_t rootEntries;
  Int_t *rootMatrices;
  bool displayFullSig;
};

class EventDisplay
{
  RQ_OBJECT("EventDisplay")
public:
  EventDisplay(const std::string &inFile, const std::string &inFileType,
               const std::string &geomFile = "JPET_geom.root");
  EventDisplay();
  ~EventDisplay();

#ifndef __CINT__
  void run(const std::string &inFile, const std::string &geomFile);
  enum FileType
  {
    TimeWindow
  };
#endif

  //signals
  void CloseWindow();
  
private:
#ifndef __CINT__
  EventDisplay(const EventDisplay &) = delete;
  EventDisplay &operator=(const EventDisplay &) = delete;

  std::unique_ptr<TRint> fApplication = std::unique_ptr<TRint>(new TRint("EventDisplay Gui", 0, 0));
  std::unique_ptr<GUIControlls> fGUIControls = std::unique_ptr<GUIControlls>(new GUIControlls);
  std::unique_ptr<TGMainFrame> fMainWindow;
#endif
};
}
#endif /*  !EVENTDISPLAY_H */
