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
 *  @file GeometryVisualizator.h
 *  @brief Class visualizes geometry along with fired strips.
 */

#ifndef GEOMETRYVISUALIZATOR_H_
#define GEOMETRYVISUALIZATOR_H_

#include <TView.h>
#include <TVirtualPad.h>
#include <TGeoManager.h>
#include <TGeoNode.h>
#include <TGeoVolume.h>
#include <cassert>
#include <map>
#include <vector>
#include "./CommonTools.h"

class TCanvas;

namespace jpet_event_display
{


class GeometryVisualizator
{
public:
  GeometryVisualizator(TCanvas* canv);
  ~GeometryVisualizator();
  bool isGeoManagerInitialized() const;
  void loadGeometry(TString geomFile);
  void drawOnlyGeometry();
  void drawStrips(const std::map<int, std::vector<int> >& selection);
  void drawPads();
  void setAllStripsUnvisible();
  void setVisibility(const std::map<int, std::vector<int> >& selection);
  std::string getLayerNodeName(int layer) const;
  std::string getStripNodeName(int strip) const;

private:
  enum ColorTable { kBlack = 1, kRed = 2, kBlue = 34, kGreen = 30};
  TGeoManager* fGeoManager;
  TCanvas* fMyCanv;

};

}
#endif  // GEOMETRYVISUALIZATOR_H_
