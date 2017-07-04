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
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#ifndef __CINT__
#include <JPetGeomMapping/JPetGeomMapping.h>
#include <JPetGeomMappingInterface/JPetGeomMappingInterface.h>
#include <JPetHit/JPetHit.h>
#include <JPetPM/JPetPM.h>
#include <JPetParamBank/JPetParamBank.h>
#include <JPetParamGetterAscii/JPetParamGetterAscii.h>
#include <JPetParamManager/JPetParamManager.h>
#include <JPetRawSignal/JPetRawSignal.h>
#include <JPetReader/JPetReader.h>
#include <JPetTimeWindow/JPetTimeWindow.h>
#include <JPetTreeHeader/JPetTreeHeader.h>
#endif

#include <TNamed.h>
#include <TVector3.h>

namespace jpet_event_display
{
enum FileTypes
{
  fNone,
  fTimeWindow,
  fRawSignal,
  fHit,
  fEvent
};

typedef std::map< size_t, std::vector< size_t > >
    ScintillatorsInLayers; // layer, scinID, hitPos
typedef std::vector< std::tuple< int, float, float, JPetSigCh::EdgeType,
                                 JPetPM::Side, size_t, size_t > >
    DiagramDataMap; // threshold number, thresholdValue, time, EdgeType, Side,
                    // layer, scin
// typedef std::map<int, std::tuple<float, float, JPetSigCh::EdgeType>>
//    DiagramDataMap;
typedef std::vector< DiagramDataMap > DiagramDataMapVector;
typedef std::vector< TVector3 > HitPositions; // rename

class ProcessedData
{
public:
  ProcessedData(const ProcessedData &) = delete;
  ProcessedData &operator=(const ProcessedData &) = delete;
  ~ProcessedData() {}

  static inline ProcessedData &getInstance()
  {
    static ProcessedData fSingleton;
    return fSingleton;
  }

  void clearData()
  {
    fActivedScins.clear();
    fDiagram.clear();
    fHits.clear();
    fInfo.clear();
  }

  void setActivedScins(ScintillatorsInLayers scins) { fActivedScins = scins; }
  void setDiagram(DiagramDataMapVector diagram) { fDiagram = diagram; }
  void setCurrentFileType(FileTypes type) { fCurrentFileType = type; }
  void setHits(HitPositions hits) { fHits = hits; }

  inline FileTypes getCurrentFileType() { return fCurrentFileType; }
  inline ScintillatorsInLayers &getActivedScintilators()
  {
    return fActivedScins;
  }
  inline DiagramDataMapVector &getDiagramData() { return fDiagram; }
  inline HitPositions &getHits() { return fHits; }

  inline void addToInfo(const std::string &str) { fInfo += str; }

  const std::string &getInfo() { return fInfo; }

private:
  ProcessedData() {}

  std::string fInfo;
  ScintillatorsInLayers fActivedScins;
  DiagramDataMapVector fDiagram;
  HitPositions fHits; // rename
  FileTypes fCurrentFileType = FileTypes::fNone;
};

class DataProcessor
{
public:
  DataProcessor(const std::string paramGetterAnsiiPath, const int runNumber);
  void getDataForCurrentEvent();
  bool openFile(const char *filename);
  void closeFile();
  bool firstEvent();
  bool nextEvent();
  bool lastEvent();
  bool nthEvent(long long n);
  long long getNumberOfEvents() { return fNumberOfEventsInFile; }

private:
#ifndef __CINT__
  DataProcessor(const DataProcessor &) = delete;
  DataProcessor &operator=(const DataProcessor &) = delete;

  void addToSelectionIfNotPresent(ScintillatorsInLayers &selection,
                                  StripPos &pos);

  void getActiveScintillators(const JPetTimeWindow &tWindow);
  void getActiveScintillators(const JPetRawSignal &rawSignal);
  void getActiveScintillators(const JPetHit &hitSignal);
  void getActiveScintillators(const JPetEvent &event);
  std::string currentActivedScintillatorsInfo();

  DiagramDataMap getDataForDiagram(const JPetRawSignal &rawSignal, bool);
  // void getDataForDiagram(const JPetTimeWindow &tWindow);
  void getDataForDiagram(const JPetRawSignal &rawSignal);
  void getDataForDiagram(const JPetHit &hitSignal);
  void getDataForDiagram(const JPetEvent &event);

  void getHitsPosition(const JPetHit &hitSignal);
  void getHitsPosition(const JPetEvent &event);

  template < typename T > const T &getCurrentEvent();

  long long fNumberOfEventsInFile = 0;

  JPetReader fReader;
  std::unique_ptr< JPetGeomMapping > fMapper;
#endif
};
}

#endif /*  !DATAPROCESSOR_H */
