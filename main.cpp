/**
 *  @copyright Copyright (c) 2013, Wojciech Krzemien
 *  @file Run.cc
 *  @version 1.0
 *  @author Wojciech Krzemien, wojciech.krzemien@if.uj.edu.pl
 */

#include <TError.h>
#include <TRint.h>
#include <TROOT.h>
#include <TEnv.h>
#include <TCanvas.h>
#include <memory>

#include "src/LoggerInclude.h"
#include "src/GeometryVisualizator.h"

int main(int argc, char** argv)
{
  using namespace jpet_event_display;
  //gErrorIgnoreLevel = kFatal; /// switch off ROOT warnings and errors
  DATE_AND_TIME();
  INFO("J-PET Event Display started");
  INFO("*********************");
  auto theApp = std::unique_ptr<TRint>(new TRint("App", &argc, argv));
  auto canvas = std::make_shared<TCanvas>("MyCanvas", "Test Canvas", 10, 10, 900, 500);
  GeometryVisualizator visualizator(canvas);
  visualizator.loadGeometry("JPET_geom.root");
  visualizator.drawOnlyGeometry();
  std::map<int, std::vector<int> > selection;
  std::vector<int> channels;
  channels.push_back(10);
  channels.push_back(11);
  channels.push_back(12);
  channels.push_back(13);
  selection[0] = channels;
  visualizator.drawStrips(selection);
  canvas->Draw();
  gEnv = new TEnv(".rootrc");
  theApp->Run();
  return 0;
}
