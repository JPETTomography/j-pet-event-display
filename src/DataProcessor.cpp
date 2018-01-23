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
#include <limits>

namespace jpet_event_display
{

DataProcessor::DataProcessor(std::shared_ptr< JPetGeomMapping > mapper)
{
  fMapper = mapper;
}

void DataProcessor::getDataForCurrentEvent()
{
  ProcessedData::getInstance().clearData();
  static std::map< std::string, int > compareMap;
  if (compareMap.empty()) {
    compareMap["JPetTimeWindow"] = FileTypes::fTimeWindow;
    compareMap["JPetRawSignal"] = FileTypes::fRawSignal;
    compareMap["JPetHit"] = FileTypes::fHit;
    compareMap["JPetEvent"] = FileTypes::fEvent;
    compareMap["JPetSigCh"] = FileTypes::fSigCh;
  }
  auto fCurrentTimeWindow =
    dynamic_cast<JPetTimeWindow&>(fReader.getCurrentEntry());

  if (fCurrentTimeWindow.getNumberOfEvents() <= 0) {
    ERROR("No events in time window");
    return;
  }
  const char* branchName = fCurrentTimeWindow[0].GetName();
  ProcessedData::getInstance().setCurrentFileType(
    static_cast< FileTypes >(compareMap[branchName]));

  switch (ProcessedData::getInstance().getCurrentFileType()) {
  case FileTypes::fSigCh:
    getActiveScintillators(fCurrentTimeWindow.getEvent< JPetSigCh >(
                             fNumberOfEventInCurrentTimeWindow));
    ProcessedData::getInstance().addToInfo(currentActivedScintillatorsInfo());
    break;
  case FileTypes::fRawSignal:
    getActiveScintillators(fCurrentTimeWindow.getEvent< JPetRawSignal >(
                             fNumberOfEventInCurrentTimeWindow));
    getDataForDiagram(fCurrentTimeWindow.getEvent< JPetRawSignal >(
                        fNumberOfEventInCurrentTimeWindow));
    ProcessedData::getInstance().addToInfo(currentActivedScintillatorsInfo());
    break;
  case FileTypes::fHit:
    getActiveScintillators(fCurrentTimeWindow.getEvent< JPetHit >(
                             fNumberOfEventInCurrentTimeWindow));
    getDataForDiagram(fCurrentTimeWindow.getEvent< JPetHit >(
                        fNumberOfEventInCurrentTimeWindow));
    getHitsPosition(fCurrentTimeWindow.getEvent< JPetHit >(
                      fNumberOfEventInCurrentTimeWindow));
    break;
  case FileTypes::fEvent:
    getActiveScintillators(fCurrentTimeWindow.getEvent< JPetEvent >(
                             fNumberOfEventInCurrentTimeWindow));
    getDataForDiagram(fCurrentTimeWindow.getEvent< JPetEvent >(
                        fNumberOfEventInCurrentTimeWindow));
    getHitsPosition(fCurrentTimeWindow.getEvent< JPetEvent >(
                      fNumberOfEventInCurrentTimeWindow));
    break;
  default:
    ProcessedData::getInstance().addToInfo("Not implemented object type");
    return;
    break;
  }
}

std::string DataProcessor::currentActivedScintillatorsInfo()
{
  std::map< int, std::map< size_t, int > > info;
  std::ostringstream oss;
  for (auto iter =
         ProcessedData::getInstance().getActivedScintilators().begin();
       iter != ProcessedData::getInstance().getActivedScintilators().end();
       ++iter) {
    int layer = iter->first;
    const std::vector< size_t >& strips = iter->second;
    for (auto stripIter = strips.begin(); stripIter != strips.end();
         ++stripIter) {
      size_t scin = *stripIter;
      info[layer][scin] = info[layer][scin] + 1;
    }
  }
  for (auto it = info.begin(); it != info.end(); it++) {
    for (auto itScin = it->second.begin(); itScin != it->second.end(); itScin++) {
      oss << "layer " << it->first << " scin " << itScin->first
          << " no of events " << itScin->second << "\n";
    }
  }
  return oss.str();
}

void DataProcessor::addToSelectionIfNotPresent(ScintillatorsInLayers& selection,
    StripPos& pos)
{
  if (selection.find(pos.layer) != selection.end()) {
    if (std::find(selection[pos.layer].begin(), selection[pos.layer].end(),
                  pos.slot) == selection[pos.layer].end())
      selection[pos.layer].push_back(pos.slot);
  } else {
    selection[pos.layer] = {pos.slot};
  }
}

void DataProcessor::getActiveScintillators(const JPetSigCh& sigCh)
{
  ScintillatorsInLayers selection;
  auto PM = sigCh.getPM();
  if (PM.isNullObject()) {
    return;
  }
  auto scin = PM.getScin();
  if (scin.isNullObject()) {
    return;
  }
  auto barrel = scin.getBarrelSlot();
  if (barrel.isNullObject()) {
    return;
  }
  StripPos pos = fMapper->getStripPos(barrel);

  addToSelectionIfNotPresent(selection, pos);
  ProcessedData::getInstance().addActivedScins(selection);
}

void DataProcessor::getActiveScintillators(const JPetRawSignal& rawSignal)
{
  auto leadingSigCh = rawSignal.getPoints(JPetSigCh::Leading);
  auto trailingSigCh = rawSignal.getPoints(JPetSigCh::Trailing);
  ScintillatorsInLayers selection;
  for (const auto& channel : leadingSigCh) {
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

    addToSelectionIfNotPresent(selection, pos);
  }

  for (const auto& channel : trailingSigCh) {
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

    addToSelectionIfNotPresent(selection, pos);
  }

  ProcessedData::getInstance().addActivedScins(selection);
}

void DataProcessor::getActiveScintillators(const JPetHit& hitSignal)
{
  ScintillatorsInLayers selection;
  StripPos pos = fMapper->getStripPos(hitSignal.getBarrelSlot());
  selection[pos.layer] = {pos.slot};

  ProcessedData::getInstance().addActivedScins(selection);
}

void DataProcessor::getActiveScintillators(const JPetEvent& event)
{
  ScintillatorsInLayers selection;
  for (JPetHit hit : event.getHits()) {
    StripPos pos = fMapper->getStripPos(hit.getBarrelSlot());
    addToSelectionIfNotPresent(selection, pos);
  }

  ProcessedData::getInstance().addActivedScins(selection);
}

DiagramDataMap DataProcessor::getDataForDiagram(const JPetRawSignal& rawSignal,
    bool)
{
  DiagramDataMap diagramData;
  StripPos pos = fMapper->getStripPos(
                   rawSignal.getPoints(JPetSigCh::Leading)[0].getPM().getBarrelSlot());
  auto data = rawSignal.getTimesVsThresholdValue(JPetSigCh::Leading);
  float startPos = data.begin()->second.second;
  float diff[4];
  float time = 0.f;
  for (auto it = data.begin(); it != data.end(); it++) {
    diff[it->first] = it->second.second - startPos;
    if (!fResetLeadingEdge)
      time = it->second.second;
    else
      time = 0.f;
    diagramData.push_back(std::make_tuple(
                            it->first, it->second.first, time, JPetSigCh::Leading,
                            rawSignal.getPoints(JPetSigCh::Leading)[0].getPM().getSide(), pos.layer,
                            pos.slot));
  }
  auto tmp = rawSignal.getTimesVsThresholdValue(JPetSigCh::Trailing);
  for (auto it = tmp.begin(); it != tmp.end(); it++) {
    if (!fResetLeadingEdge)
      time = it->second.second;
    else
      time = it->second.second - diff[it->first] - startPos;

    diagramData.push_back(std::make_tuple(
                            it->first, it->second.first, time, JPetSigCh::Trailing,
                            rawSignal.getPoints(JPetSigCh::Trailing)[0].getPM().getSide(), pos.layer,
                            pos.slot));
  }
  return diagramData;
}

void DataProcessor::getDataForDiagram(const JPetRawSignal& rawSignal)
{
  DiagramDataMapVector diagramDataVector;
  diagramDataVector.push_back(getDataForDiagram(rawSignal, true));
  ProcessedData::getInstance().addDiagram(diagramDataVector);
}

void DataProcessor::addToInfoFromStripPos(const StripPos& pos, const JPetHit& hit)
{
  double r =
    sqrt(hit.getPosX() * hit.getPosX() + hit.getPosY() * hit.getPosY());
  double fi = hit.getPosX() >= 0 ? asin(hit.getPosY() / r)
              : -asin(hit.getPosY() / r) + M_PI;
  std::ostringstream oss;
  oss << "layer: " << pos.layer << " scin: " << pos.slot << "\n"
      << " x: " << hit.getPosX() << "\n"
      << " y: " << hit.getPosY() << "\n"
      << " z: " << hit.getPosZ() << "\n"
      << " time: " << hit.getTime() << "\n"
      << "r: " << r << " theta: " << (fi * 180.0) / M_PI << "\n";
  ProcessedData::getInstance().addToInfo(oss.str());
}

void DataProcessor::getDataForDiagram(const JPetHit& hitSignal)
{
  DiagramDataMapVector diagramDataVector;
  diagramDataVector.push_back(getDataForDiagram(
                                hitSignal.getSignalA().getRecoSignal().getRawSignal(), true));
  diagramDataVector.push_back(getDataForDiagram(
                                hitSignal.getSignalB().getRecoSignal().getRawSignal(), true));
  ProcessedData::getInstance().addDiagram(diagramDataVector);

  StripPos pos = fMapper->getStripPos(hitSignal.getSignalA()
                                      .getRecoSignal()
                                      .getRawSignal()
                                      .getPoints(JPetSigCh::Leading)[0]
                                      .getPM()
                                      .getBarrelSlot());

  addToInfoFromStripPos(pos, hitSignal);
}

void DataProcessor::getDataForDiagram(const JPetEvent& event)
{
  DiagramDataMapVector diagramDataVector;
  if (event.getHits().size() < 1)
    return;
  for (JPetHit hit : event.getHits()) {
    diagramDataVector.push_back(getDataForDiagram(
                                  hit.getSignalA().getRecoSignal().getRawSignal(), true));
    diagramDataVector.push_back(getDataForDiagram(
                                  hit.getSignalB().getRecoSignal().getRawSignal(), true));

    StripPos pos = fMapper->getStripPos(hit.getSignalA()
                                        .getRecoSignal()
                                        .getRawSignal()
                                        .getPoints(JPetSigCh::Leading)[0]
                                        .getPM()
                                        .getBarrelSlot());
    // calculate position in (r, fi) domain from (x,y)

    addToInfoFromStripPos(pos, hit);
  }
  ProcessedData::getInstance().addDiagram(diagramDataVector);
}

void DataProcessor::getHitsPosition(const JPetHit& hitSignal)
{
  HitPositions hitsPos;
  hitsPos.push_back(hitSignal.getPos());
  ProcessedData::getInstance().addHits(hitsPos);
}

void DataProcessor::getHitsPosition(const JPetEvent& event)
{
  HitPositions hitsPos;
  for (JPetHit hit : event.getHits()) {
    hitsPos.push_back(hit.getPos());
  }
  ProcessedData::getInstance().addHits(hitsPos);
}

bool DataProcessor::openFile(const char* filename)
{
  fNumberOfEventsInFile = 0;
  bool openFileResult = fReader.openFileAndLoadData(filename);
  dynamic_cast< JPetParamBank* >(fReader.getObjectFromFile(
                                   "ParamBank")); // just read param bank, no need to save it to variable
  //long long numberOfTimeWindowses = fReader.getNbOfAllEntries();
  //for (long long i = 0; i < numberOfTimeWindowses; i++) {
  //  fReader.nthEntry(i);
  //  fNumberOfEventsInFile +=
  //    dynamic_cast<JPetTimeWindow&>(fReader.getCurrentEntry())
  //    .getNumberOfEvents();
  //}
  fNumberOfEventsInFile = std::numeric_limits<long long>::max();
  return openFileResult;
}

void DataProcessor::closeFile()
{
  fReader.closeFile();
}

bool DataProcessor::nextEvent()
{
  return fReader.nextEntry();
}

bool DataProcessor::firstEvent()
{
  return fReader.firstEntry();
}

bool DataProcessor::lastEvent()
{
  return fReader.lastEntry();
}

bool DataProcessor::nthEvent(long long n)
{
  long long currentEventToFind = n;
  if (n < fNumberOfEventsInFile) {
    for (long long i = 0; i < fReader.getNbOfAllEntries(); i++) {
      fReader.nthEntry(i);
      unsigned int numberOfEventsInTimeWindow =
        dynamic_cast<JPetTimeWindow&>(fReader.getCurrentEntry())
        .getNumberOfEvents();
      if (currentEventToFind < numberOfEventsInTimeWindow) {
        fNumberOfEventInCurrentTimeWindow = currentEventToFind;
        bool returnValue = fReader.nthEntry(i);
        getDataForCurrentEvent();
        return returnValue;
      } else {
        currentEventToFind -= numberOfEventsInTimeWindow;
      }
    }
    ERROR("Could not find event in file");
    return false;
  } else
    return false;
}
}
