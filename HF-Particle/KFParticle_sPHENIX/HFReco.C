#include <fun4all/Fun4AllServer.h>

#pragma GCC diagnostic push

#pragma GCC diagnostic ignored "-Wundefined-internal"

#include <kfparticle_sphenix/KFParticle_sPHENIX.h>

#pragma GCC diagnostic pop

R__LOAD_LIBRARY(libkfparticle_sphenix.so)

namespace HeavyFlavorReco
{
  // https://wiki.bnl.gov/sPHENIX/index.php/KFParticle
  string decayDescriptor = "[D0 -> K^- pi^+]cc";  //See twiki on how to set this
  string reconstructionName = "myD2KpiReco";         //Used for naming output folder, file and node
  string outputRecoFile;
  string outputHFEffFile;
  bool runTruthTrigger = false;  //Decay Finder
  bool runTrackEff = false;  //HF track efficiency
  bool getTruthInfo = false;      //Add truth matching to output file
  bool getCaloInfo = false;
  bool runTracking = false;  //Run tracking on DSTs
  bool buildTruthTable = false;
  bool runQA = false;        //Run QA, needs set up
  int VERBOSITY = 0;
};  // namespace HeavyFlavorReco

using namespace HeavyFlavorReco;

void myHeavyFlavorReco()
{
  Fun4AllServer *se = Fun4AllServer::instance();

  KFParticle_sPHENIX *kfparticle = new KFParticle_sPHENIX(reconstructionName);
  kfparticle->Verbosity(1);

  kfparticle->setDecayDescriptor(decayDescriptor);

  if (runTrackEff) kfparticle->setTrackMapNodeName("HFSelected_SvtxTrackMap");

  kfparticle->doTruthMatching(getTruthInfo);
  kfparticle->getDetectorInfo(false);
  kfparticle->getCaloInfo(getCaloInfo);
  kfparticle->getAllPVInfo(false);
  kfparticle->allowZeroMassTracks(true);
  kfparticle->saveDST(false);
  kfparticle->saveParticleContainer(false);

  bool fixToPV = false;
  bool useFakePV = true;

  if (useFakePV)
  {
    fixToPV = false;  //Constraining to a fake PV results in some gibberish variables
    kfparticle->useFakePrimaryVertex(true);
  }

  if (fixToPV)
  {
    kfparticle->constrainToPrimaryVertex(true);
    kfparticle->setMotherIPchi2(3);
    kfparticle->setFlightDistancechi2(-1.);
    kfparticle->setMinDIRA(0.90);
    kfparticle->setDecayLengthRange(-1*FLT_MAX, FLT_MAX);
  }

  //Track parameters
  kfparticle->setMinimumTrackPT(0.7);
  kfparticle->setMinimumTrackIPchi2(-1.);
  kfparticle->setMinimumTrackIP(-1.);
  kfparticle->setMaximumTrackchi2nDOF(3.);

  //Vertex parameters
  kfparticle->setMaximumVertexchi2nDOF(5);
  kfparticle->setMaximumDaughterDCA(0.005);

  //Parent parameters
  kfparticle->setMotherPT(0);
  kfparticle->setMinimumMass(1.50);
  kfparticle->setMaximumMass(2.20);
  kfparticle->setMaximumMotherVertexVolume(0.03);

  kfparticle->setOutputName(outputRecoFile);

  se->registerSubsystem(kfparticle);
}

void myDemoReco()
{
  Fun4AllServer *se = Fun4AllServer::instance();

  KFParticle_sPHENIX *kfparticle = new KFParticle_sPHENIX(reconstructionName);
  kfparticle->Verbosity(1);

  kfparticle->setDecayDescriptor("[D+ -> {phi -> K^+ K^-} pi^+]cc");
  //kfparticle->setDecayDescriptor("B_s0 -> {J/psi -> e^+ e^-} {K_S0 -> pi^+ pi^-}");
  kfparticle->setTrackMapNodeName("HFSelected_SvtxTrackMap");

  kfparticle->doTruthMatching(true);
  kfparticle->getCaloInfo(false);
  kfparticle->allowZeroMassTracks(true);
  kfparticle->constrainToPrimaryVertex(true);
  kfparticle->useFakePrimaryVertex(false);
  kfparticle->setOutputName("Ds2KKpi.root");

  //Track parameters
  kfparticle->setMinimumTrackPT(0.);
  kfparticle->setMaximumTrackPTchi2(FLT_MAX);
  kfparticle->setMinimumTrackIPchi2(-1.);
  kfparticle->setMinimumTrackIP(-1.);
  kfparticle->setMaximumTrackchi2nDOF(100);
  kfparticle->setMinTPChits(0);

  //Vertex parameters
  kfparticle->setMaximumVertexchi2nDOF(100);
  kfparticle->setMaximumDaughterDCA(0.1);

  //Parent parameters
  kfparticle->setMotherPT(0);
  kfparticle->setMinimumMass(1.5);
  kfparticle->setMaximumMass(2.2);
  kfparticle->setMaximumMotherVertexVolume(0.1);
  kfparticle->setMotherIPchi2(FLT_MAX);
  kfparticle->setFlightDistancechi2(-1.);
  kfparticle->setMinDIRA(-1.1);
  kfparticle->setDecayLengthRange(-1, 100);

  //Intermediate parameters
  std::vector<std::pair<float, float>> intermediate_mass_range;
  intermediate_mass_range.push_back(make_pair(0.8, 1.2));
  //intermediate_mass_range.push_back(make_pair(0.4, 0.6));
  kfparticle->setIntermediateMassRange(intermediate_mass_range);

  std::vector<float> intermediate_min_pt = {0.0};//, 0.0};
  kfparticle->setIntermediateMinPT(intermediate_min_pt);

  std::vector<std::pair<float, float>> intermediate_IP_range;
  intermediate_IP_range.push_back(make_pair(-1., 5.));
  //intermediate_IP_range.push_back(make_pair(-1., 5.));
  kfparticle->setIntermediateIPRange(intermediate_IP_range);

  std::vector<std::pair<float, float>> intermediate_IPchi2_range;
  intermediate_IPchi2_range.push_back(make_pair(0., 400.));
  //intermediate_IPchi2_range.push_back(make_pair(0., 400.));
  kfparticle->setIntermediateIPchi2Range(intermediate_IPchi2_range);

  std::vector<float> intermediate_min_dira = {-1.1};//, -1.1};
  kfparticle->setIntermediateMinDIRA(intermediate_min_dira);

  std::vector<float> intermediate_min_FDchi2 = {-1.};//,, -1.};
  kfparticle->setIntermediateMinFDchi2(intermediate_min_FDchi2);

  std::vector<float> intermediate_max_vertex_vol = {1.1};//,, 0.9};
  kfparticle->setIntermediateMaxVertexVolume(intermediate_max_vertex_vol);

  se->registerSubsystem(kfparticle);
}
