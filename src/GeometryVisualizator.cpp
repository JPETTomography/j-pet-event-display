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
 *  @file GeometryVisualizator.cpp
 */

#include "GeometryVisualizator.h"
#include <JPetLoggerInclude.h>
#include <TCanvas.h>
#include <TFile.h>

#include <TPolyLine3D.h>
#include <TRandom.h>
#include <memory>

namespace jpet_event_display
{

GeometryVisualizator::GeometryVisualizator() { }

  GeometryVisualizator::~GeometryVisualizator() { }

  void GeometryVisualizator::loadGeometry(const std::string& geomFile)
  {
    std::shared_ptr<TFile> inputGeomFile = std::make_shared<TFile>(static_cast<TString>(geomFile));
    if (inputGeomFile->IsZombie()) {
      assert(1 == 0);
      ERROR(std::string("Error opening file:" + geomFile));
      return;
    }
    fGeoManager = std::unique_ptr<TGeoManager>(static_cast<TGeoManager*>(inputGeomFile->Get("mgr")));
    assert(fGeoManager);
  }

  void GeometryVisualizator::drawOnlyGeometry()
  {
    if (fRootCanvas3d == 0) {
      WARNING("Canvas not set");
      return;
    }
    if(fCanvas3d == 0)
      fCanvas3d = std::unique_ptr<TCanvas>(fRootCanvas3d->GetCanvas());
    setAllStripsUnvisible();
    //fGeoManager->GetTopVolume()->Draw();
    drawPads();
    fGeoManager->SetVisLevel(4);
    fGeoManager->SetVisOption(0);
    fCanvas3d->Modified();
    fCanvas3d->Update();
    draw2dGeometry();
  }

  void GeometryVisualizator::draw2dGeometry()
  {
    if (fRootCanvas2d == 0) {
      WARNING("Canvas not set");
      return;
    }
    if(fCanvas2d == 0)
      fCanvas2d = std::unique_ptr<TCanvas>(fRootCanvas2d->GetCanvas());
    const int marginBetweenScin = 5;
    const int marginBetweenLayers = 10;
    const int topMargin = 10;
    const int bottomMargin = 10;
    const int leftMargin = 20;
    const int rightMargin = 20;
    const int canvasScale = 900;
    int startX = canvasScale - leftMargin;
    int startY = canvasScale - topMargin;
    int canvasWidth = canvasScale - leftMargin - rightMargin;
    int canvasHeight = canvasScale - topMargin - bottomMargin;
    fCanvas2d->cd();
    fCanvas2d->Range(0, 0, canvasScale, canvasScale);
    TGeoVolume* topVolume = fGeoManager->GetTopVolume();
    numberOfLayers = topVolume->GetNdaughters();
    numberOfScintilatorsInLayer = new int[numberOfLayers];
    if(numberOfLayers == 0)
      return;
    double layerWidth = (canvasWidth - (marginBetweenLayers * numberOfLayers)) / numberOfLayers;
    allScintilatorsCanv = new ScintillatorCanv *[numberOfLayers];
    for (int i = 0; i < numberOfLayers; i++) {
      TGeoNode* node = topVolume->GetNode(i);
      numberOfScintilatorsInLayer[i] = node->GetNdaughters();
      double scintilatorHeight = canvasHeight / numberOfScintilatorsInLayer[i];
      allScintilatorsCanv[i] =
          new ScintillatorCanv[numberOfScintilatorsInLayer[i]];
      for (int j = 0; j < numberOfScintilatorsInLayer[i]; j++) {
        allScintilatorsCanv[i][j].image = new TBox(
            leftMargin + (layerWidth * i), 
            startY - ((j + 1) * scintilatorHeight) + marginBetweenScin, 
            leftMargin + (layerWidth * (i + 1)) - marginBetweenLayers, 
            startY - (j * scintilatorHeight)
            );
        allScintilatorsCanv[i][j].image->SetFillColor(1);
        allScintilatorsCanv[i][j].image->Draw();
        //allScintilatorsCanv[i][j].event = new TMarker(0.5, 0.5, 3);
        //allScintilatorsCanv[i][j].event->SetMarkerColor(2);
        //allScintilatorsCanv[i][j].event->SetMarkerSize(3);
        //allScintilatorsCanv[i][j].event->Draw();
      }
    }

    fCanvas2d->Modified();
    fCanvas2d->Update();
  }

  void GeometryVisualizator::setAllStripsUnvisible2d()
  {
    assert(allScintilatorsCanv);
    for(int i = 0; i < numberOfLayers; i++)
    {
      for(int j = 0; j < numberOfScintilatorsInLayer[i]; j++)
      {
        allScintilatorsCanv[i][j].image->SetFillColor(1);
      }
    }
    fCanvas2d->Modified();
    fCanvas2d->Update();
  }

  void GeometryVisualizator::setVisibility2d(const std::map<int, std::vector<int> >& selection)
  {
    for (auto iter = selection.begin(); iter != selection.end(); ++iter) {
      int layer = iter->first - 1; // table start form 0, layers from 1
      const std::vector<int>& strips = iter->second;
      for (auto stripIter = strips.begin(); stripIter != strips.end(); ++stripIter) {
        int strip = *stripIter - 1; // scintilators start from 1
        if (layer < numberOfLayers && layer >= 0 &&
            strip < numberOfScintilatorsInLayer[layer] && strip >= 0) {
          allScintilatorsCanv[layer][strip].image->SetFillColor(2);
        }
      }
    }
    fCanvas2d->Modified();
    fCanvas2d->Update();
  }

