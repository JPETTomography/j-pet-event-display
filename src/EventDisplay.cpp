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

//#include <TError.h>
//#include <TROOT.h>
//#include <TEnv.h>
//#include <TCanvas.h>
#include <JPetLoggerInclude.h>
//#include "GeometryVisualizator.h"
//#include "DataProcessor.h"

namespace jpet_event_display
{

const char *filetypes[] = {
    "All files", "*",
    "ROOT files", "*.root",
    "Text files", "*.[tT][xX][tT]",
    0, 0};

EventDisplay::EventDisplay() {}

EventDisplay::EventDisplay(const std::string &inFile, const std::string &inFileType,
                           const std::string &geomFile)
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

EventDisplay::~EventDisplay() {
  fMainWindow->Cleanup();
}

void EventDisplay::run(const std::string &inFile, const std::string &geomFile)
{
  fMainWindow = std::unique_ptr<TGMainFrame>(new TGMainFrame(gClient->GetRoot()));
  fMainWindow->SetCleanup(kDeepCleanup);
  fMainWindow->Connect("CloseWindow()", "jpet_event_display::EventDisplay", this, "CloseWindow()");
  const Int_t w_max = 1024;
  const Int_t h_max = 720;
  fMainWindow->SetWMSize(w_max, h_max); //this is the only way to set the size of the main frame
  fMainWindow->SetLayoutBroken(kTRUE);
  assert(fMainWindow != 0);
  const Int_t w_GlobalFrame = w_max;
  const Int_t h_GlobalFrame = h_max;
  const Int_t w_Frame1 = 180;
  const Int_t h_Frame1 = h_max - 12; // cause 2*5 margins from GlobalFrame and 2*1 margins from Frame1
  const Int_t w_Frame2 = w_max - w_Frame1;
  const Int_t h_Frame2 = h_max - 12;
  int fMaxEvents = 1000; // temporary

  // adding baseFrame

  TGCompositeFrame *baseFrame = new TGCompositeFrame(fMainWindow.get(), w_GlobalFrame, h_GlobalFrame - 10, kVerticalFrame);
  fMainWindow->AddFrame(baseFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0));

  ULong_t ucolor = 0;
  gClient->GetColorByName("#e6e6e6", ucolor);
  baseFrame->ChangeBackground(ucolor);

  // adding fmenuBar

  TGMenuBar *fMenuBar = new TGMenuBar(baseFrame, 1, 1, kHorizontalFrame);
  fMenuBar->ChangeBackground(ucolor);

