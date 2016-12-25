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
 *  @file TreeReader.cpp
 */

#include "TreeReader.h"
#include "LoggerInclude.h"
#include <cassert>


TreeReader::TreeReader()
{
  /**/
}

TreeReader::TreeReader(const char* p_filename)
{
  if (!openFileAndLoadData(p_filename, "tree")) {
    ERROR("error in opening file");
  }
}

TreeReader::~TreeReader()
{
  if (fFile) {
    delete fFile;
    fFile = 0;
  }
}

TObject& TreeReader::getCurrentEvent()
{
  if (loadCurrentEvent()) {
    return *fEvent;
  } else {
    ERROR("Could not read the current event");
    if (fEvent) {
      delete fEvent;
    }
    fEvent = new TNamed("Empty event", "Empty event");
  }
  return *fEvent;
}

bool TreeReader::TreeReader::nextEvent()
{
  fCurrentEventNumber++;
  return loadCurrentEvent();
}

bool TreeReader::firstEvent()
{
  fCurrentEventNumber = 0;
  return loadCurrentEvent();
}

bool TreeReader::lastEvent()
{
  fCurrentEventNumber = getNbOfAllEvents() - 1;
  return loadCurrentEvent();
}

bool TreeReader::nthEvent(int n)
{
  fCurrentEventNumber = n;
  return loadCurrentEvent();
}

void TreeReader::closeFile ()
{
  if (fFile) delete fFile;
  fFile = 0;
  fBranch = 0;
  fEvent = 0;
  fTree = 0;
  fCurrentEventNumber = -1;
}


bool TreeReader::openFile (const char* filename)
{
  closeFile();
  fFile = new TFile(filename);
  if ((!isOpen()) || fFile->IsZombie()) {
    ERROR(std::string("Cannot open file:") + std::string(filename));
    return false;
  }
  return true;
}

bool TreeReader::loadData(const char* treename)
{
  if (!isOpen() ) {
    ERROR("File not open");
    return false;
  }
  if (!treename) {
    ERROR("empty tree name");
    return false;
  }
  fTree = static_cast<TTree*>(fFile->Get(treename));
  if (!fTree) {
    ERROR("in reading tree");
    return false;
  }
  TObjArray* arr = fTree->GetListOfBranches();
  fBranch = (TBranch*)(arr->At(0));
  if (!fBranch) {
    ERROR("in reading branch from tree");
    return false;
  }
  fBranch->SetAddress(&fEvent);
  firstEvent();
  return true;
}
