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
 *  @file GuiController.cpp
 */


#include "GuiController.h"
#include <TApplication.h>
#include <TGClient.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TGFrame.h>
#include <TRandom3.h>
#include <TRootEmbeddedCanvas.h>
#include <TString.h>
#include <TGMenu.h>
#include <TGNumberEntry.h>
#include <TGFileBrowser.h>
#include <TGFileDialog.h>
#include <TGButtonGroup.h>

#include <cassert>

#include <TGComboBox.h>
#include <TGTab.h>

using namespace std;

namespace jpet_event_display
{
  GuiController::GuiController() 
  {
    fApplication = std::unique_ptr<TRint>(new TRint("App", 0, 0));
    createGraphicalElements();
  }
  
  GuiController::~GuiController() { }

  void GuiController::createGraphicalElements()
  {
    fMainWindow = new TGMainFrame(gClient->GetRoot());
    fMainWindow->SetCleanup(kDeepCleanup);
    fMainWindow->Connect("CloseWindow()", "GuiController", this, "CloseWindow()");
    const Int_t w_max = 1024;
    const Int_t h_max = 720;
    fMainWindow->SetWMSize(w_max,h_max);  //this is the only way to set the size of the main frame
    fMainWindow->SetLayoutBroken(kTRUE);
    assert(fMainWindow != 0);
    const Int_t w_GlobalFrame = w_max;
    const Int_t h_GlobalFrame = h_max;
    const Int_t w_Frame1 = 180;
    const Int_t h_Frame1 = h_max-12;  // cause 2*5 margins from GlobalFrame and 2*1 margins from Frame1
    const Int_t w_Frame2 = w_max-w_Frame1;
    const Int_t h_Frame2 = h_max-12;
    int fMaxEvents = 1000; // temporary
    
  // adding baseFrame
    
    TGCompositeFrame *baseFrame = new TGCompositeFrame(fMainWindow,w_GlobalFrame,h_GlobalFrame-10,kVerticalFrame);
    fMainWindow->AddFrame(baseFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,0,0,0,0));

    ULong_t ucolor = 0;     
    gClient->GetColorByName("#e6e6e6",ucolor);
    baseFrame->ChangeBackground(ucolor);

    // adding fmenuBar

    fMenuBar = new TGMenuBar(baseFrame, 1, 1, kHorizontalFrame);
    fMenuBar->ChangeBackground(ucolor);

    TGLayoutHints *fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsRight, 0, 0, 0, 0);
    TGLayoutHints *fMenuBarLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop, 0, 0, 0, 0);

    fMenuFile = new TGPopupMenu(gClient->GetRoot());
    fMenuFile->AddEntry(" &Open...\tCtrl+O", M_FILE_OPEN);
    fMenuFile->AddSeparator();
    fMenuFile->AddEntry(" E&xit\tCtrl+Q", M_FILE_EXIT);
    fMenuFile->Associate(fMainWindow);
    fMenuFile->Connect("Activated(Int_t)", "GuiSignalController", guiSignalController, "handleMenu(Int_t)");
    
    fMenuFile->DisableEntry(0);
    
    fMenuBar->AddPopup("&File", fMenuFile, fMenuBarItemLayout);
    
    baseFrame->AddFrame(fMenuBar, fMenuBarLayout);
    
  // adding globalFrame

    TGCompositeFrame *globalFrame = new TGCompositeFrame(baseFrame,w_GlobalFrame,h_GlobalFrame-10,kHorizontalFrame);
    globalFrame->ChangeBackground(ucolor);
    baseFrame->AddFrame(globalFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,0,0,0,0));

  // adding embedded canvas

    fEcanvas = new TRootEmbeddedCanvas("Ecanvas",globalFrame,600,600);
    TCanvas* canv=fEcanvas->GetCanvas();
    canv->Divide(1,2);
    canv->cd(1)->SetPad(0.0,0.9,1.0,1.0); 
    canv->cd(2)->SetPad(0.0,0.0,1.0,0.9);
    canv->cd(2)->Divide(2,2);
    globalFrame->AddFrame(fEcanvas, new TGLayoutHints(kLHintsExpandX| kLHintsExpandY,10,10,10,1));

  // finalizing layout

    globalFrame->Resize(globalFrame->GetDefaultSize());
    baseFrame->Resize(baseFrame->GetDefaultSize());

    fMainWindow->SetWindowName("Single Strip Event Display ver 0.1");
    fMainWindow->MapSubwindows();
    fMainWindow->Resize(fMainWindow->GetDefaultSize());
    fMainWindow->MapWindow();
  }

  void GuiController::draw() { fCanvas->Draw(); }
  void GuiController::run() { fApplication->Run(); }

}