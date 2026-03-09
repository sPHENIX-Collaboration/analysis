#include <g4main/Fun4AllDstPileupInputManager.h>

#include <G4_Magnet.C>
#include <G4_Tracking.C>
#include <QA.C>

#include <FROG.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include <g4eval/SvtxEvaluator.h>
#include <g4eval/SvtxTruthRecoTableEval.h>

#include <kfparticle_sphenix/KFParticle_sPHENIX.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libkfparticle_sphenix.so)

using namespace std;

/****************************/
/*     MDC2 Reco for MDC2   */
/* Cameron Dean, LANL, 2021 */
/*      cdean@bnl.gov       */
/****************************/

void CommAnaTrackSelection(vector<string> myInputLists = {"/sphenix/u/rosstom/analysis/FilterEvents/macro/test.root"}, const int nEvents = 10)
{
  //Create the server
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(INT_MAX);

  //Add all required input files
  for (unsigned int i = 0; i < myInputLists.size(); ++i)
  {
    Fun4AllInputManager *infile = new Fun4AllDstInputManager("DSTin_" + to_string(i));
    infile->fileopen(myInputLists[i]);
    se->registerInputManager(infile);
  }

  //auto in = new Fun4AllDstInputManager("DSTin1");
  //in->fileopen(myInputLists[0]);
  //se->registerInputManager(in);

  Enable::MVTX = true;
  Enable::INTT = true;
  Enable::TPC = true;
  Enable::TPC_ABSORBER = true;
  Enable::MICROMEGAS=true;
  
  G4MAGNET::magfield_rescale = 1.;
  MagnetInit();
  MagnetFieldInit();

  //Mvtx_Cells();
  //Intt_Cells();
  //TPC_Cells();
  //Micromegas_Cells();
  
  TrackingInit();
 
  Tracking_Reco_TrackFit(); //PHACTSTrackFitter, cut out this section  

  string decayDescriptor = "[D0 -> K^- pi^+]cc"; 

  KFParticle_sPHENIX *kfparticle = new KFParticle_sPHENIX("CommAnaTrackSelection");
  kfparticle->Verbosity(INT_MAX);

  kfparticle->setDecayDescriptor(decayDescriptor);

  kfparticle->doTruthMatching(false);
  kfparticle->getDetectorInfo(false);
  kfparticle->getCaloInfo(false);
  kfparticle->getAllPVInfo(false);
  kfparticle->allowZeroMassTracks(true);
  kfparticle->saveDST(true);
  kfparticle->saveParticleContainer(true);
  kfparticle->saveOutput(true); //false in final system, true for debugging

  bool fixToPV = true;
  bool useFakePV = false;

  if (useFakePV)
  {
    fixToPV = false;  //Constraining to a fake PV results in some gibberish variables
    kfparticle->useFakePrimaryVertex(true);
  }

  if (fixToPV)
  {
    kfparticle->constrainToPrimaryVertex(true);
    kfparticle->setMotherIPchi2(5000);
    kfparticle->setFlightDistancechi2(-1.);
    kfparticle->setMinDIRA(-1.1);
  }

  //Track parameters
  kfparticle->setMinimumTrackPT(0.0);
  kfparticle->setMinimumTrackIPchi2(-1.0);
  kfparticle->setMinimumTrackIP(-0.01);
  kfparticle->setMaximumTrackchi2nDOF(200);

  kfparticle->setMaximumVertexchi2nDOF(200);
  kfparticle->setMaximumDaughterDCA(1);

  //Parent parameters
  kfparticle->setMotherPT(0);
  kfparticle->setMinimumMass(1.7);
  kfparticle->setMaximumMass(2.1);
 
  kfparticle->setOutputName("outputKFP.root"); 

  se->registerSubsystem(kfparticle); 

  se->run(nEvents);
  
  se->End();

  std::cout << "All done" << std::endl;
  delete se;
  gSystem->Exit(0);

  return;
}
