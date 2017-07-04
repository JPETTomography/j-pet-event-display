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
    const std::vector< std::pair< int, double > > &layerStats)
    : fScinLenghtWithoutScale(kLength)
{
  createGeometry(numberOfLayers, kLength, layerStats);
}

GeometryVisualizator::~GeometryVisualizator() {}

void GeometryVisualizator::drawData()
{
  drawStrips(ProcessedData::getInstance().getActivedScintilators());
  drawDiagram(ProcessedData::getInstance().getDiagramData());
  drawLineBetweenActivedScins(ProcessedData::getInstance().getHits());
  drawMarkers(ProcessedData::getInstance().getHits());
  setMarker2d(ProcessedData::getInstance().getHits(),
              ProcessedData::getInstance().getActivedScintilators());

  updateCanvas(fCanvas3d);
  updateCanvas(fCanvas2d);
  updateCanvas(fCanvasTopView);
  updateCanvas(fCanvasDiagrams);
}

void GeometryVisualizator::updateCanvas(std::unique_ptr< TCanvas > &canvas)
{
  canvas->Draw();
  canvas->Update();
  canvas->Modified();
}

void GeometryVisualizator::showGeometry()
{
  assert(fRootCanvas3d);
  assert(fRootCanvas2d);
  assert(fRootCanvasTopView);
  assert(fRootCanvasDiagrams);
  if (!fCanvas3d)
    fCanvas3d = std::unique_ptr< TCanvas >(fRootCanvas3d->GetCanvas());
  if (!fCanvas2d)
    fCanvas2d = std::unique_ptr< TCanvas >(fRootCanvas2d->GetCanvas());
  if (!fCanvasTopView)
    fCanvasTopView =
        std::unique_ptr< TCanvas >(fRootCanvasTopView->GetCanvas());
  if (!fCanvasDiagrams)
    fCanvasDiagrams =
        std::unique_ptr< TCanvas >(fRootCanvasDiagrams->GetCanvas());

  draw2dGeometry();
  draw2dGeometry2();

  fCanvas3d->cd();
  assert(fGeoManager);
  Int_t irep;
  fGeoManager->GetTopVolume()->Draw("ogl");
  assert(gPad);
  TView *view = gPad->GetView();
  assert(view);
  view->ZoomView(gPad, 1.75);
  view->SetView(0, 90, 0,
                irep); // if 3d view is rendered by ROOT 3d view(not openGL),
  // set camera longitude to 0, latitude to 90 and psi to 0, return value is
  // saved in irep
  if (irep == -1)
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
  int startY = canvasScale - topMargin;
  int canvasWidth = canvasScale - leftMargin - rightMargin;
  int canvasHeight = canvasScale - topMargin - bottomMargin;
  fCanvas2d->cd();
  fCanvas2d->Range(0, 0, canvasScale, canvasScale);
  TGeoVolume *topVolume = fGeoManager->GetTopVolume();
  unsigned int numberOfLayers = topVolume->GetNdaughters();
  if (numberOfLayers == 0)
    return;
  fUnRolledViewScintillators.resize(numberOfLayers);
  double layerWidth =
      (canvasWidth - (marginBetweenLayers * numberOfLayers)) / numberOfLayers;
  for (unsigned int i = 0; i < numberOfLayers; i++)
  {
    TGeoNode *node = topVolume->GetNode(i);
    unsigned int numberOfScintillatorsInCurrentLayer = node->GetNdaughters();
    if (numberOfScintillatorsInCurrentLayer == 0)
      return;
    double scintilatorHeight =
        canvasHeight / numberOfScintillatorsInCurrentLayer;
    fUnRolledViewScintillators[i].resize(numberOfScintillatorsInCurrentLayer);
    for (unsigned int j = 0; j < numberOfScintillatorsInCurrentLayer; j++)
    {
      // calculating where TBox should be rendered, first layer starts from
      // left,
      // first scintillator start from top, bottom left corner is defined as:
      // x1 = margin from left + (number of current layer * width of each layer)
      // y1 = (canvasSize - margin from top) - (next scintillator * scintillator
      // height) + margin between scintillators
      // top right corner is defined as:
      // x2 = margin from left + (next scintillator * scintillator height) -
      // margin between layers
      // y2 = (canvasSize - margin from top) - (current scintillator *
      // scintillator height)
      fUnRolledViewScintillators[i][j] =
          new TBox(leftMargin + (layerWidth * i),
                   startY - ((j + 1) * scintilatorHeight) + marginBetweenScin,
                   leftMargin + (layerWidth * (i + 1)) - marginBetweenLayers,
                   startY - (j * scintilatorHeight));
      fUnRolledViewScintillators[i][j]->SetFillColor(1);
      fUnRolledViewScintillators[i][j]->Draw();
    }
  }

  fCanvas2d->Modified();
  fCanvas2d->Update();
}