  TGLayoutHints *fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsRight, 0, 0, 0, 0);
  TGLayoutHints *fMenuBarLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop, 0, 0, 0, 0);

  TGPopupMenu *fMenuFile = new TGPopupMenu(gClient->GetRoot());
  fMenuFile->AddEntry(" &Open...\tCtrl+O", M_FILE_OPEN);
  fMenuFile->AddSeparator();
  fMenuFile->AddEntry(" E&xit\tCtrl+Q", M_FILE_EXIT);
  fMenuFile->Associate(fMainWindow.get());
  fMenuFile->Connect("Activated(Int_t)", "jpet_event_display::EventDisplay", this, "handleMenu(Int_t)");

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

  TRootEmbeddedCanvas *fEcanvas = new TRootEmbeddedCanvas("Ecanvas",frame2,600,600);
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
  TGRadioButton * chooseROOT = new TGRadioButton(chooseFiletype, new TGHotString("TimeWindow"),0);
  TGRadioButton * chooseScope = new TGRadioButton(chooseFiletype ,new TGHotString("filetype2"),1);
  TGRadioButton * chooseSim = new TGRadioButton(chooseFiletype ,new TGHotString("filetype3"),2);
  chooseFiletype->ChangeBackground(ucolor);
  chooseROOT->ChangeBackground(ucolor);
  chooseSim->ChangeBackground(ucolor);
  chooseScope->ChangeBackground(ucolor);
  chooseFiletype->Show();
  frame1_1_1->AddFrame(chooseFiletype,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,5,5,3,4));
  chooseFiletype->Connect("Pressed(Int_t)", "jpet_event_display::EventDisplay", 
                          this, "setFiletype(enum FileType)");
  chooseScope->SetState(kButtonDown,kFALSE);
  chooseSim->SetState(kButtonDown,kFALSE);
  chooseROOT->SetState(kButtonDown,kTRUE);

  // adding Frame1_1_2

  TGCompositeFrame *frame1_1_2 = new TGCompositeFrame(frame1_1,1,1,kHorizontalFrame);
  frame1_1_2->ChangeBackground(ucolor);
  frame1_1->AddFrame(frame1_1_2, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,2,2,2,2));

  TGTextButton *readButton = new TGTextButton(frame1_1_2,"Read Data");
  frame1_1_2->AddFrame(readButton,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,5,5,3,4));
  readButton->Connect("Clicked()", "jpet_event_display::EventDisplay", this, "handleMenu(=0)");
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
  frame1->AddFrame(frame1_4, new TGLayoutHints(kLHintsExpandX));

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

  TGLabel *fInputInfo = new TGLabel(tabFrame1,"No file read.",TGLabel::GetDefaultGC()(),TGLabel::GetDefaultFontStruct(),kChildFrame,ucolor);
  fInputInfo->SetTextJustify(kTextTop | kTextLeft);
  tabFrame1->AddFrame(fInputInfo, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,1,1,1,1));
  //inputInfo->SetParts(2);

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
  nextButton->Connect("Clicked()","jpet_event_display::EventDisplay",this,"doNext()");
  nextButton->ChangeBackground(ucolor);

    

  // adding Frame1_3_2

  TGCompositeFrame *frame1_3_2 = new TGCompositeFrame(frame1_3,1,1,kHorizontalFrame);
  frame1_3_2->ChangeBackground(ucolor);
  frame1_3->AddFrame(frame1_3_2, new TGLayoutHints(kLHintsExpandX| kLHintsTop,5,5,5,5));

  TGLabel *labelStep = new TGLabel(frame1_3_2,"Step",TGLabel::GetDefaultGC()(),TGLabel::GetDefaultFontStruct(),kChildFrame,ucolor);
  labelStep->SetTextJustify(36);
  frame1_3_2->AddFrame(labelStep, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  fNumberEntryStep = std::unique_ptr<TGNumberEntry>(new TGNumberEntry(frame1_3_2,
                                                  1, 5, -1, TGNumberFormat::kNESInteger,
                                                  TGNumberFormat::kNEAPositive,
                                                  TGNumberFormat::kNELLimitMinMax, 1,fMaxEvents));
  frame1_3_2->AddFrame(fNumberEntryStep.get(), new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  fNumberEntryStep->Connect("ValueSet(Long_t)", "jpet_event_display::EventDisplay", this, "updateGUIControlls()");

  TGLabel *labelEventNo = new TGLabel(frame1_3_2,"Event no",TGLabel::GetDefaultGC()(),TGLabel::GetDefaultFontStruct(),kChildFrame,ucolor);
  labelEventNo->SetTextJustify(36);
  frame1_3_2->AddFrame(labelEventNo, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  fNumberEntryEventNo = std::unique_ptr<TGNumberEntry>(new TGNumberEntry(frame1_3_2,
                                                      1, 5, -1, TGNumberFormat::kNESInteger,
                                                      TGNumberFormat::kNEANonNegative,
                                                      TGNumberFormat::kNELLimitMinMax, 0, fMaxEvents));
  frame1_3_2->AddFrame(fNumberEntryEventNo.get(), new TGLayoutHints(kLHintsExpandX));
  fNumberEntryEventNo->Connect("ValueSet(Long_t)", "jpet_event_display::EventDisplay", this, "updateGUIControlls()");

  // finalizing layout

  globalFrame->Resize(globalFrame->GetDefaultSize());
  baseFrame->Resize(baseFrame->GetDefaultSize());

  fMainWindow->SetWindowName("Single Strip Event Display ver 0.1");
  fMainWindow->MapSubwindows();
  fMainWindow->Resize(fMainWindow->GetDefaultSize());
  fMainWindow->MapWindow();
}

void EventDisplay::CloseWindow()
{
  gApplication->Terminate();
}

void EventDisplay::handleMenu(Int_t id)
{
  std::cout << "in handleMenu " << id << "\n";

  switch (id)
  {

  case M_FILE_OPEN:
  {
    TString dir("");
    fFileInfo->fFileTypes = filetypes;
    fFileInfo->fIniDir = StrDup(dir);
    new TGFileDialog(gClient->GetRoot(), fMainWindow.get(), kFDOpen, fFileInfo.get());
  }
  break;
  case M_FILE_SAVE:
  {
    TString dir("");
    fFileInfo->fFileTypes = filetypes;
    fFileInfo->fIniDir = StrDup(dir);
    new TGFileDialog(gClient->GetRoot(), fMainWindow.get(), kFDSave, fFileInfo.get());
  }
  break;
  case M_FILE_EXIT:
  {
    Emit("exit()");
  }
  break;
  }
  return;
}

void EventDisplay::setFiletype(enum FileType type){
  assert(type == TimeWindow || type == FileType2 || type == FileType3);
}

void EventDisplay::updateGUIControlls() 
{
  fGUIControls->eventNo = fNumberEntryEventNo->GetIntNumber();
  fGUIControls->stepNo = fNumberEntryStep->GetIntNumber();
  Emit("updateGUIControlls()");
}

void EventDisplay::doNext()
{
  fNumberEntryEventNo->SetIntNumber(fGUIControls->eventNo + fGUIControls->stepNo);
  updateGUIControlls();
}

}
