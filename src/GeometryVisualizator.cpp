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

GeometryVisualizator::GeometryVisualizator(
    const int numberOfLayers, const int kLength,
    const std::vector<std::pair<int, double>> &layerStats)
{
  createGeometry(numberOfLayers, kLength, layerStats);
}

GeometryVisualizator::~GeometryVisualizator() {}

void GeometryVisualizator::drawData()
{
  drawStrips(ProcessedData::getInstance().getActivedScintilators());
  drawDiagram(ProcessedData::getInstance().getDiagramData());

  updateCanvas(fCanvas3d);
  updateCanvas(fCanvas2d);
  updateCanvas(fCanvasDiagrams);
}

void GeometryVisualizator::updateCanvas(std::unique_ptr<TCanvas>& canvas)
{
  canvas->Draw();
  canvas->Update();
  canvas->Modified();
}

void GeometryVisualizator::showGeometry()
{
  assert(fRootCanvas3d);
  assert(fRootCanvas2d);
  assert(fRootCanvasDiagrams);
  if (!fCanvas3d)
    fCanvas3d = std::unique_ptr<TCanvas>(fRootCanvas3d->GetCanvas());
  if(!fCanvas2d)
    fCanvas2d = std::unique_ptr<TCanvas>(fRootCanvas2d->GetCanvas());
  if(!fCanvasDiagrams)
    fCanvasDiagrams =
        std::unique_ptr<TCanvas>(fRootCanvasDiagrams->GetCanvas());

  draw2dGeometry();
  setAllStripsUnvisible();
  //setAllStripsUnvisible2d();


  fCanvas3d->cd();
  assert(fGeoManager);
  Int_t irep;
  fGeoManager->GetTopVolume()->Draw();
  assert(gPad);
  TView *view = gPad->GetView();
  assert(view);
  view->ZoomView(gPad, 1.75);
  view->SetView(0, 90, 0, irep);
  if(irep == -1)
    WARNING("Error in min-max scope setting view to 3d canvas");

  gPad->Modified();
  gPad->Update();
}

void GeometryVisualizator::draw2dGeometry()
{
  const int marginBetweenScin = 5;
  const int marginBetweenLayers = 10;
  const int topMargin = 10;
  const int bottomMargin = 10;
  const int leftMargin = 20;
  const int rightMargin = 20;
  const int canvasScale = 900;
  //int startX = canvasScale - leftMargin;
  int startY = canvasScale - topMargin;
  int canvasWidth = canvasScale - leftMargin - rightMargin;
  int canvasHeight = canvasScale - topMargin - bottomMargin;
  fCanvas2d->cd();
  fCanvas2d->Range(0, 0, canvasScale, canvasScale);
  TGeoVolume *topVolume = fGeoManager->GetTopVolume();
  numberOfLayers = topVolume->GetNdaughters();
  if (numberOfLayers == 0)
    return;
  numberOfScintilatorsInLayer = new int[numberOfLayers];
  double layerWidth =
      (canvasWidth - (marginBetweenLayers * numberOfLayers)) / numberOfLayers;
  allScintilatorsCanv = new ScintillatorCanv *[numberOfLayers];
  for (int i = 0; i < numberOfLayers; i++)
  {
    TGeoNode *node = topVolume->GetNode(i);
    numberOfScintilatorsInLayer[i] = node->GetNdaughters();
    if (numberOfScintilatorsInLayer[i] == 0)
      return;
    double scintilatorHeight = canvasHeight / numberOfScintilatorsInLayer[i];
    allScintilatorsCanv[i] =
        new ScintillatorCanv[numberOfScintilatorsInLayer[i]];
    for (int j = 0; j < numberOfScintilatorsInLayer[i]; j++)
    {
      allScintilatorsCanv[i][j].image =
          new TBox(leftMargin + (layerWidth * i),
                   startY - ((j + 1) * scintilatorHeight) + marginBetweenScin,
                   leftMargin + (layerWidth * (i + 1)) - marginBetweenLayers,
                   startY - (j * scintilatorHeight));
      allScintilatorsCanv[i][j].image->SetFillColor(1);
      allScintilatorsCanv[i][j].image->Draw();
      // allScintilatorsCanv[i][j].event = new TMarker(0.5, 0.5, 3);
      // allScintilatorsCanv[i][j].event->SetMarkerColor(2);
      // allScintilatorsCanv[i][j].event->SetMarkerSize(3);
      // allScintilatorsCanv[i][j].event->Draw();
    }
  }

  fCanvas2d->Modified();
  fCanvas2d->Update();
}

