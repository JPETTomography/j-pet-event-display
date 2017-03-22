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

#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

#include <map>
#include <sstream> //TO delete
#include <string>
#include <vector>
#ifndef __CINT__
#include <JPetGeomMapping/JPetGeomMapping.h>
#include <JPetGeomMappingInterface/JPetGeomMappingInterface.h>
#include <JPetParamGetterAscii/JPetParamGetterAscii.h>
#include <JPetParamManager/JPetParamManager.h>
#include <JPetParamBank/JPetParamBank.h>
#include <JPetRawSignal/JPetRawSignal.h>
#include <JPetReader/JPetReader.h>
#include <JPetTimeWindow/JPetTimeWindow.h>
#include <JPetTreeHeader/JPetTreeHeader.h>
#endif

namespace jpet_event_display
{

typedef std::map<int, std::vector<int> > ScintillatorsInLayers;
typedef std::map<int, std::pair<float, float>> DiagramDataMap;

class DataProcessor {
public:
  DataProcessor() {}
  enum FileTypes { fNone, fTimeWindow, fRawSignal };
  /// this method should probably be in some other class
  ScintillatorsInLayers getActiveScintillators();
  ScintillatorsInLayers getActiveScintillators(const JPetTimeWindow& tWindow);
  ScintillatorsInLayers getActiveScintillators(const JPetRawSignal& rawSignal);
  DiagramDataMap getDataForDiagram();
  DiagramDataMap getDataForDiagram(const JPetRawSignal &rawSignal);

  bool openFile(const char* filename);
  void closeFile();
  bool firstEvent();
  bool nextEvent();
  bool lastEvent();
  bool nthEvent(long long n);

  inline FileTypes getCurrentFileType() { return fCurrentFileType; }

  std::string getDataInfo(); // change when imp new mapper

private:
  #ifndef __CINT__
  DataProcessor(const DataProcessor&) = delete;
  DataProcessor& operator=(const DataProcessor&) = delete;

  std::string activedScintilators; // TODO Change tmp workaround

  FileTypes fCurrentFileType = fNone;

  long long fNumberOfEventsInFile = 0;

  JPetReader fReader;
  std::unique_ptr<JPetGeomMapping> fMapper;
  #endif
};

}

#endif /*  !DATAPROCESSOR_H */
