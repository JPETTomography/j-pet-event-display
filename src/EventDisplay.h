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

namespace jpet_event_display
{

class EventDisplay
{
public:
  EventDisplay(const std::string& inFile, const std::string& inFileType, std::unique_ptr<TRint> theApp,   const std::string& geomFile = "JPET_geom.root");
  void run(const std::string& inFile, const std::string& geomFile);

private:
  EventDisplay(const EventDisplay&) = delete;
  EventDisplay& operator=(const EventDisplay&) = delete;

  std::unique_ptr<TRint> fApplication = nullptr;
};

}
#endif /*  !EVENTDISPLAY_H */
