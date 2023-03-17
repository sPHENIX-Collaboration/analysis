
#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>

#include <filterevents/FilterEvents.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfilterevents.so)
#endif

void Fun4All_FilterEvents(vector<string> myInputLists = {"condorJob/fileLists/productionFiles-HIJING_0-20-tracks-00000.list"}, const int nEvents = 10)
{

//  gSystem->Load("libg4dst");
//  gSystem->Load("libfiltereventsupsilon");
  string outDir = "/sphenix/u/rosstom/analysis/FilterEvents/macro/";

  string fileNumber = myInputLists[0];
  size_t findLastDash = fileNumber.find_last_of("-");
  if (findLastDash != string::npos) fileNumber.erase(0, findLastDash + 1);
  string remove_this = ".list";
  size_t pos = fileNumber.find(remove_this);
  if (pos != string::npos) fileNumber.erase(pos, remove_this.length());
  string outputFileName = "outputFilteredTrackSeeds_" + fileNumber + ".root";

  string outputRecoDir = outDir + "inReconstruction/";
  string makeDirectory = "mkdir -p " + outputRecoDir;
  system(makeDirectory.c_str());
  const string outputRecoFile = outputRecoDir + outputFileName;

  cout << "output file name: " << outputRecoFile << endl;

  bool upsilonFilter = false; // true for Upsilon, false for D0 (will make more general soon)

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);

  //Add all required input files
  for (unsigned int i = 0; i < myInputLists.size(); ++i)
  {
    Fun4AllInputManager *infile = new Fun4AllDstInputManager("DSTin_" + to_string(i));
    infile->AddListFile(myInputLists[i]);
    se->registerInputManager(infile);
  } 
  //Needs TRACKS, TRACKSEEDS, TRKR_CLUSTER, CALO_CLUSTER, and VERTEX DSTs

  //in1->AddFile("DST_TRACKS_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000060-00000.root");
  //in2->AddFile("DST_TRACKSEEDS_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000060-00000.root");
  //in3->AddFile("DST_TRKR_CLUSTER_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000060-00000.root");
  //in4->AddFile("DST_CALO_CLUSTER_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000060-00000.root");
  //in5->AddFile("DST_VERTEX_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000060-00000.root");

  FilterEvents *filter = new FilterEvents("FilterEvents");
  if (!upsilonFilter) filter->setCuts(1.0, 0.01, 20, false);
  se->registerSubsystem(filter);

  Fun4AllOutputManager *out = new Fun4AllDstOutputManager("out",outputRecoFile);
  out->Verbosity(1);
  //out->AddNode("SvtxTrackMap_ee");
  out->AddNode("TRKR_CLUSTER");
  out->AddNode("SiliconTrackSeedContainer");
  out->AddNode("TpcTrackSeedContainer");
  out->AddNode("SvtxTrackSeedContainer");
  if (upsilonFilter) out->AddNode("CLUSTER_CEMC");
  out->AddNode("SvtxVertexMap");
  out->AddNode("GlobalVertexMap");
  se->registerOutputManager(out);
  out->Print();

  se->run(nEvents);
  se->End();

  ifstream file(outputRecoFile.c_str());
  if (file.good())
  {
    string moveOutput = "mv " + outputRecoFile + " " + outDir;
    system(moveOutput.c_str());
  }

cout << "all done." << endl;
}



