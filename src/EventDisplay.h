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

#include <TGLabel.h>
#include <TGNumberEntry.h>
#include <TGMenu.h>
#include <TGToolBar.h>
#include <TGFileDialog.h>
#include <TGStatusBar.h>
#include <TGProgressBar.h>
#include <TGButtonGroup.h>
#include <TGTab.h>

#include <TGraph.h>
#include <TBox.h>
#include <TMarker.h>
#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h>

#include <RQ_OBJECT.h>

//#include "GuiSignalController.h"

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

enum EMessageTypes {
 M_FILE_OPEN,
 M_FILE_SAVE,
 M_FILE_EXIT
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
    TimeWindow,
    FileType2,
    FileType3
  };
#endif

  //signals
  void CloseWindow();
  void handleMenu (Int_t id);
  void setFiletype(enum FileType type);
  void updateGUIControlls();
  void doNext();
  
private:
#ifndef __CINT__
  EventDisplay(const EventDisplay &) = delete;
  EventDisplay &operator=(const EventDisplay &) = delete;

  std::unique_ptr<TRint> fApplication = std::unique_ptr<TRint>(new TRint("EventDisplay Gui", 0, 0));
  std::unique_ptr<GUIControlls> fGUIControls = std::unique_ptr<GUIControlls>(new GUIControlls);
  std::unique_ptr<TGMainFrame> fMainWindow;
  std::unique_ptr<TGNumberEntry> fNumberEntryStep;
  std::unique_ptr<TGNumberEntry> fNumberEntryEventNo;

  std::unique_ptr<TGFileInfo> fFileInfo = std::unique_ptr<TGFileInfo>(new TGFileInfo);
#endif
  
};
}
#endif /*  !EVENTDISPLAY_H */
