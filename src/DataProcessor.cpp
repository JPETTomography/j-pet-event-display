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

DataProcessor::DataProcessor()
{
  std::cout
      << "Generating GeomMapping, please wait, application will start soon... "
      << "\n";
  JPetParamManager fparamManagerInstance(
      new JPetParamGetterAscii("large_barrel.json"));
  fparamManagerInstance.fillParameterBank(43);
  auto bank = fparamManagerInstance.getParamBank();
  fMapper = std::unique_ptr< JPetGeomMapping >(new JPetGeomMapping(bank));
}

void DataProcessor::getDataForCurrentEvent()
{
  ProcessedData::getInstance().clearData();
  switch (ProcessedData::getInstance().getCurrentFileType())
  {
  case FileTypes::fTimeWindow:
    getActiveScintillators(getCurrentEvent< JPetTimeWindow >());
    break;
  case FileTypes::fRawSignal:
    getActiveScintillators(getCurrentEvent< JPetRawSignal >());
    getDataForDiagram(getCurrentEvent< JPetRawSignal >());
    break;
  case FileTypes::fHit:
    getActiveScintillators(getCurrentEvent< JPetHit >());
    getDataForDiagram(getCurrentEvent< JPetHit >());
    getHitsPosition(getCurrentEvent< JPetHit >());
    break;
  case FileTypes::fEvent:
    getActiveScintillators(getCurrentEvent< JPetEvent >());
    getDataForDiagram(getCurrentEvent< JPetEvent >());
    getHitsPosition(getCurrentEvent< JPetEvent >());
    break;
  default:
    break;
  }
}

template < typename T > const T &DataProcessor::getCurrentEvent()
{
  return dynamic_cast< T & >(fReader.getCurrentEvent());
}

