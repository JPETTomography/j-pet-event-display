/** 
 *   @copyright Copyright (c) 2015, Wojciech Krzemien
 *   @file EDGeometryVisualizator.h
 *   @version 1.0
 *   @author Wojciech Krzemien
 *   @brief Class visualizes geometry of the IT and DC along with fired strips.
 *   
 */

#ifndef EDGEOMETRYVISUALIZATOR_H_
#define EDGEOMETRYVISUALIZATOR_H_

#include <TView.h>
#include <TVirtualPad.h>
#include <TGeoManager.h>
#include <TGeoNode.h>
#include <TGeoVolume.h>
#include <cassert>
#include <map>
#include <vector>
#include "./EDCommonTools.h"

class TCanvas;

class EDGeometryVisualizator {
 public:
  EDGeometryVisualizator(TCanvas* canv);
  ~EDGeometryVisualizator();
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

#endif  // EDGEOMETRYVISUALIZATOR_H_