  void GeometryVisualizator::drawStrips(const std::map<int, std::vector<int> >& selection)
  {
    if (fCanvas3d == 0) {
      WARNING("Canvas not set");
      return;
    }
    setVisibility(selection);
    drawPads();
  }


  void GeometryVisualizator::drawPads()
  {
    if (fCanvas3d == 0) {
      WARNING("Canvas not set");
      return;
    }
    fCanvas3d->cd(0);
    assert(fGeoManager);
    Int_t irep;
    fGeoManager->GetTopVolume()->Draw();
    assert(gPad);
    TView* view = gPad->GetView();
    assert(view);
    view->ZoomView(0, 1);
    view->SetView(0, 0 , 0, irep);
    
    gPad->Modified();
    gPad->Update();
  }

  void GeometryVisualizator::setVisibility(const std::map<int, std::vector<int> >& selection)
  {
    setAllStripsUnvisible();
    setAllStripsUnvisible2d();
    setVisibility2d(selection);
    if (selection.empty()) return;
    assert(fGeoManager);
    TGeoNode *topNode = fGeoManager->GetTopNode();
    assert(topNode);
    TGeoNode* nodeLayer = 0; //making those shared ptr causing segfault
    TGeoNode* nodeStrip = 0;
    
    std::map<int, std::vector<int> >::const_iterator iter;
    int layer = -1;
    int strip = -1;
    std::string nodeName;
    for (iter = selection.begin(); iter != selection.end(); ++iter) {
      layer = iter->first;
      const std::vector<int>& strips = iter->second;
      std::vector<int>::const_iterator stripIter;
      nodeName = getLayerNodeName(layer + 1); /// layer numbers starts from 1
      nodeLayer = topNode->GetVolume()->FindNode(nodeName.c_str());
      if(!nodeLayer) //TODO remove, just for tests
        return;
      
      assert(nodeLayer);
      for (stripIter = strips.begin(); stripIter != strips.end(); ++stripIter) {
        strip = *stripIter;
        nodeName = getStripNodeName(strip + 1); /// strips numbers starts from ?
        nodeStrip = nodeLayer->GetVolume()->FindNode(nodeName.c_str());
        if(!nodeStrip) //TODO remove, just for tests
          return;
        assert(nodeStrip);
        nodeStrip->SetVisibility(kTRUE);
      }
    }
  }

  void GeometryVisualizator::setAllStripsUnvisible()
  {
    assert(fGeoManager);
    TGeoNode* topNode = fGeoManager->GetTopNode(); //making this shared ptr causing segfault
    assert(topNode);
    TGeoNode* node = 0;
    node = topNode->GetVolume()->FindNode("layer_1_1");
    assert(node);
    node->SetVisibility(kTRUE);
    node->GetVolume()->SetLineColor(kBlack);
    TObjArray* array = node->GetNodes();
    assert(array);
    TIter it(array);
    while ((node = (TGeoNode*)it.Next())) {
      node->SetVisibility(kFALSE);
      node->GetVolume()->SetLineWidth(5);
    }

    node = topNode->GetVolume()->FindNode("layer_2_1");
    assert(node);
    node->SetVisibility(kTRUE);
    node->GetVolume()->SetLineColor(kBlack);
    array = node->GetNodes();
    assert(array);
    it  = array->MakeIterator();
    while ((node = (TGeoNode*)it.Next())) {
      node->SetVisibility(kFALSE);
      node->GetVolume()->SetLineWidth(5);
    }

    node = topNode->GetVolume()->FindNode("layer_3_1");
    assert(node);
    node->SetVisibility(kTRUE);
    node->GetVolume()->SetLineColor(kBlack);
    array = node->GetNodes();
    assert(array);
    it  = array->MakeIterator();
    while ((node = (TGeoNode*)it.Next())) {
      node->SetVisibility(kFALSE);
      node->GetVolume()->SetLineWidth(5);
    }
  }

  bool GeometryVisualizator::isGeoManagerInitialized() const
  {
    if (fGeoManager == 0) {
      WARNING("GEOMETRY is not loaded.");
      return false;
    } else {
      return true;
    }
  }

  std::string GeometryVisualizator::getLayerNodeName(int layer) const
  {
    std::string name("layer_");
    name.append(CommonTools::intToString(layer) + std::string("_1"));
    return name;
  }

  std::string GeometryVisualizator::getStripNodeName(int strip) const
  {
    std::string name("XStrip_");
    name.append(CommonTools::intToString(strip));
    return name;
  }

  void GeometryVisualizator::drawDiagram(const std::map<int, std::pair<float, float>>& diagramData)
  {
    if (fRootCanvasDiagrams == 0) {
      WARNING("Canvas not set");
      return;
    }
    if(fCanvasDiagrams == 0)
      fCanvasDiagrams = std::unique_ptr<TCanvas>(fRootCanvasDiagrams->GetCanvas());
    int n = diagramData.size();
    if(n == 0)
      return;
    double x[n], y[n];
    int i = 0;
    for (auto it = diagramData.begin(); it != diagramData.end(); it++) {
      y[i] = static_cast<double>(it->first);
      // y[i] = static_cast<double>(it->second.first);
      x[i] = static_cast<double>(it->second.second);
      //std::cout << "x: " << x[i] << " y: " << y[i] << "\n";
      i++;
    }
    fCanvasDiagrams->cd();
    TGraph *gr = new TGraph(n, x, y);
    gr->Draw("ACP*");
    fCanvasDiagrams->Update();
    fCanvasDiagrams->Modified();
  }
}
