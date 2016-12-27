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
 */

#include "./DataProcessor.h"

namespace jpet_event_display
{

ScintillatorsInLayers DataProcessor::getActiveScintillators(const JPetTimeWindow& tWindow)
{
  auto sigChannels = tWindow.getSigChVect();
  ScintillatorsInLayers selection;
  /// This loop makes not much sense, but just for tests.
  for (const auto & channel : sigChannels) {
    /// we need to use some mapping of those numbers but for a moment let's use those values.
    /// ! In some analyses PM, Scin, BarrelSlots etc will be not set 
    /// We should use part of the code from TaskB1 with LargeBarrelMapping
    /// auto scinId = channel.getPM().getScin().getID();
    /// auto layerId = channel.getPM().getScin().getBarrelSlot().getLayer().getID();
    /// Just for tests.
    auto scinId1 = 10;
    auto scinId2 = 11;
    auto scinId3 = 12;
    auto scinId4 = 13;
    auto layerId = 0;
    if (selection.find(layerId) != selection.end()) {
      /// The key already exists so we just add this element
      selection[layerId].push_back(scinId1);
    } else {
      selection[layerId] = {scinId1, scinId2, scinId3, scinId4};
    }
    //channels.push_back(10);
    //channels.push_back(11);
    //channels.push_back(12);
    //channels.push_back(13);
    //selection[0] = channels;
  }
  return selection;
}
bool DataProcessor::openFile(const char* filename)
{
  return fReader.openFileAndLoadData(filename);
}

void DataProcessor::closeFile()
{
  fReader.closeFile();
}
JPetTimeWindow& DataProcessor::getCurrentEvent()
{
  return dynamic_cast<JPetTimeWindow&> (fReader.getCurrentEvent());
}

bool DataProcessor::nextEvent()
{
  return fReader.nextEvent();
}

}
