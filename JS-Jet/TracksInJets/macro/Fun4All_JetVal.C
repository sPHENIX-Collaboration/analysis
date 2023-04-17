#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

#include <g4jets/FastJetAlgo.h>
#include <g4jets/JetReco.h>
#include <g4jets/TowerJetInput.h>
#include <g4jets/TruthJetInput.h>

#include <g4centrality/PHG4CentralityReco.h>

#include <HIJetReco.C>

#include <tracksinjets/TracksInJets.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libTracksInJets.so)
R__LOAD_LIBRARY(libg4centrality.so)
R__LOAD_LIBRARY(libg4dst.so)

#endif

void Fun4All_JetVal(const char *filelisttrk = "dst_tracks.list",
		    const char *filelistcalo = "dst_calo_cluster.list",
		    const char *filelistbbc = "dst_bbc_g4hit.list",
		    const char *outname = "outputest.root")
{

  
  Fun4AllServer *se = Fun4AllServer::instance();
  int verbosity = 0;

  se->Verbosity(verbosity);
  recoConsts *rc = recoConsts::instance();

  PHG4CentralityReco *cent = new PHG4CentralityReco();
  cent->Verbosity(0);
  cent->GetCalibrationParameters().ReadFromFile("centrality", "xml", 0, 0, string(getenv("CALIBRATIONROOT")) + string("/Centrality/"));
  se->registerSubsystem( cent );

  HIJetReco();  

  TracksInJets *trksinjets = new TracksInJets("AntiKt_Tower_r04_Sub1", outname);
  se->registerSubsystem(trksinjets);  
  
  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTcalo");
  in->AddListFile(filelistcalo,1);
  se->registerInputManager(in);

  Fun4AllInputManager *intrk = new Fun4AllDstInputManager("DSTtrk");
  intrk->AddListFile(filelisttrk,1);
  se->registerInputManager(intrk);
  
  Fun4AllInputManager *inbbc = new Fun4AllDstInputManager("DSTbbc");
  inbbc->AddListFile(filelistbbc,1);
  se->registerInputManager(inbbc);

  se->run(100);
  se->End();

  gSystem->Exit(0);
  return 0;

}
