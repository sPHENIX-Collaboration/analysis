#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <g4main/Fun4AllDstPileupInputManager.h>
#include <qa_modules/QAG4SimulationKFParticle.h>
#include <decayfinder/DecayFinder.h>
#include <G4_Magnet.C>
#include <G4_Tracking.C>
#include <QA.C>
#include <FROG.h>
#include "HFReco.C"

R__LOAD_LIBRARY(libqa_modules.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libkfparticle_sphenix.so)

using namespace std;
using namespace HeavyFlavorReco;

/****************************/
/*     HF Reco for MDC2     */
/* Cameron Dean, LANL, 2021 */
/*      cdean@bnl.gov       */
/****************************/

void Fun4All_HFreco(string trackList = "track.list"
                  , string truthList = "truth.list"
                  , string caloList = "calo.list"
                  , string pileupList = "pileup.list"
                  , const int nEvents = 0)
{
  int verbosity = VERBOSITY;

  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4dst.so");
  FROG *fr = new FROG();

  //The next set of lines figures out folder revisions, file numbers etc
  string outDir = "./";
  if (outDir.substr(outDir.size() - 1, 1) != "/") outDir += "/";
  outDir += reconstructionName + "/";

  string fileNumber = trackList;
  size_t findLastDash = fileNumber.find_last_of("-");
  if (findLastDash != string::npos) fileNumber.erase(0, findLastDash + 1);
  string remove_this = ".list";
  size_t pos = fileNumber.find(remove_this);
  if (pos != string::npos) fileNumber.erase(pos, remove_this.length());
  string outputFileName = "outputData_" + reconstructionName + "_" + fileNumber + ".root";

  string outputRecoDir = outDir + "/inReconstruction/";
  string makeDirectory = "mkdir -p " + outputRecoDir;
  system(makeDirectory.c_str());
  outputRecoFile = outputRecoDir + outputFileName;

  //Create the server
  Fun4AllServer* se = Fun4AllServer::instance();
  se->Verbosity(verbosity);

  //Add all required input files
  Fun4AllInputManager *tracks = new Fun4AllDstInputManager("tracks");
  tracks->AddListFile(trackList);
  se->registerInputManager(tracks);

  if (getTruthInfo)
  {
    Fun4AllInputManager *truth = new Fun4AllDstInputManager("truth");
    truth->AddListFile(truthList);
    se->registerInputManager(truth);
  }

  if (getCaloInfo)
  {
    Fun4AllInputManager *calo = new Fun4AllDstInputManager("calo");
    calo->AddListFile(caloList);
    se->registerInputManager(calo);
  }
  
  if (runPileUp)
  {
    Fun4AllDstPileupInputManager *pileup = new Fun4AllDstPileupInputManager("pileup");
    pileup->setCollisionRate(3e6); // 3MHz according to BUP
    pileup->AddListFile(pileupList);
    se->registerInputManager(pileup);
  }

  // Runs decay finder to trigger on your decay. Useful for signal cleaning
  if (runTruthTrigger)
  {
    DecayFinder *myFinder = new DecayFinder(reconstructionName);
    myFinder->Verbosity(verbosity);
    myFinder->setDecayDescriptor(decayDescriptor);
    myFinder->saveDST(runQA);
    myFinder->allowPi0(true);
    myFinder->allowPhotons(true);
    myFinder->triggerOnDecay(true);
    se->registerSubsystem(myFinder);
  }

  //Run the tracking if not already done
  if (runTracking)
  {
    G4MAGNET::magfield_rescale = 1.;
    MagnetInit();
    MagnetFieldInit();
    
    Mvtx_Cells();
    Intt_Cells();
    TPC_Cells();
    Micromegas_Cells();

    TrackingInit();

    Mvtx_Clustering();
    Intt_Clustering();
    TPC_Clustering();
    Micromegas_Clustering();

    Tracking_Reco();
  }

  //Now run the actual reconstruction
  myHeavyFlavorReco(); 

  //We should set this up correctly
  if (runQA)
  {
    QAG4SimulationKFParticle *myQA = new QAG4SimulationKFParticle("myQA", "D0", 1.7, 2.0);
    se->registerSubsystem(myQA);
    QA_Output("hf_qa.root");
  }

  se->run(nEvents);
  se->End();

  string moveOutput = "mv " + outputRecoFile + " " + outDir;
  system(moveOutput.c_str());
  
  std::cout << "All done" << std::endl;
  delete se;
  gSystem->Exit(0);

  return;
}
