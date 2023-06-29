#include "HFReco.C"

#include <resonancejettaggingoutputs/BuildResonanceJetTaggingTree.h>

#include <resonancejettagging/ResonanceJetTagging.h>

#include <g4main/Fun4AllDstPileupInputManager.h>

#include <G4_Magnet.C>
#include <G4_Tracking.C>
#include <QA.C>
#include <G4_Global.C>
#include <FROG.h>
#include <decayfinder/DecayFinder.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include <g4eval/SvtxEvaluator.h>
#include <g4eval/SvtxTruthRecoTableEval.h>

#include <caloreco/RawClusterBuilderTopo.h>
#include <particleflowreco/ParticleFlowReco.h>
#include <phool/recoConsts.h>

R__LOAD_LIBRARY(libdecayfinder.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libparticleflow.so)
R__LOAD_LIBRARY(libresonancejettaggingoutputs.so)

using namespace std;

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
/****************************/
/*        Contributor       */
/* Jakub Kvapil, LANL, 2023 */
/*   jakub.kvapil@cern.ch   */
/****************************/

void Fun4All_MDC2reco(vector<string> myInputLists = {"condorJob/fileLists/productionFiles-CHARM-dst_tracks-00000.LIST"}, const int nEvents = 10, ResonanceJetTagging::TAG tag = ResonanceJetTagging::TAG::D0)
{
  int verbosity = 0;

  gSystem->Load("libg4dst.so");
  gSystem->Load("libFROG.so");
  FROG *fr = new FROG();
  recoConsts *rc = recoConsts::instance();
  Enable::CDB = true;
  // global tag
  rc->set_StringFlag("CDB_GLOBALTAG",CDB::global_tag);
  // 64 bit timestamp
  rc->set_uint64Flag("TIMESTAMP",CDB::timestamp);

  std::string particle_name;
  switch (tag) {
    case ResonanceJetTagging::TAG::D0:
      particle_name = HFjets::KFPARTICLE::D0Name;
      break;
    case ResonanceJetTagging::TAG::D0TOK3PI:
      particle_name = HFjets::KFPARTICLE::D0toK3piName;
      break;
    case ResonanceJetTagging::TAG::DPLUS:
      particle_name = HFjets::KFPARTICLE::DplusName;
      break;
    case ResonanceJetTagging::TAG::LAMBDAC:
      particle_name = HFjets::KFPARTICLE::LambdacName;
      break;
    default:
      std::cout<<"ERROR:decay not implemented, ABORTING!";
      return Fun4AllReturnCodes::ABORTRUN;
      break;
  }

  //The next set of lines figures out folder revisions, file numbers etc
  string outDir = "./";
  if (outDir.substr(outDir.size() - 1, 1) != "/") outDir += "/";
  outDir += HFjets::Enable::reconstructionName + "_" + particle_name + "/";

  string fileNumber = myInputLists[0];
  size_t findLastDash = fileNumber.find_last_of("-");
  if (findLastDash != string::npos) fileNumber.erase(0, findLastDash + 1);
  string remove_this = ".list";
  size_t pos = fileNumber.find(remove_this);
  if (pos != string::npos) fileNumber.erase(pos, remove_this.length());
  string outputFileName = "outputData_" + HFjets::Enable::reconstructionName + "_" + fileNumber + ".root";

  string outputRecoDir = outDir + "/inReconstruction/";
  string makeDirectory = "mkdir -p " + outputRecoDir;
  system(makeDirectory.c_str());
  string outputRecoFile = outputRecoDir + outputFileName;

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
  if (HFjets::Enable::runTruthTrigger)
  {
    DecayFinder *myFinder = new DecayFinder("myFinder");
    myFinder->Verbosity(verbosity);
    switch (tag) {
    case ResonanceJetTagging::TAG::D0:
      myFinder->setDecayDescriptor(HFjets::KFPARTICLE::D0toKpiDecayDescriptor);
      break;
    case ResonanceJetTagging::TAG::D0TOK3PI:
      myFinder->setDecayDescriptor(HFjets::KFPARTICLE::D0toK3piDecayDescriptor);
      break;
    case ResonanceJetTagging::TAG::DPLUS:
      myFinder->setDecayDescriptor(HFjets::KFPARTICLE::DplustoK2piDecayDescriptor);
      break;
    case ResonanceJetTagging::TAG::LAMBDAC:
      myFinder->setDecayDescriptor(HFjets::KFPARTICLE::LambdacDecayDescriptor);
      break;
    default:
      std::cout<<"ERROR:decay not implemented, ABORTING!";
      return Fun4AllReturnCodes::ABORTRUN;
      break;
   }
    myFinder->saveDST(true);
    myFinder->allowPi0(false);
    myFinder->allowPhotons(false);
    myFinder->triggerOnDecay(true);
    myFinder->setPTmin(0.); //Note: sPHENIX min pT is 0.2 GeV for tracking
    myFinder->setEtaRange(-10., 10.); //Note: sPHENIX acceptance is |eta| <= 1.1
    se->registerSubsystem(myFinder);
  }

  //Run the tracking if not already done
  if (HFjets::Enable::runTracking)
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

  // Commenting out these 3 lines, as the tables seem to be outdated for this analysis package
  //SvtxTruthRecoTableEval *tables = new SvtxTruthRecoTableEval();
  //tables->Verbosity(verbosity);
  //se->registerSubsystem(tables);

  Global_Reco();

  //Now run the actual reconstruction
  if (HFjets::KFParticle_Set_Reco(tag) == Fun4AllReturnCodes::ABORTRUN) return;

  //Set Calo towers
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


  std::string jetTagRecoFile = outputRecoDir + particle_name + "JetTree_" + fileNumber + ".root";

  ResonanceJetTagging *jetTag = new ResonanceJetTagging(particle_name + "Tagging", tag, particle_name + "_KFParticle_Container");
  jetTag->Verbosity(verbosity);
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
  jetTag->setParticleFlowEtaAcc(-1.7, 1.7); // -1.1 1.1
  jetTag->setJetParameters(0.4, ResonanceJetTagging::ALGO::ANTIKT, ResonanceJetTagging::RECOMB::E_SCHEME);
  jetTag->setJetContainerName(particle_name + "Jets");
  jetTag->setDoTruth(true);
  se->registerSubsystem(jetTag);

  BuildResonanceJetTaggingTree *buildTree = new BuildResonanceJetTaggingTree(particle_name + "JetTree", jetTagRecoFile, tag);
  buildTree->setDoTruth(true);
  buildTree->setTagContainerName(particle_name + "_KFParticle_Container");
  buildTree->setJetContainerName(particle_name + "Jets_Jet_Container");
  buildTree->setTruthJetContainerName(particle_name + "Jets_Truth_Jet_Container");
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