void GeometryVisualizator::setAllStripsUnvisible2d()
{
  for (unsigned int i = 0; i < fUnRolledViewScintillators.size(); i++)
  {
    for (unsigned int j = 0; j < fUnRolledViewScintillators[i].size(); j++)
    {
      fUnRolledViewScintillators[i][j]->SetFillColor(1);
    }
  }
  fCanvas2d->Modified();
  fCanvas2d->Update();
}

void GeometryVisualizator::setMarker2d(const HitPositions &pos,
                                       const ScintillatorsInLayers &selection)
{
  fCanvas2d->cd();

  if (!fSaveMarkersAndLinesBetweenEvents)
  {
    for (TMarker *marker : fUnRolledViewMarker)
    {
      fCanvas2d->GetListOfPrimitives()->Remove(marker);
      delete marker;
    }
    fUnRolledViewMarker.clear();
  }

  static const double centerOfScintillator = 25;
  int i = 0;
  for (auto iter = selection.begin(); iter != selection.end(); ++iter)
  {
    unsigned int layer = iter->first - 1; // table start form 0, layers from 1
    const std::vector< size_t > &strips = iter->second;
    for (auto stripIter = strips.begin(); stripIter != strips.end();
         ++stripIter)
    {
      unsigned int strip = *stripIter - 1;
      if (layer < fUnRolledViewScintillators.size() &&
          strip < fUnRolledViewScintillators[layer].size())
      {
        double drawedScintillatorLength =
            fUnRolledViewScintillators[layer][strip]->GetX2() -
            fUnRolledViewScintillators[layer][strip]->GetX1();
        double drawedScintillatorCenter = drawedScintillatorLength / 2;
        double z = pos[i].Z() < centerOfScintillator &&
                           pos[i].Z() > -centerOfScintillator
                       ? pos[i].Z()
                       : pos[i].Z() < centerOfScintillator
                             ? -centerOfScintillator
                             : centerOfScintillator;
        double hittedXPos =
            (drawedScintillatorCenter * z) / centerOfScintillator;
        double centerX = fUnRolledViewScintillators[layer][strip]->GetX1() +
                         ((fUnRolledViewScintillators[layer][strip]->GetX2() -
                           fUnRolledViewScintillators[layer][strip]->GetX1()) /
                          2);
        double centerY = fUnRolledViewScintillators[layer][strip]->GetY1() +
                         ((fUnRolledViewScintillators[layer][strip]->GetY2() -
                           fUnRolledViewScintillators[layer][strip]->GetY1()) /
                          2);
        TMarker *marker = new TMarker(centerX + hittedXPos, centerY, 3);
        marker->SetMarkerColor(2);
        marker->SetMarkerSize(3);
        marker->Draw();
        fUnRolledViewMarker.push_back(marker);
        i++;
      }
    }
  }
}