void DataProcessor::getActiveScintillators(const JPetTimeWindow &tWindow)
{
  auto sigChannels = tWindow.getSigChVect();
  ScintillatorsInLayers selection;
  for (const auto &channel : sigChannels)
  {
    auto PM = channel.getPM();
    if (PM.isNullObject())
    {
      continue;
    }
    auto scin = PM.getScin();
    if (scin.isNullObject())
    {
      continue;
    }
    auto barrel = scin.getBarrelSlot();
    if (barrel.isNullObject())
    {
      continue;
    }
    StripPos pos = fMapper->getStripPos(barrel);

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

  ProcessedData::getInstance().setActivedScins(selection);
}

void DataProcessor::getActiveScintillators(const JPetRawSignal &rawSignal)
{
  auto leadingSigCh = rawSignal.getPoints(JPetSigCh::Leading);
  auto trailingSigCh = rawSignal.getPoints(JPetSigCh::Trailing);
  ScintillatorsInLayers selection;
  for (const auto &channel : leadingSigCh)
  {
    auto PM = channel.getPM();
    if (PM.isNullObject())
    {
      continue;
    }
    auto scin = PM.getScin();
    if (scin.isNullObject())
    {
      continue;
    }
    auto barrel = scin.getBarrelSlot();
    if (barrel.isNullObject())
    {
      continue;
    }
    StripPos pos = fMapper->getStripPos(barrel);

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

  for (const auto &channel : trailingSigCh)
  {
    auto PM = channel.getPM();
    if (PM.isNullObject())
    {
      continue;
    }
    auto scin = PM.getScin();
    if (scin.isNullObject())
    {
      continue;
    }
    auto barrel = scin.getBarrelSlot();
    if (barrel.isNullObject())
    {
      continue;
    }
    StripPos pos = fMapper->getStripPos(barrel);

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

  ProcessedData::getInstance().setActivedScins(selection);
}

void DataProcessor::getActiveScintillators(const JPetHit &hitSignal)
{
  ScintillatorsInLayers selection;
  StripPos pos = fMapper->getStripPos(hitSignal.getBarrelSlot());
  selection[pos.layer] = {pos.slot};

  ProcessedData::getInstance().setActivedScins(selection);
}

void DataProcessor::getActiveScintillators(const JPetEvent &event)
{
  ScintillatorsInLayers selection;
  for (JPetHit hit : event.getHits())
  {
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

  ProcessedData::getInstance().setActivedScins(selection);
}

DiagramDataMap DataProcessor::getDataForDiagram(const JPetRawSignal &rawSignal,
                                                bool)
{
  DiagramDataMap r;
  auto data = rawSignal.getTimesVsThresholdValue(JPetSigCh::Leading);
  for (auto it = data.begin(); it != data.end(); it++)
  {
    r.push_back(std::make_tuple(it->first, it->second.first, it->second.second,
                                JPetSigCh::Leading));
  }
  auto tmp = rawSignal.getTimesVsThresholdValue(JPetSigCh::Trailing);
  for (auto it = tmp.begin(); it != tmp.end(); it++)
  {
    r.push_back(std::make_tuple(it->first, it->second.first, it->second.second,
                                JPetSigCh::Trailing));
  }
  return r;
}

void DataProcessor::getDataForDiagram(const JPetRawSignal &rawSignal)
{
  DiagramDataMapVector rv;
  rv.push_back(getDataForDiagram(rawSignal, true));
  ProcessedData::getInstance().setDiagram(rv);
}

void DataProcessor::getDataForDiagram(const JPetHit &hitSignal)
{
  DiagramDataMapVector rv;
  rv.push_back(getDataForDiagram(
      hitSignal.getSignalA().getRecoSignal().getRawSignal(), true));
  rv.push_back(getDataForDiagram(
      hitSignal.getSignalB().getRecoSignal().getRawSignal(), true));
  ProcessedData::getInstance().setDiagram(rv);
}

void DataProcessor::getDataForDiagram(const JPetEvent &event)
{
  DiagramDataMapVector rv;
  for (JPetHit hit : event.getHits())
  {
    rv.push_back(getDataForDiagram(
        hit.getSignalA().getRecoSignal().getRawSignal(), true));
    rv.push_back(getDataForDiagram(
        hit.getSignalB().getRecoSignal().getRawSignal(), true));
  }
  ProcessedData::getInstance().setDiagram(rv);
}

void DataProcessor::getHitsPosition(const JPetHit &hitSignal)
{
  HitPositions hits;
  hits.push_back(hitSignal.getPos());
  ProcessedData::getInstance().setHits(hits);
}

void DataProcessor::getHitsPosition(const JPetEvent &event)
{
  HitPositions hits;
  for (JPetHit hit : event.getHits())
  {
    hits.push_back(hit.getPos());
  }
  ProcessedData::getInstance().setHits(hits);
}

bool DataProcessor::openFile(const char *filename)
{
  static std::map< std::string, int > compareMap;
  if (compareMap.empty())
  {
    compareMap["JPetTimeWindow"] = FileTypes::fTimeWindow;
    compareMap["JPetRawSignal"] = FileTypes::fRawSignal;
    compareMap["JPetHit"] = FileTypes::fHit;
    compareMap["JPetEvent"] = FileTypes::fEvent;
  }
  bool r = fReader.openFileAndLoadData(filename);
  dynamic_cast< JPetParamBank * >(fReader.getObjectFromFile(
      "ParamBank")); // just read param bank, no need to save it to variable
  fNumberOfEventsInFile = fReader.getNbOfAllEvents();
  if (r)
  {
    TTree *fTree = dynamic_cast< TTree * >(fReader.getObjectFromFile("tree"));
    TObjArray *arr = fTree->GetListOfBranches();
    TBranch *fBranch = dynamic_cast< TBranch * >(arr->At(0));
    const char *branchName = fBranch->GetClassName();
    ProcessedData::getInstance().setCurrentFileType(
        static_cast< FileTypes >(compareMap[branchName]));
  }
  return r;
}

void DataProcessor::closeFile() { fReader.closeFile(); }

bool DataProcessor::nextEvent() { return fReader.nextEvent(); }

bool DataProcessor::firstEvent() { return fReader.firstEvent(); }

bool DataProcessor::lastEvent() { return fReader.lastEvent(); }

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
