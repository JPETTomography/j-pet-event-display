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

ScintillatorsInLayers DataProcessor::getActiveScintillators()
{
  ScintillatorsInLayers selection;
  switch(fCurrentFileType)
  {
    case FileTypes::fTimeWindow :
      selection = getActiveScintillators(
          dynamic_cast<JPetTimeWindow &>(fReader.getCurrentEvent()));
      break;
    case FileTypes::fRawSignal :
      selection = getActiveScintillators(
          dynamic_cast<JPetRawSignal &>(fReader.getCurrentEvent()));
      break;
    default:
      break;
  }

  return selection;
}

ScintillatorsInLayers DataProcessor::getActiveScintillators(const JPetTimeWindow& tWindow)
{
  auto sigChannels = tWindow.getSigChVect();
  auto mappedScins = fMapper->getTOMBMapping();

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

  std::ostringstream oss;
  for (auto iter = selection.begin(); iter != selection.end(); ++iter) {
    int layer = iter->first - 1; // table start form 0, layers from 1
    const std::vector<int> &strips = iter->second;
    for (auto stripIter = strips.begin(); stripIter != strips.end();
         ++stripIter) {
      oss << "layer: " << layer << " scin: " << *stripIter << "\n";
    }
  }
  activedScintilators = oss.str();
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

  std::ostringstream oss;
  for (auto iter = selection.begin(); iter != selection.end(); ++iter) {
    int layer = iter->first; // table start form 0, layers from 1
    const std::vector<int> &strips = iter->second;
    for (auto stripIter = strips.begin(); stripIter != strips.end();
         ++stripIter) {
      oss << "layer: " << layer << " scin: " << *stripIter << "\n";
    }
  }
  activedScintilators = oss.str();
  return selection;
}

DiagramDataMap DataProcessor::getDataForDiagram() 
{ 
  DiagramDataMap data;
  if (fCurrentFileType == FileTypes::fRawSignal)
    data = getDataForDiagram(
        dynamic_cast<JPetRawSignal &>(fReader.getCurrentEvent()));

  return data;
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
        fCurrentFileType = FileTypes::fTimeWindow;
        break;
      case FileTypes::fRawSignal:
        fCurrentFileType = FileTypes::fRawSignal;
        break;
      default:
        fCurrentFileType = FileTypes::fNone;
        break;
    }
    //set mapper
    fMapper = std::unique_ptr<JPetGeomMapping>(new JPetGeomMapping(
        dynamic_cast<JPetParamBank *>(fReader.getObjectFromFile("ParamBank"))));
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
    return fReader.nthEvent(n);
  else
    return false;
}

std::string DataProcessor::getDataInfo() { return activedScintilators; }
}
