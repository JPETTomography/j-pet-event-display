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
 *  @brief Class visualizes geometry along with fired strips.
 */

#ifndef GEOMETRYVISUALIZATOR_H_
#define GEOMETRYVISUALIZATOR_H_

#include <TView.h>
#include <TVirtualPad.h>
#include <TGeoManager.h>
#include <TGeoNode.h>
#include <TGeoVolume.h>
#include <TMarker.h>
#include <TBox.h>
#include <TGraph.h>
#include <cassert>
#include <map>
#include <vector>
#include <memory>
#include "./CommonTools.h"


#include <TRootEmbeddedCanvas.h>

class TCanvas;

namespace jpet_event_display
{
  class GeometryVisualizator
  {
  public:
    GeometryVisualizator();
    ~GeometryVisualizator();
    bool isGeoManagerInitialized() const;
    void loadGeometry(const std::string& geomFile);
    void drawOnlyGeometry();
    void draw2dGeometry();
    void drawStrips(const std::map<int, std::vector<int> >& selection);
    void drawPads();
    void setAllStripsUnvisible();
    void setAllStripsUnvisible2d();
    void setVisibility(const std::map<int, std::vector<int> >& selection);
    void setVisibility2d(const std::map<int, std::vector<int> >& selection);
    std::string getLayerNodeName(int layer) const;
    std::string getStripNodeName(int strip) const;
    void drawDiagram(const std::map<int, std::pair<float, float>> &diagramData);

    inline std::unique_ptr<TRootEmbeddedCanvas>& getCanvas3d() { return fRootCanvas3d; }
    inline std::unique_ptr<TRootEmbeddedCanvas>& getCanvas2d() { return fRootCanvas2d; }
    inline std::unique_ptr<TRootEmbeddedCanvas>& getCanvasDiagrams() { return fRootCanvasDiagrams; }

  private:
    enum ColorTable { kBlack = 1, kRed = 2, kBlue = 34, kGreen = 30 };
    #ifndef __CINT__
    std::unique_ptr<TGeoManager> fGeoManager;
    int numberOfLayers = 0;
    int* numberOfScintilatorsInLayer; 
    std::unique_ptr<TRootEmbeddedCanvas> fRootCanvas3d;
    std::unique_ptr<TRootEmbeddedCanvas> fRootCanvas2d;
    std::unique_ptr<TRootEmbeddedCanvas> fRootCanvasDiagrams;
    std::unique_ptr<TCanvas> fCanvas3d;
    std::unique_ptr<TCanvas> fCanvas2d;
    std::unique_ptr<TCanvas> fCanvasDiagrams;
    #endif
    struct ScintillatorCanv {
      TBox* image;
      TMarker* event;
    } fScintCanv;
    ScintillatorCanv fScintCanv2;
    ScintillatorCanv** allScintilatorsCanv;
  };

}
#endif  // GEOMETRYVISUALIZATOR_H_
