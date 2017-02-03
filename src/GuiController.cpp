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

namespace jpet_event_display
{
  GuiController::GuiController() 
  {
    fApplication = std::unique_ptr<TRint>(new TRint("App", 0, 0));
    fCanvas = std::make_shared<TCanvas>("MyCanvas", "Test Canvas", 10, 10, 900, 500);
  }
  GuiController::~GuiController() { }

  void GuiController::draw() { fCanvas->Draw(); }
  void GuiController::run() { fApplication->Run(); }

}