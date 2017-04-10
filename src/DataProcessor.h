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
#include <memory>
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
#include <JPetHit/JPetHit.h>
#endif

namespace jpet_event_display
{
enum FileTypes { fNone, fTimeWindow, fRawSignal, fHit };

typedef std::map<size_t, std::vector<size_t> > ScintillatorsInLayers;
typedef std::map<int, std::pair<float, float>> DiagramDataMap;
typedef std::vector<std::map<int, std::pair<float, float>>> DiagramDataMapVector;

class ProcessedData 
{
public:
  ProcessedData(const ProcessedData&) = delete;
  ProcessedData& operator=(const ProcessedData&) = delete;
  ~ProcessedData() { }

  static inline ProcessedData & getInstance() {
    static ProcessedData fSingleton;
    return fSingleton; 
  }

  void setActivedScins(ScintillatorsInLayers scins) { fActivedScins = scins; }
  void setDiagram(DiagramDataMapVector diagram) { fDiagram = diagram; }
  void setCurrentFileType(FileTypes type) { fCurrentFileType = type; }

  const std::string getActivedScintilatorsString()
  {
    std::ostringstream oss;
    for (auto iter = fActivedScins.begin(); iter != fActivedScins.end(); ++iter)
    {
      int layer = iter->first;
      const std::vector<size_t> &strips = iter->second;
      for (auto stripIter = strips.begin(); stripIter != strips.end();
           ++stripIter)
      {
        oss << "layer: " << layer << " scin: " << *stripIter << "\n";
      }
    }
    return oss.str();
  }

  inline FileTypes getCurrentFileType() { return fCurrentFileType; }
  inline ScintillatorsInLayers getActivedScintilators() { return fActivedScins; }
  inline DiagramDataMapVector getDiagramData() { return fDiagram; }
private:
  ProcessedData() { }

  ScintillatorsInLayers fActivedScins;
  DiagramDataMapVector fDiagram;
  FileTypes fCurrentFileType = FileTypes::fNone;

};

class DataProcessor {
public:
  DataProcessor() {}
  void getDataForCurrentEvent();
  bool openFile(const char* filename);
  void closeFile();
  bool firstEvent();
  bool nextEvent();
  bool lastEvent();
  bool nthEvent(long long n);

private:
  #ifndef __CINT__
  DataProcessor(const DataProcessor&) = delete;
  DataProcessor& operator=(const DataProcessor&) = delete;

  ScintillatorsInLayers getActiveScintillators(const JPetTimeWindow& tWindow);
  ScintillatorsInLayers getActiveScintillators(const JPetRawSignal& rawSignal);
  ScintillatorsInLayers getActiveScintillators(const JPetHit &hitSignal);
  DiagramDataMap getDataForDiagram(const JPetRawSignal &rawSignal);
  DiagramDataMapVector getDataForDiagram(const JPetHit &hitSignal);

  template <typename T> const T &getCurrentEvent();

  long long fNumberOfEventsInFile = 0;

  JPetReader fReader;
  std::unique_ptr<JPetGeomMapping> fMapper;
  #endif
};

}

#endif /*  !DATAPROCESSOR_H */
