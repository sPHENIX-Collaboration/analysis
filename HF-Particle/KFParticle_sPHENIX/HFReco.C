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
  string reconstructionName = "myTestReco";         //Used for naming output folder, file and node
  string outputRecoFile;
  string outputHFEffFile;
  bool runTruthTrigger = true;  //Decay Finder
  bool runTrackEff = true;  //HF track efficiency
  bool getTruthInfo = true;      //Add truth matching to output file
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

  //if (runTrackEff) kfparticle->setTrackMapNodeName("HFSelected_SvtxTrackMap");

  kfparticle->doTruthMatching(getTruthInfo);
  kfparticle->getDetectorInfo(false);
  kfparticle->getCaloInfo(getCaloInfo);
  kfparticle->getAllPVInfo(false);
  kfparticle->allowZeroMassTracks(true);
  kfparticle->saveDST(false);
  kfparticle->saveParticleContainer(false);

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
    kfparticle->setMotherIPchi2(3);
    kfparticle->setFlightDistancechi2(0.);
    kfparticle->setMinDIRA(0.90);
  }

  //Track parameters
  kfparticle->setMinimumTrackPT(0.7);
  kfparticle->setMinimumTrackIPchi2(0.0);
  kfparticle->setMinimumTrackIP(-0.01);
  kfparticle->setMaximumTrackchi2nDOF(3.);

  //Vertex parameters
  kfparticle->setMaximumVertexchi2nDOF(5);
  kfparticle->setMaximumDaughterDCA(0.005);

  //Parent parameters
  kfparticle->setMotherPT(0);
  kfparticle->setMinimumMass(1.70);
  kfparticle->setMaximumMass(2.00);
  kfparticle->setDecayLengthRange(0.0035, 0.025);
  kfparticle->setMaximumMotherVertexVolume(0.03);

  //Intermediate parameters
  std::vector<std::pair<float, float>> intermediate_mass_range;
  intermediate_mass_range.push_back(make_pair(0.4, 0.6));
  kfparticle->setIntermediateMassRange(intermediate_mass_range);
  std::vector<float> intermediate_min_pt = {0.5};
  kfparticle->setIntermediateMinPT(intermediate_min_pt);
  std::vector<std::pair<float, float>> intermediate_IP_range;
  intermediate_IP_range.push_back(make_pair(0., 1.));
  kfparticle->setIntermediateIPRange(intermediate_IP_range);
  std::vector<std::pair<float, float>> intermediate_IPchi2_range;
  intermediate_IPchi2_range.push_back(make_pair(0., 100.));
  kfparticle->setIntermediateIPchi2Range(intermediate_IPchi2_range);
  std::vector<float> intermediate_min_dira = {0.5};
  kfparticle->setIntermediateMinDIRA(intermediate_min_dira);
  std::vector<float> intermediate_min_FDchi2 = {0.};
  kfparticle->setIntermediateMinFDchi2(intermediate_min_FDchi2);

  kfparticle->setOutputName(outputRecoFile);

  se->registerSubsystem(kfparticle);

}
