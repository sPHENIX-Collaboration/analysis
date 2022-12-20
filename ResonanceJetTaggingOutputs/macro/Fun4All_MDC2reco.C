R__LOAD_LIBRARY(libdecayfinder.so)
#include "HFReco.C"
#include "resonancejettaggingoutputs/BuildResonanceJetTaggingTree.h"

#include <resonancejettagging/ResonanceJetTagging.h>

#include <g4main/Fun4AllDstPileupInputManager.h>

#include <G4_Magnet.C>
#include <G4_Tracking.C>
#include <QA.C>

#include <FROG.h>
#include <decayfinder/DecayFinder.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include <g4eval/SvtxEvaluator.h>
#include <g4eval/SvtxTruthRecoTableEval.h>

#include <caloreco/RawClusterBuilderTopo.h>
#include <particleflowreco/ParticleFlowReco.h>

R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libparticleflow.so)
R__LOAD_LIBRARY(resonancejettagging.so)

using namespace std;
using namespace HeavyFlavorReco;

/****************************/
/*     MDC2 Reco for MDC2   */
/* Cameron Dean, LANL, 2021 */
/*      cdean@bnl.gov       */
/****************************/
/****************************/
/*   Modified for D0Jets    */
/* Antonio Silva, ISU, 2022 */
/*antonio.sphenix@gmail.com */
/****************************/

void Fun4All_MDC2reco(vector<string> myInputLists = {"condorJob/fileLists/productionFiles-CHARM-dst_tracks-00000.list"}, const int nEvents = 10)
{
  int verbosity = 0;

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
  string outputFileName = "outputData_" + reconstructionName + "_" + fileNumber + ".root";

  string outputRecoDir = outDir + "/inReconstruction/";
  string makeDirectory = "mkdir -p " + outputRecoDir;
  system(makeDirectory.c_str());
  outputRecoFile = outputRecoDir + outputFileName;

  //Create the server
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(verbosity);

  //Add all required input files
  for (unsigned int i = 0; i < myInputLists.size(); ++i)
  {
    Fun4AllInputManager *infile = new Fun4AllDstInputManager("DSTin_" + to_string(i));
    infile->AddListFile(myInputLists[i]);
    se->registerInputManager(infile);
  }

  // Runs decay finder to trigger on your decay. Useful for signal cleaning
  if (runTruthTrigger)
  {
    DecayFinder *myFinder = new DecayFinder("myFinder");
    myFinder->Verbosity(verbosity);
    myFinder->setDecayDescriptor(decayDescriptor);
    myFinder->saveDST(true);
    myFinder->allowPi0(false);
    myFinder->allowPhotons(false);
    myFinder->triggerOnDecay(true);
    myFinder->setPTmin(0.); //Note: sPHENIX min pT is 0.2 GeV for tracking
    myFinder->setEtaRange(-10., 10.); //Note: sPHENIX acceptance is |eta| <= 1.1
    se->registerSubsystem(myFinder);
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

  SvtxTruthRecoTableEval *tables = new SvtxTruthRecoTableEval();

  tables->Verbosity(0);

  se->registerSubsystem(tables);

  //Now run the actual reconstruction
  myHeavyFlavorReco();

  RawClusterBuilderTopo* ClusterBuilder1 = new RawClusterBuilderTopo("HcalRawClusterBuilderTopo1");
  ClusterBuilder1->Verbosity(verbosity);
  ClusterBuilder1->set_nodename("TOPOCLUSTER_EMCAL");
  ClusterBuilder1->set_enable_HCal(false);
  ClusterBuilder1->set_enable_EMCal(true);
  ClusterBuilder1->set_noise(0.0025, 0.006, 0.03);
  ClusterBuilder1->set_significance(4.0, 2.0, 0.0);
  ClusterBuilder1->allow_corner_neighbor(true);
  ClusterBuilder1->set_do_split(true);
  ClusterBuilder1->set_minE_local_max(1.0, 2.0, 0.5);
  ClusterBuilder1->set_R_shower(0.025);
  se->registerSubsystem(ClusterBuilder1);

  RawClusterBuilderTopo* ClusterBuilder2 = new RawClusterBuilderTopo("HcalRawClusterBuilderTopo2");
  ClusterBuilder2->Verbosity(verbosity);
  ClusterBuilder2->set_nodename("TOPOCLUSTER_HCAL");
  ClusterBuilder2->set_enable_HCal(true);
  ClusterBuilder2->set_enable_EMCal(false);
  ClusterBuilder2->set_noise(0.0025, 0.006, 0.03);
  ClusterBuilder2->set_significance(4.0, 2.0, 0.0);
  ClusterBuilder2->allow_corner_neighbor(true);
  ClusterBuilder2->set_do_split(true);
  ClusterBuilder2->set_minE_local_max(1.0, 2.0, 0.5);
  ClusterBuilder2->set_R_shower(0.025);
  se->registerSubsystem(ClusterBuilder2);

  ParticleFlowReco *pfr = new ParticleFlowReco();
  pfr->set_energy_match_Nsigma(1.5);
  pfr->Verbosity(verbosity);
  se->registerSubsystem(pfr);

  std::string jetTagRecoFile = outputRecoDir + "D0JetTree_" + fileNumber + ".root";

  ResonanceJetTagging *jetTag = new ResonanceJetTagging("D0Tagging", ResonanceJetTagging::TAG::D0);
  jetTag->Verbosity(1);
  /*
  jetTag->setAddTracks(false);
  jetTag->setAddEMCalClusters(false);
  jetTag->setTrackPtAcc(0.2, 9999.);
  jetTag->setTrackEtaAcc(-1.1, 1.1);
  jetTag->setEMCalClusterPtAcc(0.3, 9999.);
  jetTag->setEMCalClusterEtaAcc(-1.1, 1.1);
  jetTag->setHCalClusterPtAcc(0.3, 9999.);
  jetTag->setHCalClusterEtaAcc(-1.1, 1.1);
  */
  jetTag->setParticleFlowEtaAcc(-1.1, 1.1);
  jetTag->setJetParameters(0.3, ResonanceJetTagging::ALGO::ANTIKT, ResonanceJetTagging::RECOMB::PT_SCHEME);
  jetTag->setJetContainerName("D0Jets");
  jetTag->setDoTruth(true);
  se->registerSubsystem(jetTag);

  BuildResonanceJetTaggingTree *buildTree = new BuildResonanceJetTaggingTree("D0JetTree", jetTagRecoFile, ResonanceJetTagging::TAG::D0);
  buildTree->setDoTruth(true);
  buildTree->setTagContainerName("reconstructedParticles_KFParticle_Container");
  buildTree->setJetContainerName("D0Jets_Jet_Container");
  buildTree->setTruthJetContainerName("D0Jets_Truth_Jet_Container");
  se->registerSubsystem(buildTree);

  se->run(nEvents);
  se->End();

  ifstream file(outputRecoFile.c_str());
  if (file.good())
  {
    std::string moveOutput = "mv " + outputRecoFile + " " + outDir;
    system(moveOutput.c_str());
  }

  ifstream jetTagfile(jetTagRecoFile.c_str());
  if(jetTagfile.good())
  {
    std::string moveOutput = "mv " + jetTagRecoFile + " " + outDir;
    system(moveOutput.c_str());
  }

  std::cout << "All done" << std::endl;
  delete se;
  gSystem->Exit(0);

  return;
}
