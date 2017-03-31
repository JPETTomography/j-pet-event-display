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

void DataProcessor::getDataForCurrentEvent()
{
  switch(ProcessedData::getInstance().getCurrentFileType())
  {
    case FileTypes::fTimeWindow:
      ProcessedData::getInstance().setActivedScins(getActiveScintillators(getCurrentEvent<JPetTimeWindow>()));
      break;
    case FileTypes::fRawSignal:
    ProcessedData::getInstance().setActivedScins(getActiveScintillators(getCurrentEvent<JPetRawSignal>()));
    ProcessedData::getInstance().setDiagram(getDataForDiagram(getCurrentEvent<JPetRawSignal>()));
    break;
    default:
      break;
  }
}

template<typename T>
const T& DataProcessor::getCurrentEvent()
{
  return dynamic_cast<T &>(fReader.getCurrentEvent());
}

ScintillatorsInLayers DataProcessor::getActiveScintillators(const JPetTimeWindow& tWindow)
{
  auto sigChannels = tWindow.getSigChVect();
  ScintillatorsInLayers selection;
  for (const auto & channel : sigChannels) {
    auto PM = channel.getPM();
    if(PM.isNullObject()) {
      continue;
    }
    auto scin = PM.getScin();
    if(scin.isNullObject()) {
      continue;
    }
    auto scinId = scin.getID();
    auto barrel = scin.getBarrelSlot();
    if (barrel.isNullObject()) {
      continue;
    }
    StripPos pos = fMapper->getStripPos(barrel);

    if (selection.find(pos.layer) != selection.end()) {
      if (std::find(selection[pos.layer].begin(), selection[pos.layer].end(),
                    pos.slot) == selection[pos.layer].end())
        selection[pos.layer].push_back(pos.slot);
    } else {
      selection[pos.layer] = {pos.slot};
    }
  }
  return selection;
}

ScintillatorsInLayers
DataProcessor::getActiveScintillators(const JPetRawSignal &rawSignal)
{
  auto leadingSigCh = rawSignal.getPoints(JPetSigCh::Leading);
  auto trailingSigCh = rawSignal.getPoints(JPetSigCh::Trailing);
  ScintillatorsInLayers selection;
  for (const auto &channel : leadingSigCh) {
    auto PM = channel.getPM();
    if (PM.isNullObject()) {
      continue;
    }
    auto scin = PM.getScin();
    if (scin.isNullObject()) {
      continue;
    }
    auto scinId = scin.getID();
    auto barrel = scin.getBarrelSlot();
    if (barrel.isNullObject()) {
      continue;
    }
    StripPos pos = fMapper->getStripPos(barrel);

    if (selection.find(pos.layer) != selection.end()) {
      if (std::find(selection[pos.layer].begin(), selection[pos.layer].end(),
                    pos.slot) == selection[pos.layer].end())
        selection[pos.layer].push_back(pos.slot);
    } else {
      selection[pos.layer] = {pos.slot};
    }
  }

  for (const auto &channel : trailingSigCh) {
    auto PM = channel.getPM();
    if (PM.isNullObject()) {
      continue;
    }
    auto scin = PM.getScin();
    if (scin.isNullObject()) {
      continue;
    }
    auto scinId = scin.getID();
    auto barrel = scin.getBarrelSlot();
    if (barrel.isNullObject()) {
      continue;
    }
    StripPos pos = fMapper->getStripPos(barrel);

    if (selection.find(pos.layer) != selection.end()) {
      if (std::find(selection[pos.layer].begin(), selection[pos.layer].end(),
                    pos.slot) == selection[pos.layer].end())
        selection[pos.layer].push_back(pos.slot);
    } else {
      selection[pos.layer] = {pos.slot};
    }
  }

  return selection;
}

DiagramDataMap DataProcessor::getDataForDiagram(const JPetRawSignal &rawSignal)
{
  return rawSignal.getTimesVsThresholdValue(JPetSigCh::Leading);
}

bool DataProcessor::openFile(const char *filename) {
  static std::map<std::string, int> compareMap;
  if (compareMap.empty())
  {
    compareMap["JPetTimeWindow"] = FileTypes::fTimeWindow;
    compareMap["JPetRawSignal"] = FileTypes::fRawSignal;
  }
  bool r = fReader.openFileAndLoadData(filename);
  fNumberOfEventsInFile = fReader.getNbOfAllEvents();
  if(r)
  {
    TTree *fTree = dynamic_cast<TTree *>(fReader.getObjectFromFile("tree"));
    TObjArray *arr = fTree->GetListOfBranches();
    TBranch *fBranch = dynamic_cast<TBranch*>(arr->At(0));
    const char *branchName = fBranch->GetClassName();
    switch(compareMap[branchName])
    {
      case FileTypes::fTimeWindow:
        ProcessedData::getInstance().setCurrentFileType(FileTypes::fTimeWindow);
        break;
      case FileTypes::fRawSignal:
        ProcessedData::getInstance().setCurrentFileType(FileTypes::fRawSignal);
        break;
      default:
        ProcessedData::getInstance().setCurrentFileType(FileTypes::fNone);
        break;
    }
    // set mapper
    JPetParamManager fparamManagerInstance(new JPetParamGetterAscii("large_barrel.json"));
    fparamManagerInstance.fillParameterBank(43);
    auto bank = fparamManagerInstance.getParamBank();
    const JPetParamBank *bank2 =
        dynamic_cast<JPetParamBank *>(fReader.getObjectFromFile("ParamBank"));
    fMapper = std::unique_ptr<JPetGeomMapping>(new JPetGeomMapping(bank));
  }
  return r;
}

void DataProcessor::closeFile()
{
  fReader.closeFile();
}

bool DataProcessor::nextEvent()
{
  return fReader.nextEvent();
}

bool DataProcessor::firstEvent()
{
  return fReader.firstEvent();
}

bool DataProcessor::lastEvent()
{
  return fReader.lastEvent();
}

bool DataProcessor::nthEvent(long long n)
{
  if (n < fNumberOfEventsInFile)
  {
    bool returnValue = fReader.nthEvent(n);
    getDataForCurrentEvent();
    return returnValue;
  }
  else
    return false;
}
}
