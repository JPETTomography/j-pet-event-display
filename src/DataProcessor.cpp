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
#include <iostream>

namespace jpet_event_display
{

ScintillatorsInLayers DataProcessor::getActiveScintillators(const JPetTimeWindow& tWindow)
{
  auto sigChannels = tWindow.getSigChVect();
  
  ScintillatorsInLayers selection;
  int skipped = 0;
  int added = 0;
  /// This loop makes not much sense, but just for tests.
  for (const auto & channel : sigChannels) {
    /// we need to use some mapping of those numbers but for a moment let's use those values.
    /// ! In some analyses PM, Scin, BarrelSlots etc will be not set 
    /// We should use part of the code from TaskB1 with LargeBarrelMapping
    /// auto scinId = channel.getPM().getScin().getID();
    /// auto layerId = channel.getPM().getScin().getBarrelSlot().getLayer().getID();
    /// Just for tests.
    
    auto PM = channel.getPM();
    if(PM.isNullObject()) {
      skipped++;
      continue;
    }
    auto scin = PM.getScin();
    if(scin.isNullObject()) {
      skipped++;
      continue;
    }
    auto scinId = scin.getID();
    auto barrel = scin.getBarrelSlot();
    if(barrel.isNullObject()) {
      skipped++;
      continue;
    }
    auto layer = barrel.getLayer();
    if(layer.isNullObject()) {
      skipped++;
      continue;
    }
    auto layerId = layer.getID();
    if (selection.find(layerId) != selection.end()) {
      /// The key already exists so we just add this element
      added++;
      selection[layerId].push_back(scinId);
    } else {
      added++;
      selection[layerId] = {scinId};
    }
  }
  return selection;
}

ScintillatorsInLayers DataProcessor::getActiveScintillators(const JPetParamBank* pBank)
{
  ScintillatorsInLayers selection;
  int skipped = 0;
  int added = 0;
  auto scins = pBank->getScintillators();

  for(auto scin : scins) {
    if(scin.second->isNullObject()) {
      skipped++;
      continue;
    }
    auto scinId = scin.second->getID();
    auto barrel = scin.second->getBarrelSlot();
    if(barrel.isNullObject()) {
      skipped++;
      continue;
    }
    auto layer = barrel.getLayer();
    if(layer.isNullObject()) {
      skipped++;
      continue;
    }
    auto layerId = layer.getID();
    if (selection.find(layerId) != selection.end()) {
      added++;
      selection[layerId].push_back(scinId);
    } else {
      added++;
      selection[layerId] = {scinId};
    }
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

JPetParamBank* DataProcessor::getParamBank() {
  return dynamic_cast<JPetParamBank*> (fReader.getObjectFromFile("ParamBank"));
}

bool DataProcessor::nextEvent()
{
  return fReader.nextEvent();
}

}