void GeometryVisualizator::setAllStripsUnvisible2d()
{
  assert(allScintilatorsCanv);
  for (int i = 0; i < numberOfLayers; i++)
  {
    for (int j = 0; j < numberOfScintilatorsInLayer[i]; j++)
    {
      allScintilatorsCanv[i][j].image->SetFillColor(1);
    }
  }
  assert(fCanvas2d);
  fCanvas2d->Modified();
  fCanvas2d->Update();
}

void GeometryVisualizator::setVisibility2d(
    const ScintillatorsInLayers &selection)
{
  for (auto iter = selection.begin(); iter != selection.end(); ++iter)
  {
    int layer = iter->first - 1; // table start form 0, layers from 1
    const std::vector<size_t> &strips = iter->second;
    for (auto stripIter = strips.begin(); stripIter != strips.end();
         ++stripIter)
    {
      int strip = *stripIter - 1;
      if (layer < numberOfLayers && layer >= 0 &&
          strip < numberOfScintilatorsInLayer[layer] && strip >= 0)
      {
        allScintilatorsCanv[layer][strip].image->SetFillColor(2);
      }
    }
  }
  fCanvas2d->Modified();
  fCanvas2d->Update();
}

void GeometryVisualizator::drawStrips(const ScintillatorsInLayers &selection)
{
  setAllStripsUnvisible();
  setAllStripsUnvisible2d();
  if (selection.empty())
    return;
  setVisibility(selection);
  setVisibility2d(selection);
}


void GeometryVisualizator::setVisibility(const ScintillatorsInLayers &selection)
{
  assert(fGeoManager);
  TGeoNode *topNode = fGeoManager->GetTopNode();
  assert(topNode);
  TGeoNode *nodeLayer = 0;
  TGeoNode *nodeStrip = 0;
  int layer = -1;
  int strip = -1;
  for (auto iter = selection.begin(); iter != selection.end(); ++iter)
  {
    layer = iter->first;
    const std::vector<size_t> &strips = iter->second;
    nodeLayer = topNode->GetDaughter(layer - 1);
    assert(nodeLayer);

    for (auto stripIter = strips.begin(); stripIter != strips.end();
         ++stripIter)
    {
      strip = *stripIter;
      nodeStrip = nodeLayer->GetDaughter(strip - 1);
      assert(nodeStrip);
      nodeStrip->SetVisibility(kTRUE);
    }
  }
}

void GeometryVisualizator::setAllStripsUnvisible()
{
  assert(fGeoManager);
  TGeoNodeMatrix *topNode =
      static_cast < TGeoNodeMatrix * >(fGeoManager->GetTopNode());
  assert(topNode);
  int fNumberOfLayers = topNode->GetNdaughters();
  TGeoNode *node = 0;
  for (int i = 0; i < fNumberOfLayers; i++)
  {
    node = topNode->GetDaughter(i);
    assert(node);
    int fNumberOfStrips = node->GetNdaughters();
    for (int j = 0; j < fNumberOfStrips; j++)
    {
      TGeoNode *stripNode = node->GetDaughter(j);
      stripNode->SetVisibility(kFALSE);
    }
  }
}

