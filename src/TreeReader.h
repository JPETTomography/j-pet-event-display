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
 *  @file TreeReader.h
 */

#ifndef TREEREADER_H
#define TREEREADER_H

#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <vector>

#ifndef __CINT__
#include <boost/noncopyable.hpp>
#else
namespace boost;
class boost::noncopyable;
#endif /* __CINT __ */



/**
 * @brief A class responsible for reading any data from ROOT trees.
 *
 * Based on the JPetReader class from j-pet-framework project
 */
class TreeReader : private boost::noncopyable
{
public:
  TreeReader(void); //maybe remove this one
  explicit TreeReader(const char* p_filename);
  virtual ~TreeReader(void);

  TObject& getCurrentEvent();
  bool nextEvent();
  bool firstEvent();
  bool lastEvent();
  bool nthEvent(int n);
  long long getCurrentEventNumber() const {
    return fCurrentEventNumber;
  }
  virtual long long getNbOfAllEvents() const {
    return fTree ? fTree->GetEntries() : 0;
  }

  virtual bool openFileAndLoadData(const char* filename, const char* treename = "tree") {
    if (openFile(filename) ) {
      return loadData(treename);
    }
    return false;
  }
  virtual void closeFile();

  virtual TObject* getObjectFromFile(const char* name) {
    if (fFile) return fFile->Get(name);
    else return 0;
  }
  virtual bool isOpen() const {
    if (fFile) return (fFile->IsOpen() && !fFile->IsZombie());
    else return false;
  }

protected:
  virtual bool openFile(const char* filename);
  virtual bool loadData(const char* treename = "tree");
  bool loadCurrentEvent() {
    if (fTree) {
      int entryCode = fTree->GetEntry(fCurrentEventNumber);
      return isCorrectTreeEntryCode(entryCode);
    }
    return false;
  }

  inline bool isCorrectTreeEntryCode (int entryCode) const { ///see TTree GetEntry method
    if (entryCode == -1) return false;
    if (entryCode == 0) return false;
    return true;
  }

  TBranch* fBranch = 0;
  TObject* fEvent = 0;
  TTree* fTree = 0;
  TFile* fFile = 0;
  long long fCurrentEventNumber = -1.;
};

#endif	// TREEREADER_H