void GeometryVisualizator::setVisibility2d(
    const ScintillatorsInLayers &selection)
{
  fCanvas2d->cd();
  for (auto iter = selection.begin(); iter != selection.end(); ++iter)
  {
    unsigned int layer = iter->first - 1; // table start form 0, layers from 1
    const std::vector< size_t > &strips = iter->second;
    for (auto stripIter = strips.begin(); stripIter != strips.end();
         ++stripIter)
    {
      unsigned int strip = *stripIter - 1;
      if (layer < fUnRolledViewScintillators.size() &&
          strip < fUnRolledViewScintillators[layer].size())
      {
        fUnRolledViewScintillators[layer][strip]->SetFillColor(2);
      }
    }
  }
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
  unsigned int layer = JPetGeomMapping::kBadLayerNumber;
  unsigned int strip = JPetGeomMapping::kBadSlotNumber;
  for (auto iter = selection.begin(); iter != selection.end(); ++iter)
  {
    layer = iter->first;
    if (layer == JPetGeomMapping::kBadLayerNumber)
    {
      std::cout << "/* Bad layer number in setVisibility, returning...*/"
                << "\n";
      // Error("Bad layer number in setVisibility, returning...");
      return;
    }
    const std::vector< size_t > &strips = iter->second;
    nodeLayer = topNode->GetDaughter(layer - 1);
    assert(nodeLayer);

    for (auto stripIter = strips.begin(); stripIter != strips.end();
         ++stripIter)
    {
      strip = *stripIter;
      if (strip == JPetGeomMapping::kBadSlotNumber)
      {
        std::cout << "/* Bad strip number in setVisibility, returning...*/"
                  << "\n";
        // Error("Bad strip number in setVisibility, returning...");
        return;
      }
      nodeStrip = nodeLayer->GetDaughter(strip - 1);
      assert(nodeStrip);
      nodeStrip->GetVolume()->SetLineColor(kRed);
    }
  }
}

void GeometryVisualizator::drawMarkers(const HitPositions &pos)
{
  fCanvasTopView->cd();
  if (!fSaveMarkersAndLinesBetweenEvents)
  {
    for (TPolyLine *line : fLineOnTopView)
    {
      fCanvasTopView->GetListOfPrimitives()->Remove(line);
      delete line;
    }
    fLineOnTopView.clear();
    for (TPolyMarker *marker : fMarkerOnTopView)
    {
      fCanvasTopView->GetListOfPrimitives()->Remove(marker);
      delete marker;
    }
    fMarkerOnTopView.clear();
  }
  fLineOnTopView.push_back(new TPolyLine());
  fMarkerOnTopView.push_back(new TPolyMarker());
  for (unsigned int i = 0; i < pos.size(); i++)
  {
    fLineOnTopView.back()->SetLineWidth(2);
    fLineOnTopView.back()->SetLineColor(kRed);
    fLineOnTopView.back()->SetLineStyle(4);
    fLineOnTopView.back()->SetNextPoint(pos[i].X(), pos[i].Y());

    fMarkerOnTopView.back()->SetMarkerSize(2);
    fMarkerOnTopView.back()->SetMarkerColor(kGreen);
    fMarkerOnTopView.back()->SetMarkerStyle(2);
    fMarkerOnTopView.back()->SetNextPoint(pos[i].X(), pos[i].Y());
  }
  fLineOnTopView.back()->Draw();
  fMarkerOnTopView.back()->Draw();
}

void GeometryVisualizator::drawLineBetweenActivedScins(const HitPositions &pos)
{
  fCanvas3d->cd();
  if (!fSaveMarkersAndLinesBetweenEvents)
  {
    for (TPolyLine3D *line : fLineOn3dView)
    {
      fCanvas3d->GetListOfPrimitives()->Remove(line);
      delete line;
    }
    fLineOn3dView.clear();
    for (TPolyMarker3D *marker : fMarkerOn3dView)
    {
      fCanvas3d->GetListOfPrimitives()->Remove(marker);
      delete marker;
    }
    fMarkerOn3dView.clear();
  }
  fLineOn3dView.push_back(new TPolyLine3D());
  fMarkerOn3dView.push_back(new TPolyMarker3D());
  for (unsigned int i = 0; i < pos.size(); i++)
  {
    double x = pos[i].X();
    double y = pos[i].Y();
    double z = pos[i].Z() < 25 && pos[i].Z() > -25 ? pos[i].Z()
                                                   : pos[i].Z() < 25 ? -25 : 25;
    fLineOn3dView.back()->SetLineWidth(2);
    fLineOn3dView.back()->SetLineColor(kRed);
    fLineOn3dView.back()->SetLineStyle(4);
    fLineOn3dView.back()->SetNextPoint(x, y, z);

    fMarkerOn3dView.back()->SetMarkerSize(2);
    fMarkerOn3dView.back()->SetMarkerColor(kGreen);
    fMarkerOn3dView.back()->SetMarkerStyle(2);
    fMarkerOn3dView.back()->SetNextPoint(x, y, z);
  }
  fLineOn3dView.back()->Draw();
  fMarkerOn3dView.back()->Draw();
}

