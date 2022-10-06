#include "HFReco.C"

#include <g4main/Fun4AllDstPileupInputManager.h>

#include <G4_Magnet.C>
#include <G4_Tracking.C>
#include <QA.C>

#include <FROG.h>
#include <decayfinder/DecayFinder.h>
#include <fun4all/Fun4AllDstInputManager.h>
//#include <qa_modules/QAG4SimulationKFParticle.h>
//#include <qa_modules/QAG4SimulationTruthDecay.h>

#include <g4eval/SvtxEvaluator.h>
#include <g4eval/SvtxTruthRecoTableEval.h>

//R__LOAD_LIBRARY(libqa_modules.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libdecayfinder.so)

using namespace std;
using namespace HeavyFlavorReco;

/****************************/
/*     MDC2 Reco for MDC2   */
/* Cameron Dean, LANL, 2021 */
/*      cdean@bnl.gov       */
/****************************/

void Fun4All_MDC2reco(vector<string> myInputLists = {"condorJob/fileLists/productionFiles-CHARM-dst_tracks-00000.list"}, const int nEvents = 10)
{
  gSystem->Load("libg4dst.so");
  gSystem->Load("libFROG.so");
  FROG *fr = new FROG();

  //The next set of lines figures out folder revisions, file numbers etc
  string outDir = "./";
  if (outDir.substr(outDir.size() - 1, 1) != "/") outDir += "/";
  outDir += reconstructionName + "/";

  string fileNumber = myInputLists[0];
  size_t findLastDash = fileNumber.find_last_of("-");
  if (findLastDash != string::npos) fileNumber.erase(0, findLastDash + 1);
  string remove_this = ".list";
  size_t pos = fileNumber.find(remove_this);
  if (pos != string::npos) fileNumber.erase(pos, remove_this.length());
  string outputFileName = "outputTruthDecayTest_" + reconstructionName + "_" + fileNumber + ".root";
  string outputEvalFileName = "outputEvaluator_" + reconstructionName + "_" + fileNumber + ".root";

  string outputRecoDir = outDir + "inReconstruction/";
  string makeDirectory = "mkdir -p " + outputRecoDir;
  system(makeDirectory.c_str());
  outputRecoFile = outputRecoDir + outputFileName;
  outputEvalFile = outputRecoDir + outputEvalFileName;

  //Create the server
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(VERBOSITY);

  //Add all required input files
  for (unsigned int i = 0; i < myInputLists.size(); ++i)
  {
    Fun4AllInputManager *infile = new Fun4AllDstInputManager("DSTin_" + to_string(i));
    infile->AddListFile(myInputLists[i]);
    se->registerInputManager(infile);
  }

  //Run the tracking if not already done
  if (runTracking)
  {
    Enable::MICROMEGAS=true;

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

  // Runs decay finder to trigger on your decay. Useful for signal cleaning
  if (runTruthTrigger)
  {
    DecayFinder *myFinder = new DecayFinder("myFinder");
    myFinder->Verbosity(VERBOSITY);
    myFinder->setDecayDescriptor(decayDescriptor);
    myFinder->saveDST(1);
    myFinder->allowPi0(0);
    myFinder->allowPhotons(0);
    myFinder->triggerOnDecay(1);
    myFinder->setPTmin(0.); //Note: sPHENIX min pT is 0.2 GeV for tracking
    myFinder->setEtaRange(-10., 10.); //Note: sPHENIX acceptance is |eta| <= 1.1
    se->registerSubsystem(myFinder);
  } 

  // this module builds high level truth track association table.
  // If this module is used, this table should be called before any evaluator calls.
  // Removing this module, evaluation will still work but trace truth association through the layers of G4-hit-cluster
  if (buildTruthTable && runTracking)
  {
    SvtxTruthRecoTableEval *tables = new SvtxTruthRecoTableEval();
    tables->Verbosity(VERBOSITY);
    se->registerSubsystem(tables);
  }
  else if (buildTruthTable && !runTracking)
  {
    cout << "You requested to build the truth reco track table but the tracking is not being run" << endl;
    cout << "Normally you must run the tracking before you build the truth tables, exiting now!" << endl;
    cout << "If your DSTs are capable of creating the association tables without rerunning the tracking, comment this exit command out!" << endl;
    exit(1);
  }
  else
  {
  }

  //Now run the actual reconstruction
  myHeavyFlavorReco();

  //We should set this up correctly
  //if (runQA)
  //{
  //  QAG4SimulationKFParticle *myQA = new QAG4SimulationKFParticle("myQA", "D0", 1.7, 2.0);
  //  se->registerSubsystem(myQA);
  //  QA_Output("hf_qa.root");
  //}

  se->run(nEvents);
  //QA_Output(outputEvalFile);
  se->End();

  ifstream file(outputRecoFile.c_str());
  if (file.good())
  {
    string moveOutput = "mv " + outputRecoFile + " " + outDir;
    system(moveOutput.c_str());
  }

  //ifstream evalfile(outputEvalFile.c_str());
  //if (evalfile.good())
  //{
  //  string moveOutput = "mv " + outputEvalFile + " " + outDir;
  //  system(moveOutput.c_str());
  //}

  std::cout << "All done" << std::endl;
  delete se;
  gSystem->Exit(0);

  return;
}
