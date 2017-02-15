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
#include <vector>
#ifndef __CINT__
#include <JPetTimeWindow/JPetTimeWindow.h>
#include <JPetReader/JPetReader.h>
#include <JPetParamBank/JPetParamBank.h>
#endif

namespace jpet_event_display
{

typedef std::map<int, std::vector<int> > ScintillatorsInLayers;

class DataProcessor
{
public:
  DataProcessor() {}
  /// this method should probably be in some other class
  static ScintillatorsInLayers getActiveScintillators(const JPetTimeWindow& tWindow);
  static ScintillatorsInLayers getActiveScintillators(const JPetParamBank* pBank);

  bool openFile(const char* filename);
  void closeFile();
  JPetTimeWindow& getCurrentEvent();
  JPetParamBank* getParamBank();
  bool nextEvent();

private:
  #ifndef __CINT__
  DataProcessor(const DataProcessor&) = delete;
  DataProcessor& operator=(const DataProcessor&) = delete;
  

  JPetReader fReader;
  #endif
};

}

#endif /*  !DATAPROCESSOR_H */
