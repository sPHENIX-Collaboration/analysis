#include <caloreco/RawClusterPositionCorrection.h>

#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>

#include <fun4allraw/Fun4AllPrdfInputManager.h>

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllUtils.h>
#include <fun4all/SubsysReco.h>

#include <phool/recoConsts.h>

#include <ffamodules/CDBInterface.h>
#include <GlobalVariables.C>


R__LOAD_LIBRARY(libcdbobjects)

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libffamodules.so)

#include <caloana/CaloAna.h>
R__LOAD_LIBRARY(libcaloana.so)

void Fun4All_EMCal_sp(int nevents = 100000, const std::string &fname = "inputdata_sp.txt")
{

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);

  // se->Verbosity(verbosity);
  recoConsts *rc = recoConsts::instance();

  ifstream file(fname);
  string first_file;
  getline(file, first_file);

  //===============
  // conditions DB flags
  //===============
  pair<int, int> runseg = Fun4AllUtils::GetRunSegment(first_file);
  int runnumber = runseg.first;
  cout << "run number = " << runnumber << endl;

  // global tag
  rc->set_StringFlag("CDB_GLOBALTAG", "MDC2");
  // // 64 bit timestamp
  rc->set_uint64Flag("TIMESTAMP", runnumber);

  Fun4AllInputManager *in = new Fun4AllDstInputManager("DST_TOWERS");
  in->AddListFile(fname);
  se->registerInputManager(in);

  std::string filename = first_file.substr(first_file.find_last_of("/\\") + 1);
  std::string OutFile = Form("OUTHIST_iter_%s",filename.c_str());


/*
  std::cout << "Applying Position Dependent Correction" << std::endl;
  RawClusterPositionCorrection *clusterCorrection = new RawClusterPositionCorrection("CEMC");
  clusterCorrection->set_UseTowerInfo(1);  // to use towerinfo objects rather than old RawTower
  se->registerSubsystem(clusterCorrection);
*/

  ///////////////////
  // analysis modules
  CaloAna *ca = new CaloAna("calomodulename", OutFile);
  ca->set_timing_cut_width(16);
  ca->apply_vertex_cut(false);
  ca->set_vertex_cut(20.);
  se->registerSubsystem(ca);

  se->run(nevents);
  se->End();
  se->PrintTimer();
  delete se;

  TFile* f_done_signal = new TFile("DONE.root","recreate");
  std::cout << "All done!" << std::endl;
  gSystem->Exit(0);

}

