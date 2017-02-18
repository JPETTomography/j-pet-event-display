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

#include <string>

using namespace std;

namespace jpet_event_display
{
  GuiController::GuiController(GuiSignalController* guiSigController) 
  {
    fGUIControls = new GUIControlls;
    fGUIControls->eventNo = 0;
    fGUIControls->stepNo = 0;
    fGUIControls->displayFullSig = 0;
    fApplication = std::unique_ptr<TRint>(new TRint("App", 0, 0));
    createGraphicalElements();
    TCanvas *fCanvas = getfEcanvas()->GetCanvas();
    visualizator = new GeometryVisualizator(fCanvas);
    guiSignalController = guiSigController;
  }

  GuiController::~GuiController() { }

  void GuiController::loadGeometry(const std::string& geometry)
  {
    assert(visualizator);
    visualizator->loadGeometry(geometry);
    visualizator->drawOnlyGeometry();
  }

  void GuiController::openFile(const std::string& readFile)
  {
    dataProcessor.openFile(readFile.c_str());
    dataProcessor.getParamBank();
  }

  void GuiController::createGraphicalElements()
  {
    fMainWindow = new TGMainFrame(gClient->GetRoot());
    fMainWindow->SetCleanup(kDeepCleanup);
    fMainWindow->Connect("CloseWindow()", "jpet_event_display::GuiController", this, "CloseWindow()");
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
    fMenuFile->Connect("Activated(Int_t)", "jpet_event_display::GuiSignalController", guiSignalController, "handleMenu(Int_t)");
    
    fMenuFile->DisableEntry(0);
    
    fMenuBar->AddPopup("&File", fMenuFile, fMenuBarItemLayout);
    
    baseFrame->AddFrame(fMenuBar, fMenuBarLayout);
    
  // adding globalFrame

    TGCompositeFrame *globalFrame = new TGCompositeFrame(baseFrame,w_GlobalFrame,h_GlobalFrame-10,kHorizontalFrame);
    globalFrame->ChangeBackground(ucolor);
    baseFrame->AddFrame(globalFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,0,0,0,0));

    // adding frame1

    TGGroupFrame *frame1 = new TGGroupFrame(globalFrame,"Options",kVerticalFrame,TGGroupFrame::GetDefaultGC()(),TGGroupFrame::GetDefaultFontStruct(),ucolor);
    frame1->Resize(w_Frame1,h_Frame1);
    globalFrame->AddFrame(frame1, new TGLayoutHints(kLHintsLeft| kLHintsExpandY,1,1,1,1));

  // adding frame2

