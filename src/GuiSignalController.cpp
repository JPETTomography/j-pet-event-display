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
 *  @file GuiSignalController.cpp
 */


#include "GuiSignalController.h"
#include <TMath.h>
#include <cassert>
#include <iostream>

namespace jpet_event_display
{

  const char *filetypes[] = {
    "All files",     "*",
    "ROOT files",    "*.root",
    "Text files",    "*.[tT][xX][tT]",
    0,               0
  };

  GuiSignalController::GuiSignalController() {
    fFileInfo = new TGFileInfo;
   }
  GuiSignalController::~GuiSignalController() { }

  void GuiSignalController::addConnect(const std::string& functionName, const std::string& RQObjectName, 
                                  void* obj)
  {
    this->Connect(functionName.c_str(), RQObjectName.c_str(), static_cast<GuiController*>(obj), functionName.c_str());
  }

  void GuiSignalController::handleMenu (Int_t id) 
  {
    std::cout << "in handleMenu " << id << "\n";
    switch (id) {
      
      case M_FILE_OPEN:
      {
        TString dir("");
        fFileInfo->fFileTypes = filetypes;
        fFileInfo->fIniDir    = StrDup(dir);
        EmitVA("openFileDialog(TGFileInfo*,EFileDialogMode)",2,fFileInfo,(EFileDialogMode)kFDOpen);
      }
      break;

      case M_FILE_SAVE:
      {
        TString dir("");
        fFileInfo->fFileTypes = filetypes;
        fFileInfo->fIniDir    = StrDup(dir);
        EmitVA("openFileDialog(TGFileInfo*,EFileDialogMode)",2,fFileInfo,(EFileDialogMode)kFDSave);
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

  void GuiSignalController::setFiletype(enum FileType type){ // for now do nothing
	assert(type == kRoot || type == kScope || type == kSim);
	/*switch (type) {
		case kRoot:
            fDataProcessor->ChangeFiletype(EDDataProcessor::kROOT);
			break;
		case kScope:
            fDataProcessor->ChangeFiletype(EDDataProcessor::kScope);
			break;
        case kSim:
            fDataProcessor->ChangeFiletype(EDDataProcessor::kSim);
			break;
		default:
			break;
	}*/
  }
}