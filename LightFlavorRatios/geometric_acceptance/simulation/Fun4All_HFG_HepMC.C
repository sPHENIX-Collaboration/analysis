#ifndef MACRO_FUN4ALLG4SPHENIX_C
#define MACRO_FUN4ALLG4SPHENIX_C

#include "G4_Input.C"
#include <G4_Global.C>
#include <G4Setup_sPHENIX.C>

#include <Trkr_RecoInit.C>
#include <Trkr_Clustering.C>
#include <Trkr_TruthTables.C>
#include <Trkr_Reco.C>
#include <Trkr_Eval.C>

#include <phpythia8/PHPy8ParticleTrigger.h>

#include <decayfinder/DecayFinder.h>
#include <hftrackefficiency/HFTrackEfficiency.h>
#include <kfparticle_sphenix/KFParticle_sPHENIX.h>

#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>
#include <ffamodules/CDBInterface.h>
#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <simqa_modules/QAG4SimulationTracking.h>
#include <qautils/QAHistManagerDef.h>

#include <nodedump/Dumper.h>

#include "HF_selections.C"

#include <geoacceptancenog4/GeoAcceptanceNoG4.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libphnodedump.so)
R__LOAD_LIBRARY(libGeoAcceptanceNoG4.so)
//R__LOAD_LIBRARY(libdecayfinder.so)
//R__LOAD_LIBRARY(libhftrackefficiency.so)
//R__LOAD_LIBRARY(libsimqa_modules.so)

int Fun4All_HFG_HepMC(std::string processID = "000000")
{
  int nEvents = -1;

  std::string infile = "/phenix/u/pinkenbu/DST_TRUTH_G4HIT_pythia8_NONE-0000000029-000000.root";

  //F4A setup
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);

  PHRandomSeed::Verbosity(1);
  recoConsts *rc = recoConsts::instance();

  Input::READHITS = true;
  INPUTREADHITS::filename[0] = infile;

  //InputInit(); //does nothing in this case

  InputManagers();

  // copy HepMC records into G4
  HepMCNodeReader *hr = new HepMCNodeReader();
  hr->Verbosity(5);
  //se->registerSubsystem(hr);

  Enable::CDB = true;
  //rc->set_StringFlag("CDB_GLOBALTAG","MDC2");
  rc->set_StringFlag("CDB_GLOBALTAG",CDB::global_tag);
  //rc->set_StringFlag("CDB_GLOBALTAG","ProdA_2024");
  rc->set_uint64Flag("TIMESTAMP",CDB::timestamp);
  rc->set_IntFlag("RUNNUMBER",29);

  FlagHandler *flag = new FlagHandler();
  se->registerSubsystem(flag);

  GeoAcceptanceNoG4* geoacc = new GeoAcceptanceNoG4();
  //se->registerSubsystem(geoacc);

  Dumper* truth_dump = new Dumper("truth_dump");
  truth_dump->SetOutDir(".");
  se->registerSubsystem(truth_dump);

  se->run(nEvents);

  se->End();

  gSystem->Exit(0);
}

#endif