    TGGroupFrame *frame2 = new TGGroupFrame(globalFrame,"Display",kVerticalFrame,TGGroupFrame::GetDefaultGC()(),TGGroupFrame::GetDefaultFontStruct(),ucolor);
    frame2->Resize(w_Frame2,h_Frame2);
    globalFrame->AddFrame(frame2, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,1,1,1,1));

  // adding embedded canvas

    fEcanvas = new TRootEmbeddedCanvas("Ecanvas",frame2,600,600);
    TCanvas* canv=fEcanvas->GetCanvas();
    canv->Divide(1,2);
    canv->cd(1)->SetPad(0.0,0.9,1.0,1.0); 
    canv->cd(2)->SetPad(0.0,0.0,1.0,0.9);
    canv->cd(2)->Divide(2,2);
    frame2->AddFrame(fEcanvas, new TGLayoutHints(kLHintsExpandX| kLHintsExpandY,10,10,10,1));

    // adding Frame1_1

    TGCompositeFrame *frame1_1 = new TGCompositeFrame(frame1,1,1,kVerticalFrame);
    frame1_1->ChangeBackground(ucolor);
    //frame1_1->SetLayoutBroken(kTRUE);
    frame1->AddFrame(frame1_1, new TGLayoutHints(kLHintsExpandX |kLHintsTop,1,1,1,1));

    // adding Frame1_1_1

    TGCompositeFrame *frame1_1_1 = new TGCompositeFrame(frame1_1,1,1,kHorizontalFrame);
    frame1_1_1->ChangeBackground(ucolor);
    frame1_1->AddFrame(frame1_1_1, new TGLayoutHints(kLHintsExpandX | kLHintsTop,1,1,0,0));
    
    TGButtonGroup * chooseFiletype = new TGButtonGroup(frame1_1_1,"File type",kHorizontalFrame);
    TGRadioButton * chooseROOT = new TGRadioButton(chooseFiletype, new TGHotString("&ROOT"),0);
    TGRadioButton * chooseScope = new TGRadioButton(chooseFiletype ,new TGHotString("&Scope"),1);
    TGRadioButton * chooseSim = new TGRadioButton(chooseFiletype ,new TGHotString("Simulation"),2);
    chooseFiletype->ChangeBackground(ucolor);
    chooseROOT->ChangeBackground(ucolor);
    chooseSim->ChangeBackground(ucolor);
    chooseScope->ChangeBackground(ucolor);
    chooseFiletype->Show();
    frame1_1_1->AddFrame(chooseFiletype,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,5,5,3,4));
    chooseFiletype->Connect("Pressed(Int_t)", "jpet_event_display::GuiSignalController", 
                            guiSignalController, "setFiletype(enum FileType)");
    chooseScope->SetState(kButtonDown,kFALSE);
    chooseSim->SetState(kButtonDown,kFALSE);
    chooseROOT->SetState(kButtonDown,kTRUE);

    // adding Frame1_1_2

    TGCompositeFrame *frame1_1_2 = new TGCompositeFrame(frame1_1,1,1,kHorizontalFrame);
    frame1_1_2->ChangeBackground(ucolor);
    frame1_1->AddFrame(frame1_1_2, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,2,2,2,2));

    TGTextButton *readButton = new TGTextButton(frame1_1_2,"Read Data");
    frame1_1_2->AddFrame(readButton,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,5,5,3,4));
    readButton->Connect("Clicked()", "jpet_event_display::GuiSignalController", guiSignalController, "handleMenu(=0)");
    readButton->SetTextJustify(36);
    readButton->ChangeBackground(ucolor);

    TGTextButton *readBaseButton = new TGTextButton(frame1_1_2,"Read Base");
    frame1_1_2->AddFrame(readBaseButton, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,5,5,3,4));
    //reconstructButton->Connect("Clicked()","EDController",fManager,"recreate()");
    readBaseButton->ChangeBackground(ucolor);
    readBaseButton->SetEnabled(kFALSE);

    // adding Frame1_4
    TGCompositeFrame *frame1_4 = new TGCompositeFrame(frame1,1,1,kHorizontalFrame);
    frame1_4->ChangeBackground(ucolor);
    frame1->AddFrame(frame1_4, new TGLayoutHints(kLHintsExpandX /*| kLHintsExpandY*/));
    
    TGCheckButton *ShowFullSig = new TGCheckButton(frame1_4, "Full signal",1);
    frame1_4->AddFrame(ShowFullSig, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,5,5,3,4) );
    ShowFullSig->ChangeBackground(ucolor);
    /** @todo find a way to make ShowFullSig emit a bool value */
    ShowFullSig->Connect("Clicked()","jpet_event_display::GuiController", this , "fullSigDisplay()");

