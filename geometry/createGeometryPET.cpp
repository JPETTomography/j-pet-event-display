// Wojciech Krzemien
// 08.11.2013 
#include <TROOT.h>
#include <TPad.h>
#include <TView.h>
#include <TSystem.h>
#include <TObjArray.h>
#include <TGeoVolume.h>
#include <TGeoBBox.h>
#include <TGeoTube.h>
#include <TGeoManager.h>
#include <TGeoMatrix.h>

#include <cassert>
#include <vector>

void createDetector(TGeoVolume *top, TGeoRotation* rotation) 
{
  assert(top);
  TGeoMaterial *matAl = new TGeoMaterial("Al", 0,0 ,0);
  assert(matAl);
  TGeoMedium *medium = new TGeoMedium("Aluminium",2, matAl);
  assert(medium);
  // pseudo geometry for Inner Tracker for KLOE
  // concentric cylinder layers
  
  static const double kLength = 70;
  static const int kNumberOfLayers = 3; 
  static const double kLayerThickness = 0.9; 
  // radius of every layer without layer thickness 
  static const double kRadii[] = {13., 15.5, 18., 20.5};
  

  // XStrips are parallel to OZ axis
  // VStrips are inclined by some value i dont know what 
  static const int kNumberOfXStrips[] = { 48, 48, 96 };
  
  std::vector<TGeoTube*> layers; 
  
  for (int i = 0; i < kNumberOfLayers; i++) {
    // the last argument is the distance along Z from 0,0,0 to positive Z border (called dz) 
    // and the total kLength along Z axis 
    // is from -dz to dz
    TGeoTube* layer = new TGeoTube(kRadii[i], kRadii[i] + kLayerThickness, kLength/2.); 
    assert(layer);
    layers.push_back(layer);
  }



  TGeoVolume* vol = 0; 
  // Inner Tracker

  int volNr = top->GetNtotal() + 1;
  std::vector<TGeoTube*>::iterator it; 
  int i = 0;
  char nameOfLayer[100];
  for (it = layers.begin(); it != layers.end(); ++it) {

    //(*it)->Divide(vol, "XSlicesCreation",2 , 30, 0, 0);
    sprintf(nameOfLayer, "layer_%d", i + 1);
    vol = new TGeoVolume(nameOfLayer, *it, medium);
    assert(vol);
    vol->SetVisibility(kTRUE);
    vol->SetLineColor(kBlue);

    vol->Divide("XStrip", 2, kNumberOfXStrips[i], 0, 0 ,0);
    TObjArray* array = vol->GetNodes();
    assert(array);
    TGeoNode* strip = 0;
    TIter iter(array);
    while (strip = static_cast<TGeoNode*>(iter.Next())) {
     strip->GetVolume()->SetLineColor(kRed);
     strip->GetVolume()->SetVisibility(kTRUE);
    }
  //  strip = vol->GetNode(2);
  //  strip->GetVolume()->SetVisiblity(kTRUE);
    // vol->SetVisibility(kTRUE);
    //vol->SetLineColor(kBlue);
    //vol->SetVisibility(kTRUE);
    volNr = top->GetNtotal() + 1; 
    top->AddNode(vol, volNr, rotation);
    i++;
  }
}


// isTopVisible - if the top volume will be visible during drawning
void createGeometryPET(bool isTopVisible = false)
{

  std::string outputGeomFile = "JPET_geom.root";
 

  const double kXWorld = 500;
  const double kYWorld = 500;
  const double kZWorld = 500;

  gSystem->Load("libGeom");
  TGeoManager *mgr = new TGeoManager("mgr", "PET detector");
  assert(mgr);

  TGeoMaterial *matVacuum = new TGeoMaterial("Vacuum", 0,0,0);
  assert(matVacuum);
  TGeoMedium *vacuum = new TGeoMedium("Vacuum",1, matVacuum);
  assert(vacuum);
  TGeoMaterial* matAl = new TGeoMaterial("Al", 0,0 ,0);
  assert(matAl);
  TGeoMedium* medium2 = new TGeoMedium("Aluminium",2, matAl);
  assert(medium2);

  TGeoVolume* top = mgr ->MakeBox("TOP", vacuum, kXWorld, kYWorld, kZWorld);
  assert(top);
  mgr->SetTopVolume(top);

  TGeoRotation* rotation = new TGeoRotation();
   //rotation->SetAngles(60., 90., 0.);
  //rotation->SetAngles(0, 90., 0.);
  rotation->SetAngles(0., 0., 0.);

  //rotation->SetAngles(60., 0., 0.);
  createDetector(top, rotation);


  if (isTopVisible) {
    top->SetLineColor(kMagenta);
    mgr->SetTopVisible();
  }

  mgr->CloseGeometry();
  //gPad->GetView()->Top();
  mgr->SetVisLevel(4);
  //mgr->SetVisOption(0);
  top->Draw();

  mgr->Export(outputGeomFile.c_str());
}

int main()
{
  createGeometryPET();
}