void GeometryVisualizator::drawDiagram(const DiagramDataMapVector &diagramData)
{
  if (fRootCanvasDiagrams == 0)
  {
    WARNING("Canvas not set");
    return;
  }
  if (fCanvasDiagrams == 0)
    fCanvasDiagrams =
        std::unique_ptr<TCanvas>(fRootCanvasDiagrams->GetCanvas());
  int vectorSize = diagramData.size();
  std::cout << "vector size: " << vectorSize << "\n";
  fCanvasDiagrams->cd();
  fCanvasDiagrams->DivideSquare(vectorSize);
  for (int j = 0; j < vectorSize; j++)
  {
    fCanvasDiagrams->cd(j + 1);
    int n = diagramData[j].size();
    if (n == 0)
      return;
    double x[n], y[n];
    int i = 0;
    for (auto it = diagramData[j].begin(); it != diagramData[j].end(); it++)
    {
      y[i] = static_cast<double>(it->first);
      // y[i] = static_cast<double>(it->second.first);
      x[i] = static_cast<double>(it->second.second);
      // std::cout << "x: " << x[i] << " y: " << y[i] << "\n";
      i++;
    }
    fCanvasDiagrams->cd();
    TGraph *gr = new TGraph(n, x, y);
    gr->GetXaxis()->SetTitle("Time");
    gr->GetYaxis()->SetTitle("Threshold Number");
    gr->Draw("ACP*");
    TLine *line =
        new TLine(gr->GetXaxis()->GetXmin(), 1, gr->GetXaxis()->GetXmax(), 1);
    line->SetLineColor(kRed);
    line->Draw();
  }

}

void GeometryVisualizator::createGeometry(
    const int numberOfLayers, const int kLength,
    const std::vector<std::pair<int, double>> &layerStats)
{
  const double kXWorld = kLength + 100;
  const double kYWorld = kLength + 100;
  const double kZWorld = kLength + 100;

  const double kLayerThickness = 0.9;
  gSystem->Load("libGeom");

  fGeoManager =
      std::unique_ptr<TGeoManager>(new TGeoManager("mgr", "PET detector"));

  TGeoMaterial *matVacuum = new TGeoMaterial("Vacuum", 0, 0, 0);
  assert(matVacuum);
  TGeoMedium *vacuum = new TGeoMedium("Vacuum", 1, matVacuum);
  assert(vacuum);
  TGeoMaterial *matAl = new TGeoMaterial("Al", 0, 0, 0);
  assert(matAl);
  TGeoMedium *medium = new TGeoMedium("Aluminium", 2, matAl);
  assert(medium);

  TGeoVolume *top =
      fGeoManager->MakeBox("TOP", vacuum, kXWorld, kYWorld, kZWorld);
  assert(top);
  fGeoManager->SetTopVolume(top);

  TGeoRotation *rotation = new TGeoRotation();
  rotation->SetAngles(0., 0., 0.);

  std::vector<TGeoTube *> layers;

  for (int i = 0; i < numberOfLayers; i++)
  {
    TGeoTube *layer =
        new TGeoTube(layerStats[i].second,
                     layerStats[i].second + kLayerThickness, kLength);
    assert(layer);
    layers.push_back(layer);
  }

  TGeoVolume *vol = 0;

  int volNr = 0;
  int i = 0;
  char nameOfLayer[100];
  for (TGeoTube *layer : layers)
  {
    sprintf(nameOfLayer, "layer_%d", i + 1);
    vol = new TGeoVolume(nameOfLayer, layer, medium);
    assert(vol);
    vol->SetVisibility(kTRUE);
    vol->SetLineColor(kBlack);

    vol->Divide("XStrip", 2, layerStats[i].first, 0, 0, 1);
    TObjArray *array = vol->GetNodes();
    assert(array);
    TGeoNode *strip = 0;
    TIter iter(array);
    while ((strip = static_cast<TGeoNode *>(iter.Next())) != 0)
    {
      strip->GetVolume()->SetLineColor(kRed);
      strip->SetVisibility(kTRUE);
    }
    volNr = top->GetNtotal() + 1;
    top->AddNode(vol, volNr, rotation);
    i++;
  }
  fGeoManager->GetMasterVolume()->SetVisContainers(kTRUE);
  fGeoManager->CloseGeometry();
  fGeoManager->SetVisLevel(4);
}

}
