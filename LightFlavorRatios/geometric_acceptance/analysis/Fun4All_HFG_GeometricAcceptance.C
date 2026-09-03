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

//#include <geoacceptancenog4/GeoAcceptanceNoG4.h>
#include <resonance_geometricacceptance/ResonanceGeometricAcceptance.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libphnodedump.so)
R__LOAD_LIBRARY(libResonanceGeometricAcceptance.so)
//R__LOAD_LIBRARY(libGeoAcceptanceNoG4.so)
//R__LOAD_LIBRARY(libdecayfinder.so)
//R__LOAD_LIBRARY(libhftrackefficiency.so)
//R__LOAD_LIBRARY(libsimqa_modules.so)

int Fun4All_HFG_GeometricAcceptance(std::string processID = "000000")
{
  int nEvents = -1;

  //std::string infile = "/sphenix/user/mjpeters/analysis/LightFlavorRatios/geometric_acceptance/simulation/DST/lambdaKshort_DST_000000.root";
  std::string infile = "/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/closureTestSample/DST/lambdaKshort_DST_"+processID+".root";

  //std::string outDir = "./";
  std::string outDir = "/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/closureTestSample/";

  std::string makeDirectory = "mkdir -p " + outDir + "geometricAcceptance";
  system(makeDirectory.c_str());

  //F4A setup
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);

  PHRandomSeed::Verbosity(1);
  recoConsts *rc = recoConsts::instance();

  Input::READHITS = true;
  INPUTREADHITS::filename[0] = infile;

  //InputInit(); //does nothing in this case

  InputManagers();
/*
  // copy HepMC records into G4
  HepMCNodeReader *hr = new HepMCNodeReader();
  hr->Verbosity(5);
  //se->registerSubsystem(hr);
*/
  Enable::CDB = true;
  //rc->set_StringFlag("CDB_GLOBALTAG","MDC2");
  rc->set_StringFlag("CDB_GLOBALTAG",CDB::global_tag);
  //rc->set_StringFlag("CDB_GLOBALTAG","ProdA_2024");
  rc->set_uint64Flag("TIMESTAMP",CDB::timestamp);
  rc->set_IntFlag("RUNNUMBER",29);

  FlagHandler *flag = new FlagHandler();
  se->registerSubsystem(flag);

  int geo_accept_verbosity = 1;

  ResonanceGeometricAcceptance* geoaccept_lambda = new ResonanceGeometricAcceptance("GeometricAcceptance_LambdaInclusive");
  geoaccept_lambda->setOutputFilename(outDir+"geometricAcceptance/Lambda0_geo_acceptance_"+processID+".root");
  geoaccept_lambda->setMotherName("Lambda0");
  geoaccept_lambda->setMotherPDGID(3122);
  geoaccept_lambda->setDaughterPDGIDs({-211,2212});
  geoaccept_lambda->includeConjugate();
  geoaccept_lambda->Verbosity(geo_accept_verbosity);
  se->registerSubsystem(geoaccept_lambda);

  ResonanceGeometricAcceptance* geoaccept_lambda_pos = new ResonanceGeometricAcceptance("GeometricAcceptance_Lambda");
  geoaccept_lambda_pos->setOutputFilename(outDir+"geometricAcceptance/Lambda0_geo_acceptance_pos_"+processID+".root");
  geoaccept_lambda_pos->setMotherName("Lambda0");
  geoaccept_lambda_pos->setMotherPDGID(3122);
  geoaccept_lambda_pos->setDaughterPDGIDs({-211,2212});
  geoaccept_lambda_pos->includeConjugate(false);
  geoaccept_lambda_pos->Verbosity(geo_accept_verbosity);
  se->registerSubsystem(geoaccept_lambda_pos);

  ResonanceGeometricAcceptance* geoaccept_lambda_neg = new ResonanceGeometricAcceptance("GeometricAcceptance_LambdaBar");
  geoaccept_lambda_neg->setOutputFilename(outDir+"geometricAcceptance/Lambda0_geo_acceptance_neg_"+processID+".root");
  geoaccept_lambda_neg->setMotherName("Lambda0");
  geoaccept_lambda_neg->setMotherPDGID(-3122);
  geoaccept_lambda_neg->setDaughterPDGIDs({211,-2212});
  geoaccept_lambda_neg->includeConjugate(false);
  geoaccept_lambda_neg->Verbosity(geo_accept_verbosity);
  se->registerSubsystem(geoaccept_lambda_neg);

  ResonanceGeometricAcceptance* geoaccept_kshort = new ResonanceGeometricAcceptance("GeometricAcceptance_Kshort");
  geoaccept_kshort->setOutputFilename(outDir+"geometricAcceptance/K_S0_geo_acceptance_"+processID+".root");
  geoaccept_kshort->setMotherName("K_S0");
  geoaccept_kshort->setMotherPDGID(310);
  geoaccept_kshort->setDaughterPDGIDs({211,-211});
  geoaccept_kshort->includeConjugate(false);
  geoaccept_kshort->Verbosity(geo_accept_verbosity);
  se->registerSubsystem(geoaccept_kshort);

/*
  GeoAcceptanceNoG4* geoacc = new GeoAcceptanceNoG4();
  //se->registerSubsystem(geoacc);

  Dumper* truth_dump = new Dumper("truth_dump");
  truth_dump->SetOutDir(".");
  se->registerSubsystem(truth_dump);
*/
  se->run(nEvents);

  se->End();

  gSystem->Exit(0);
}

#endif
