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
    ProcessedData::getInstance().addToInfo(currentActivedScintillatorsInfo());
    break;
  case FileTypes::fRawSignal:
    getActiveScintillators(getCurrentEvent< JPetRawSignal >());
    getDataForDiagram(getCurrentEvent< JPetRawSignal >());
    ProcessedData::getInstance().addToInfo(currentActivedScintillatorsInfo());
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

std::string DataProcessor::currentActivedScintillatorsInfo()
{
  std::ostringstream oss;
  for (auto iter =
           ProcessedData::getInstance().getActivedScintilators().begin();
       iter != ProcessedData::getInstance().getActivedScintilators().end();
       ++iter)
  {
    int layer = iter->first;
    const std::vector< size_t > &strips = iter->second;
    for (auto stripIter = strips.begin(); stripIter != strips.end();
         ++stripIter)
    {
      oss << "layer: " << layer << " scin: " << *stripIter << "\n";
    }
  }
  return oss.str();
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
  StripPos pos = fMapper->getStripPos(
      rawSignal.getPoints(JPetSigCh::Leading)[0].getPM().getBarrelSlot());
  auto data = rawSignal.getTimesVsThresholdValue(JPetSigCh::Leading);
  float startPos = data.begin()->second.second;
  float diff[4];
  int i = 0;
  for (auto it = data.begin(); it != data.end(); it++)
  {
    diff[i] = startPos - it->second.second;
    if (diff[i] > 0)
      diff[i] = -diff[i];
    r.push_back(std::make_tuple(
        it->first, it->second.first, 0, JPetSigCh::Leading,
        rawSignal.getPoints(JPetSigCh::Leading)[0].getPM().getSide(), pos.layer,
        pos.slot));
    i++;
  }
  i = 0;
  auto tmp = rawSignal.getTimesVsThresholdValue(JPetSigCh::Trailing);
  for (auto it = tmp.begin(); it != tmp.end(); it++)
  {
    r.push_back(std::make_tuple(
        it->first, it->second.first, it->second.second - diff[i] - startPos,
        JPetSigCh::Trailing,
        rawSignal.getPoints(JPetSigCh::Trailing)[0].getPM().getSide(),
        pos.layer, pos.slot));
    i++;
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

  StripPos pos = fMapper->getStripPos(hitSignal.getSignalA()
                                          .getRecoSignal()
                                          .getRawSignal()
                                          .getPoints(JPetSigCh::Leading)[0]
                                          .getPM()
                                          .getBarrelSlot());

  std::ostringstream oss;
  oss << "layer: " << pos.layer << " scin: " << pos.slot
      << " x: " << hitSignal.getPosX() << " y: " << hitSignal.getPosY()
      << " z: " << hitSignal.getPosZ() << "\n";
  ProcessedData::getInstance().addToInfo(oss.str());
}

void DataProcessor::getDataForDiagram(const JPetEvent &event)
{
  DiagramDataMapVector rv;
  if (event.getHits().size() < 1)
    return;
  for (JPetHit hit : event.getHits())
  {
    rv.push_back(getDataForDiagram(
        hit.getSignalA().getRecoSignal().getRawSignal(), true));
    rv.push_back(getDataForDiagram(
        hit.getSignalB().getRecoSignal().getRawSignal(), true));

    StripPos pos = fMapper->getStripPos(hit.getSignalA()
                                            .getRecoSignal()
                                            .getRawSignal()
                                            .getPoints(JPetSigCh::Leading)[0]
                                            .getPM()
                                            .getBarrelSlot());
    double r =
        sqrt(hit.getPosX() * hit.getPosX() + hit.getPosY() * hit.getPosY());
    double fi = hit.getPosX() >= 0 ? asin(hit.getPosY() / r)
                                   : -asin(hit.getPosY() / r) + M_PI;
    std::ostringstream oss;
    oss << "layer: " << pos.layer << " scin: " << pos.slot
        << " x: " << hit.getPosX() << " y: " << hit.getPosY()
        << " z: " << hit.getPosZ() << "\n"
        << "r: " << r << " theta: " << (fi * 180.0) / M_PI << "\n";
    ProcessedData::getInstance().addToInfo(oss.str());
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
