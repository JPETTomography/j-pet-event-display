/**
 *   @copyright Copyright (c) 2013, Wojciech Krzemien
 *   @file GeometryVisualizator.cc
 *   @version 1.0
 *   @author Wojciech Krzemien
 *   @brief
 */

#include "./GeometryVisualizator.h"
//#include "./Gui.h"
#include "./LoggerInclude.h"
#include <TCanvas.h>
#include <TFile.h>

#include <TPolyLine3D.h>
#include <TRandom.h>


using namespace std;

GeometryVisualizator::GeometryVisualizator(TCanvas* canv):
fGeoManager(0),
fMyCanv(canv)
{
  if (fMyCanv) fMyCanv->Divide(2, 1);
}

GeometryVisualizator::~GeometryVisualizator()
{
  if (fGeoManager != 0) {
    delete fGeoManager;
    fGeoManager = 0;
  }
}

void GeometryVisualizator::loadGeometry(TString geomFile)
{
  TFile* inputGeomFile;
  inputGeomFile = new TFile(geomFile);
  if (inputGeomFile->IsZombie()) {
    assert(1 == 0);
    ERROR(std::string("Error opening file:" + geomFile));
    return;
  }
  fGeoManager = (TGeoManager*) inputGeomFile->Get("mgr");
  assert(fGeoManager);
}

void GeometryVisualizator::drawOnlyGeometry()
{
  if (fMyCanv == 0) {
    WARNING("Canvas not set");
    return;
  }

  //fGeoManager->SetVisLevel(4);
  //fGeoManager->SetVisOption(0);
  setAllStripsUnvisible();
  //fGeoManager->GetTopVolume()->Draw();
  drawPads();
  fGeoManager->SetVisLevel(4);
  fGeoManager->SetVisOption(0);
  fMyCanv->Modified();
  fMyCanv->Update();
}

void GeometryVisualizator::drawStrips(const std::map<int, std::vector<int> >& selection)
{
  if (fMyCanv == 0) {
    WARNING("Canvas not set");
    return;
  }
/// @todo add some silly examples to test like strips 1,2,3 in every layer
 // std::vector<int> sel2;
 // sel2.push_back(1);
 // sel2.push_back(2);
 // sel2.push_back(3);
 //// std::map<int, std::vector<int> > full;
 // full[0]=sel2;
 // full[1]=sel2;
 // full[2]=sel2;
 // full[3]=sel2;
 setVisibility(selection);
 // setVisibility(full);
  drawPads();
}


void GeometryVisualizator::drawPads()
{
  if (fMyCanv == 0) {
    WARNING("Canvas not set");
    return;
  }
  fMyCanv->cd(0);
  assert(fGeoManager);
  //fGeoManager->GetTopVolume()->Draw();
  //assert(gPad);
  //assert(view);
  //view->ZoomView(0, 13.);
  //view->SetParallel();
  //view->RotateView(-90,0);


  Int_t irep;

  //fMyCanv->cd(2);
  fGeoManager->GetTopVolume()->Draw();
  assert(gPad);
  TView* view = gPad->GetView();
  view = gPad->GetView();
  assert(view);
  view->ZoomView(0, 1.9);
  view->SetView(view->GetLongitude(), view->GetLatitude() , view->GetPsi() -90, irep);
  gPad->Modified();
  gPad->Update();
}

void GeometryVisualizator::setVisibility(const std::map<int, std::vector<int> >& selection)
{
    std::cout <<"Im here" <<std::endl;
  setAllStripsUnvisible();
    std::cout <<"Im here" <<std::endl;
  if (selection.empty()) return;
  assert(fGeoManager);
  TGeoNode* topNode = fGeoManager->GetTopNode();
  assert(topNode);
  TGeoNode* nodeLayer = 0;
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
    assert(nodeLayer);
    for (stripIter = strips.begin(); stripIter != strips.end(); ++stripIter) {
      strip = *stripIter;
      nodeName = getStripNodeName(strip + 1); /// strips numbers starts from ?
      nodeStrip = nodeLayer->GetVolume()->FindNode(nodeName.c_str());
      assert(nodeStrip);
      nodeStrip->SetVisibility(kTRUE);
    }
  }
}

void GeometryVisualizator::setAllStripsUnvisible()
{
  assert(fGeoManager);
  TGeoNode* topNode = fGeoManager->GetTopNode();
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

  node = topNode->GetVolume()->FindNode("layer_4_1");
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

  //node = topNode->GetVolume()->FindNode("DC_1");
  //assert(node);
  //node->SetVisibility(kTRUE);
//  node->GetVolume()->SetVisibility(kTRUE);
//  node->SetVisibility(kTRUE);
//  TObjArray* array= node->GetNodes();
//  assert(array);
//  TIter it(array);
//  while((node = (TGeoNode*)it.Next())) {
//      node->SetVisibility(kFALSE);
// //     node->GetVolume()->SetLineColor(kBlue);
////      node->GetVolume()->Paint();
//    if(std::string(node->GetName())=="XStrip_1000") {
//      node->GetVolume()->SetLineWidth(5);
//      node->SetVisibility(kTRUE);
//    }
//    if(std::string(node->GetName())=="XStrip_20") {
//      node->GetVolume()->SetLineWidth(5);
//      node->SetVisibility(kTRUE);
//    }
//  }
//  node = topNode->GetVolume()->FindNode("layer_1_1");
////  node->GetVolume->Draw();
//
//
//
//  //node = topNode->GetVolume()->FindNode("layer_1_1");
//  //assert(node);
//  //node = node->GetVolume()->FindNode("XStrip_23");
//  //node->GetVolume()->SetVisibility(kTRUE);
//  //assert(node);
//
//
//  node = topNode->GetVolume()->FindNode("layer_2_1");
//  assert(node);
//  node->SetVisibility(kTRUE);
//  node->VisibleDaughters(kFALSE);
//
//  node = topNode->GetVolume()->FindNode("layer_3_1");
//  assert(node);
//  node->SetVisibility(kTRUE);
//  node->VisibleDaughters(kFALSE);
//
//  node = topNode->GetVolume()->FindNode("layer_4_1");
//  assert(node);
//  node->SetVisibility(kTRUE);
//  node->VisibleDaughters(kFALSE);
//
//  node = topNode->GetVolume()->FindNode("DC_1");
//  assert(node);
//  node->GetVolume()->SetVisibility(kTRUE);
}

bool GeometryVisualizator::isGeoManagerInitialized()const
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
  name = name + CommonTools::intToString(layer) + std::string("_1");
  return name;
}

std::string GeometryVisualizator::getStripNodeName(int strip) const
{
  std::string name("XStrip_");
  name = name + CommonTools::intToString(strip);
  return name;
}
