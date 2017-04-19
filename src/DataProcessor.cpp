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
  DiagramDataMapVector tmpV;
  switch(ProcessedData::getInstance().getCurrentFileType())
  {
    case FileTypes::fTimeWindow:
      ProcessedData::getInstance().setActivedScins(
          getActiveScintillators(getCurrentEvent<JPetTimeWindow>()));
      break;
    case FileTypes::fRawSignal:
      ProcessedData::getInstance().setActivedScins(
          getActiveScintillators(getCurrentEvent<JPetRawSignal>()));
      tmpV.push_back(getDataForDiagram(getCurrentEvent<JPetRawSignal>()));
      ProcessedData::getInstance().setDiagram(tmpV);
      break;
    case FileTypes::fHit:
      ProcessedData::getInstance().setActivedScins(
          getActiveScintillators(getCurrentEvent<JPetHit>()));
      ProcessedData::getInstance().setDiagram(
          getDataForDiagram(getCurrentEvent<JPetHit>()));
      break;
    case FileTypes::fEvent:
      ProcessedData::getInstance().setActivedScins(
          getActiveScintillators(getCurrentEvent<JPetEvent>()));
      ProcessedData::getInstance().setDiagram(
          getDataForDiagram(getCurrentEvent<JPetEvent>()));
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

ScintillatorsInLayers DataProcessor::getActiveScintillators(const JPetHit &hitSignal)
{

  std::cout << "x: " << hitSignal.getPosX() << " y: " << hitSignal.getPosY()
            << " z: " << hitSignal.getPosZ()
            << " energy: " << hitSignal.getEnergy()
            << " qualityOfEnergy: " << hitSignal.getQualityOfEnergy()
            << " time: " << hitSignal.getTime()
            << " timeDiff: " << hitSignal.getTimeDiff()
            << " qualityOfTime: " << hitSignal.getQualityOfTime()
            << " qualityOfTimeDiff: " << hitSignal.getQualityOfTimeDiff()
            << "\n";

  ScintillatorsInLayers selection;
  StripPos pos = fMapper->getStripPos(hitSignal.getBarrelSlot());
  selection[pos.layer] = {pos.slot};
  return selection;
}

ScintillatorsInLayers
DataProcessor::getActiveScintillators(const JPetEvent &event)
{
  ScintillatorsInLayers selection;
  for(JPetHit hit : event.getHits())
  {
    std::cout << "x: " << hit.getPosX() << " y: " << hit.getPosY()
              << " z: " << hit.getPosZ() << " energy: " << hit.getEnergy()
              << " qualityOfEnergy: " << hit.getQualityOfEnergy()
              << " time: " << hit.getTime()
              << " timeDiff: " << hit.getTimeDiff()
              << " qualityOfTime: " << hit.getQualityOfTime()
              << " qualityOfTimeDiff: " << hit.getQualityOfTimeDiff() << "\n";

    StripPos pos = fMapper->getStripPos(hit.getBarrelSlot());
    if (selection.find(pos.layer) != selection.end())
    {
      if (std::find(selection[pos.layer].begin(), selection[pos.layer].end(),
                    pos.slot) == selection[pos.layer].end())
        selection[pos.layer].push_back(pos.slot);
    }
    else
    {
      selection[pos.layer] = {pos.slot};
    }
  }

  return selection;
}

DiagramDataMap DataProcessor::getDataForDiagram(const JPetRawSignal &rawSignal)
{
  DiagramDataMap r;
  auto data = rawSignal.getTimesVsThresholdValue(JPetSigCh::Leading);
  for (auto it = data.begin(); it != data.end(); it++)
  {
    r.push_back(std::make_tuple(it->first, it->second.first,
                                it->second.second, JPetSigCh::Leading));
  }
  auto tmp = rawSignal.getTimesVsThresholdValue(JPetSigCh::Trailing);
  int dataSize = data.size();
  for (auto it = tmp.begin(); it != tmp.end(); it++)
  {
    r.push_back(std::make_tuple(it->first, it->second.first,
                                it->second.second, JPetSigCh::Trailing));
  }

  return r;
}

DiagramDataMapVector DataProcessor::getDataForDiagram(const JPetHit &hitSignal)
{
  DiagramDataMap signalA =
      getDataForDiagram(hitSignal.getSignalA().getRecoSignal().getRawSignal());
  DiagramDataMap signalB =
      getDataForDiagram(hitSignal.getSignalB().getRecoSignal().getRawSignal());
  DiagramDataMapVector result;
  result.push_back(signalA);
  result.push_back(signalB);
  return result;
}

DiagramDataMapVector DataProcessor::getDataForDiagram(const JPetEvent &event)
{
  DiagramDataMapVector result;
  for(JPetHit hit : event.getHits())
  {
    DiagramDataMap signalA =
        getDataForDiagram(hit.getSignalA().getRecoSignal().getRawSignal());
    DiagramDataMap signalB =
        getDataForDiagram(hit.getSignalB().getRecoSignal().getRawSignal());

    result.push_back(signalA);
    result.push_back(signalB);
  }
  return result;
}

bool DataProcessor::openFile(const char *filename) {
  static std::map<std::string, int> compareMap;
  if (compareMap.empty())
  {
    compareMap["JPetTimeWindow"] = FileTypes::fTimeWindow;
    compareMap["JPetRawSignal"] = FileTypes::fRawSignal;
    compareMap["JPetHit"] = FileTypes::fHit;
    compareMap["JPetEvent"] = FileTypes::fEvent;
  }
  bool r = fReader.openFileAndLoadData(filename);
  fNumberOfEventsInFile = fReader.getNbOfAllEvents();
  if(r)
  {
    TTree *fTree = dynamic_cast<TTree *>(fReader.getObjectFromFile("tree"));
    TObjArray *arr = fTree->GetListOfBranches();
    TBranch *fBranch = dynamic_cast<TBranch*>(arr->At(0));
    const char *branchName = fBranch->GetClassName();
    ProcessedData::getInstance().setCurrentFileType(static_cast<FileTypes>(compareMap[branchName]));
    // set mapper
    JPetParamManager fparamManagerInstance(new JPetParamGetterAscii("large_barrel.json"));
    fparamManagerInstance.fillParameterBank(43);
    auto bank = fparamManagerInstance.getParamBank();
    //const JPetParamBank *bank2 =
    dynamic_cast<JPetParamBank *>(fReader.getObjectFromFile("ParamBank")); // just read param bank, no need to save it to variable
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
