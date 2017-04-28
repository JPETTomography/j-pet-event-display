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

#include "./CommonTools.h"
#include <TAxis.h>
#include <TBox.h>
#include <TGeoManager.h>
#include <TGeoNode.h>
#include <TGeoTube.h>
#include <TGeoVolume.h>
#include <TGraph.h>
#include <TLine.h>
#include <TMarker.h>
#include <TMultiGraph.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TView.h>
#include <TVirtualPad.h>
#include <TPoint.h>
#include <TPolyMarker3D.h>
#include <TEllipse.h>
#include <TGaxis.h>
#include <cassert>
#include <map>
#include <memory>
#include <vector>

#include "DataProcessor.h"


#include <TRootEmbeddedCanvas.h>

class TCanvas;

namespace jpet_event_display
{
  class GeometryVisualizator
  {
  public:
    GeometryVisualizator(const int numberOfLayers, const int kLength,
                         const std::vector<std::pair<int, double>> &layerStats);
    ~GeometryVisualizator();
    
    void showGeometry();
    void drawData();

    inline std::unique_ptr<TRootEmbeddedCanvas>& getCanvas3d() { return fRootCanvas3d; }
    inline std::unique_ptr<TRootEmbeddedCanvas>& getCanvas2d() { return fRootCanvas2d; }
    inline std::unique_ptr<TRootEmbeddedCanvas>& getCanvas2d2() { return fRootCanvas2d2; }
    inline std::unique_ptr<TRootEmbeddedCanvas>& getCanvasDiagrams() { return fRootCanvasDiagrams; }

  private: 


#ifndef __CINT__
    void createGeometry(const int numberOfLayers, const int kLength,
                        const std::vector<std::pair<int, double>> &layerStats);

    void updateCanvas(std::unique_ptr<TCanvas> &canvas);

    void draw2dGeometry();
    void drawStrips(const ScintillatorsInLayers& selection);
    void drawPads();
    void setAllStripsUnvisible();
    void setAllStripsUnvisible2d();
    void setVisibility(const ScintillatorsInLayers& selection);
    void setVisibility2d(const ScintillatorsInLayers& selection);

    void drawLineBetweenActivedScins(const HitPositions &pos);

    void drawDiagram(const DiagramDataMapVector &diagramData);

    void draw2dGeometry2();

    enum ColorTable
    {
      kBlack = 1,
      kRed = 2,
      kBlue = 34,
      kGreen = 30
    };
    std::unique_ptr<TGeoManager> fGeoManager;
    int numberOfLayers = 0;
    int* numberOfScintilatorsInLayer; 
    std::unique_ptr<TRootEmbeddedCanvas> fRootCanvas3d;
    std::unique_ptr<TRootEmbeddedCanvas> fRootCanvas2d;
    std::unique_ptr<TRootEmbeddedCanvas> fRootCanvas2d2;
    std::unique_ptr<TRootEmbeddedCanvas> fRootCanvasDiagrams;
    std::unique_ptr<TCanvas> fCanvas3d;
    std::unique_ptr<TCanvas> fCanvas2d;
    std::unique_ptr<TCanvas> fCanvas2d2;
    std::unique_ptr<TCanvas> fCanvasDiagrams;

    int fScinLenghtWithoutScale = 0;

    int fLastDiagramVectorSize = 0;
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
