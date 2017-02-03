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
 *  @file GuiController.h
 */

#include <memory>
#include <TRint.h>
#include <TCanvas.h>

#ifndef GUICONTROLLER_H
#define GUICONTROLLER_H

namespace jpet_event_display
{
  class GuiController 
  {
  public:
    GuiController();
    ~GuiController();

    void draw();
    void run();

    std::shared_ptr<TCanvas> getCanvas() { return fCanvas; }


  
  private:
    GuiController(const GuiController&) = delete;
    GuiController& operator=(const GuiController&) = delete;

    std::unique_ptr<TRint> fApplication;
    std::shared_ptr<TCanvas> fCanvas;

  };
}

#endif /*  !GUICONTROLLER_H */