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
 *  @file GuiSignalController.h
 */


#ifndef GUISIGNALCONTROLLER_H
#define GUISIGNALCONTROLLER_H

#include <RQ_OBJECT.h>
#include <TGFileDialog.h>
#include <string>

namespace jpet_event_display
{
  class GuiController;
  enum EMessageTypes {
   M_FILE_OPEN,
   M_FILE_SAVE,
   M_FILE_EXIT
  };
  class GuiSignalController 
  {
    RQ_OBJECT("GuiSignalController")
  public:

    GuiSignalController();
    ~GuiSignalController();

    enum FileType{ kRoot, kScope, kSim };

    void handleMenu (Int_t id);
    void setFiletype(enum FileType type);

    void addConnect(const std::string& functionName, const std::string& RQObjectName, jpet_event_display::GuiController* obj);

  private:
    TGFileInfo* fFileInfo;
  };
}

#endif /*  !GUISIGNALCONTROLLER_H */