void GeometryVisualizator::draw2dGeometry2()
{
  const double canvasRange = 60;
  const double axisPos = 59;
  fCanvasTopView->cd();
  fCanvasTopView->Range(-canvasRange, -canvasRange, canvasRange, canvasRange);
  TEllipse *thirdLayerCircle = new TEllipse(0, 0, 57.5, 57.5);
  thirdLayerCircle->Draw();
  TEllipse *secondLayerCircle = new TEllipse(0, 0, 46.75, 46.75);
  secondLayerCircle->Draw();
  TEllipse *firstLayerCircle = new TEllipse(0, 0, 42.5, 42.5);
  firstLayerCircle->Draw();

  TGaxis *axisX =
      new TGaxis(-57.5, axisPos, 57.5, axisPos, -57.5, 57.5, 50510, "");
  axisX->SetName("axisX");
  axisX->SetLabelSize(0.02);
  axisX->Draw();

  TGaxis *axisY =
      new TGaxis(axisPos, -57.5, axisPos, 57.5, -57.5, 57.5, 50510, "");
  axisY->SetName("axisY");
  axisY->SetLabelSize(0.02);
  axisY->Draw();
}

void GeometryVisualizator::setAllStripsUnvisible()
{
  assert(fGeoManager);
  TGeoNodeMatrix *topNode =
      static_cast< TGeoNodeMatrix * >(fGeoManager->GetTopNode());
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
      stripNode->GetVolume()->SetLineColor(layersColors[i]);
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
        std::unique_ptr< TCanvas >(fRootCanvasDiagrams->GetCanvas());
  int vectorSize = diagramData.size();
  fCanvasDiagrams->cd();
  if (vectorSize != fLastDiagramVectorSize)
  {
    fCanvasDiagrams->Clear();
    fCanvasDiagrams->DivideSquare(vectorSize);
  }

  for (int j = 0; j < vectorSize; j++)
  {
    fCanvasDiagrams->cd(j + 1);
    int n = diagramData[j].size();
    if (n == 0)
      return;
    std::vector< float > leadingX;
    std::vector< float > leadingY;
    std::vector< float > trailingX;
    std::vector< float > trailingY;
    const float changePsToNs = 0.001;
    for (auto it = diagramData[j].begin(); it != diagramData[j].end(); it++)
    {
      if (std::get< 3 >(*it) == JPetSigCh::Leading)
      {
        leadingX.push_back(static_cast< float >(std::get< 2 >(*it)) *
                           changePsToNs);
        leadingY.push_back(changeSignalNumber(std::get< 0 >(*it)));
      }
      else
      {
        trailingX.push_back(static_cast< float >(std::get< 2 >(*it)) *
                            changePsToNs);
        trailingY.push_back(changeSignalNumber(std::get< 0 >(*it)));
      }
    }
    TMultiGraph *mg = new TMultiGraph();

    std::string title = "layer ";
    title.append(std::to_string(std::get< 5 >(*diagramData[j].begin())));
    title.append(" slot ");
    title.append(std::to_string(std::get< 6 >(*diagramData[j].begin())));
    if (std::get< 4 >(*diagramData[j].begin()) == JPetPM::SideA)
      title.append(" Side A");
    else
      title.append(" Side B");
    mg->SetTitle(title.c_str());

    if (leadingX.size() > 0)
    {
      TGraph *gr =
          new TGraph(leadingX.size(), leadingX.data(), leadingY.data());
      mg->Add(gr);
    }

    if (trailingX.size() > 0)
    {
      TGraph *gr2 =
          new TGraph(trailingX.size(), trailingX.data(), trailingY.data());
      gr2->SetMarkerColor(kRed);

      mg->Add(gr2);
    }

    mg->Draw("AP*");
    mg->GetYaxis()->SetLabelOffset(999);
    mg->GetYaxis()->SetTickLength(0);
    mg->GetYaxis()->SetRangeUser(0, 5);
    mg->GetXaxis()->SetLimits(0, 50);
    // mg->GetXaxis()->SetLimits(mg->GetXaxis()->GetXmin() - 10 * 1000,
    //                          mg->GetXaxis()->GetXmin() + 20 * 1000);
    mg->GetXaxis()->SetNdivisions(504, kFALSE);
    mg->GetXaxis()->SetTitle("Time (ns)");
    mg->GetYaxis()->SetTitle("Threshold Number");

    for (int k = 0; k < 4; k++)
    {
      TLine *line = new TLine(mg->GetXaxis()->GetXmin(), k + 1,
                              mg->GetXaxis()->GetXmax(), k + 1);
      line->SetLineColor(kRed);
      line->Draw();
    }
  }
}