// adding Frame1_2

    TGCompositeFrame *frame1_2 = new TGCompositeFrame(frame1,1,1,kVerticalFrame);
    frame1_2->ChangeBackground(ucolor);
    frame1->AddFrame(frame1_2, new TGLayoutHints(kLHintsExpandX |kLHintsExpandY,1,1,1,1));

    //adding pTab

    TGTab* pTab = new TGTab(frame1_2, 1, 1);
    pTab->ChangeBackground(ucolor);
    //pTab->Connect("Selected(Int_t)", "EDGui", this, "doTab(Int_t)");

    // adding tabFrame1

    TGCompositeFrame* tf1 = pTab->AddTab("Info");
    tf1->ChangeBackground(ucolor);

    TGCompositeFrame* tabFrame1 = new TGCompositeFrame(tf1, 1, 1, kVerticalFrame);
    tabFrame1->ChangeBackground(ucolor);
    tf1->AddFrame(tabFrame1, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));

    fInputInfo = new TGLabel(tabFrame1,"No file read.",TGLabel::GetDefaultGC()(),TGLabel::GetDefaultFontStruct(),kChildFrame,ucolor);
    fInputInfo->SetTextJustify(kTextTop | kTextLeft);
    tabFrame1->AddFrame(fInputInfo, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,1,1,1,1));
    //inputInfo->SetParts(2);

    // adding tabFrame2

    TGCompositeFrame* tf2 = pTab->AddTab("Reconstruction");
    tf2->ChangeBackground(ucolor);

    TGCompositeFrame* tabFrame2 = new TGCompositeFrame(tf2, 1, 1, kVerticalFrame);
    tabFrame2->ChangeBackground(ucolor);
    tf2->AddFrame(tabFrame2, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 5, 5, 5, 5));

    // finalizing pTab
  
    pTab->SetEnabled(1,kTRUE);
    frame1_2->AddFrame(pTab, new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY, 2, 2, 5, 1));

    // adding Frame1_3

    TGCompositeFrame *frame1_3 = new TGCompositeFrame(frame1,1,1,kVerticalFrame);
    frame1_3->ChangeBackground(ucolor);
    frame1->AddFrame(frame1_3, new TGLayoutHints(kLHintsExpandX| kLHintsBottom,2,2,2,2));

    // adding Frame1_3_1

    TGCompositeFrame *frame1_3_1 = new TGCompositeFrame(frame1_3,1,1,kHorizontalFrame);
    frame1_3_1->ChangeBackground(ucolor);
    frame1_3->AddFrame(frame1_3_1, new TGLayoutHints(kLHintsExpandX| kLHintsTop,2,2,2,2));

    TGTextButton *nextButton = new TGTextButton(frame1_3_1,"&Next >");
    frame1_3_1->AddFrame(nextButton, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,5,5,3,4));
    nextButton->Connect("Clicked()","jpet_event_display::GuiController",this,"doNext()");
    nextButton->ChangeBackground(ucolor);

    /*TGTextButton *resetButton = new TGTextButton(frame1_3_1,"&Reset >");
    frame1_3_1->AddFrame(resetButton,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,5,5,3,4));
    resetButton->Connect("Clicked()","GuiController",this,"doReset()");
    resetButton->SetTextJustify(36);
    resetButton->ChangeBackground(ucolor);

    TGTextButton *showButton = new TGTextButton(frame1_3_1,"Show Data");
    frame1_3_1->AddFrame(showButton, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,5,5,3,4));
    showButton->Connect("Clicked()","GuiController",this,"clearPads()");
    showButton->Connect("Clicked()","GuiController",this,"updateGUIControlls()");
    showButton->Connect("Clicked()","GuiController",this,"showData()");
    showButton->ChangeBackground(ucolor);*/

    // adding Frame1_3_2

    TGCompositeFrame *frame1_3_2 = new TGCompositeFrame(frame1_3,1,1,kHorizontalFrame);
    frame1_3_2->ChangeBackground(ucolor);
    frame1_3->AddFrame(frame1_3_2, new TGLayoutHints(kLHintsExpandX| kLHintsTop,5,5,5,5));

    TGLabel *labelStep = new TGLabel(frame1_3_2,"Step",TGLabel::GetDefaultGC()(),TGLabel::GetDefaultFontStruct(),kChildFrame,ucolor);
    labelStep->SetTextJustify(36);
    frame1_3_2->AddFrame(labelStep, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

    fNumberEntryStep= new TGNumberEntry(frame1_3_2,1,5,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEAPositive,TGNumberFormat::kNELLimitMinMax,1,fMaxEvents);
    frame1_3_2->AddFrame(fNumberEntryStep, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
    fNumberEntryStep->Connect("ValueSet(Long_t)", "jpet_event_display::GuiController", this, "updateGUIControlls()");

    TGLabel *labelEventNo = new TGLabel(frame1_3_2,"Event no",TGLabel::GetDefaultGC()(),TGLabel::GetDefaultFontStruct(),kChildFrame,ucolor);
    labelEventNo->SetTextJustify(36);
    frame1_3_2->AddFrame(labelEventNo, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

    fNumberEntryEventNo= new TGNumberEntry(frame1_3_2,1,5,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,fMaxEvents);
    frame1_3_2->AddFrame(fNumberEntryEventNo, new TGLayoutHints(kLHintsExpandX));
    fNumberEntryEventNo->Connect("ValueSet(Long_t)", "jpet_event_display::GuiController", this, "updateGUIControlls()");


    fProgBar = new TGHProgressBar(frame1_3,TGProgressBar::kFancy,250);
    fProgBar->SetBarColor("lightblue");
    fProgBar->ShowPosition(kTRUE,kFALSE,"%.0f events");
    fProgBar->SetRange(0,fMaxEvents);
    frame1_3->AddFrame(fProgBar,new TGLayoutHints(kLHintsCenterX,5,5,3,4));

  // finalizing layout

    globalFrame->Resize(globalFrame->GetDefaultSize());
    baseFrame->Resize(baseFrame->GetDefaultSize());

    fMainWindow->SetWindowName("Single Strip Event Display ver 0.1");
    fMainWindow->MapSubwindows();
    fMainWindow->Resize(fMainWindow->GetDefaultSize());
    fMainWindow->MapWindow();
  }

void GuiController::openFileDialog (TGFileInfo* fFileInfo, EFileDialogMode dlg_type) {
  std::cout << "in openFileDialog" << "\n";
  new TGFileDialog(gClient->GetRoot(), fMainWindow, dlg_type, fFileInfo);
  //guiSignalController->readData();
  return;
}

  void GuiController::doNext()
  {
    fNumberEntryEventNo->SetIntNumber(fGUIControls->eventNo + fGUIControls->stepNo);
    updateGUIControlls();
  }



  void GuiController::updateGUIControlls() 
  {
    fGUIControls->eventNo = fNumberEntryEventNo->GetIntNumber();
    fGUIControls->stepNo = fNumberEntryStep->GetIntNumber();
    Emit("updateGUIControlls()");
  }

  void GuiController::CloseWindow() {
  gApplication->Terminate();
}

  void GuiController::draw() { fCanvas->Draw(); }
  void GuiController::run() { fApplication->Run(); }

}