float GeometryVisualizator::changeSignalNumber(
    int signalNumber) // change this...
{
  // std::cout << "signalNumber: " << signalNumber << "\n";
  if (signalNumber == 4)
    return 1.;
  if (signalNumber == 3)
    return 2.;
  if (signalNumber == 2)
    return 3.;
  if (signalNumber == 1)
    return 4.;
  return 0.;
}

void GeometryVisualizator::reverseYAxis(TGraph *g)
{
  // Remove the current axis
  g->GetYaxis()->SetLabelOffset(999);
  g->GetYaxis()->SetTickLength(0);

  gPad->Update();
  TGaxis *newaxis =
      new TGaxis(gPad->GetUxmin(), gPad->GetUymax(), gPad->GetUxmin() - 0.001,
                 gPad->GetUymin(), g->GetYaxis()->GetXmin(),
                 g->GetYaxis()->GetXmax(), 510, "+");
  newaxis->SetLabelOffset(-0.03);
  newaxis->Draw();
}

void GeometryVisualizator::createGeometry(
    const int numberOfLayers, const int kLength,
    const std::vector< std::pair< int, double > > &layerStats)
{
  const double kXWorld = kLength + 100;
  const double kYWorld = kLength + 100;
  const double kZWorld = kLength + 100;

  const double kLayerThickness = 0.9;
  gSystem->Load("libGeom");

  fGeoManager =
      std::unique_ptr< TGeoManager >(new TGeoManager("mgr", "PET detector"));

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

  std::vector< TGeoTube * > layers;

  for (int i = 0; i < numberOfLayers; i++)
  {
    TGeoTube *layer = new TGeoTube(
        layerStats[i].second, layerStats[i].second + kLayerThickness, kLength);
    assert(layer);
    layers.push_back(layer);
  }

  TGeoVolume *vol = 0;
  char nameOfLayer[100];
  int i = 0;
  int volNr = 0;
  double startFi[]{0, 3.75, 1.875};
  double currentFi = 0;
  for (TGeoTube *layer : layers)
  {
    sprintf(nameOfLayer, "layer_%d", i + 1);
    vol = new TGeoVolume(nameOfLayer, layer, medium);
    assert(vol);
    vol->SetVisibility(kTRUE);
    currentFi = startFi[i];
    for (int j = 0; j < layerStats[i].first; j++)
    {
      TGeoVolume *scin = gGeoManager->MakeTube("scin", medium, 0, 0.7, 50);
      // scin->SetLineWidth(5);
      // scin->SetLineColor(layersColors[i]);

      scin->SetLineColorAlpha(layersColors[i], 0.3);
      vol->AddNode(
          scin, j,
          new TGeoTranslation(
              layerStats[i].second * std::cos(currentFi * (M_PI / 180)),
              layerStats[i].second * std::sin(currentFi * (M_PI / 180)), 0));
      currentFi += 360. / layerStats[i].first;
    }
    volNr = top->GetNtotal() + 1;
    top->AddNode(vol, volNr, rotation);
    i++;
  }
  fGeoManager->CloseGeometry();
  fGeoManager->SetVisLevel(4);
  // fGeoManager->Export("tt.root");
}
}
