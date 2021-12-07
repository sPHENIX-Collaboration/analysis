#include <kfparticle_sphenix/KFParticle_sPHENIX.h>

namespace HeavyFlavorReco
{
  // https://wiki.bnl.gov/sPHENIX/index.php/KFParticle
  string decayDescriptor = "[D0 -> K^- pi^+]cc"; //See twiki on how to set this
  string reconstructionName = "myHeavyFlavorReco"; //Used for naming output folder, file and nodes
  string outputRecoFile;
  bool runTruthTrigger = false; //Decay Finder
  bool getTruthInfo = false; //Add truth matching to output file
  bool getCaloInfo = false;
  bool runTracking = false; //Run tracking on DSTs
  bool runPileUp = false; //Set true to have pile up added to reco
  bool runQA = false; //Run QA, needs set up
  int VERBOSITY = INT_MAX;
};

using namespace HeavyFlavorReco;

void myHeavyFlavorReco()
{
  int verbosity = VERBOSITY;

  Fun4AllServer *se = Fun4AllServer::instance();

  KFParticle_sPHENIX *kfparticle = new KFParticle_sPHENIX(reconstructionName);
  kfparticle->Verbosity(verbosity);

  kfparticle->setDecayDescriptor(decayDescriptor);

  kfparticle->doTruthMatching(getTruthInfo);
  kfparticle->getDetectorInfo(false);
  kfparticle->getCaloInfo(getCaloInfo);
  kfparticle->getAllPVInfo(false);
  kfparticle->allowZeroMassTracks(true);

  bool fixToPV = true;
  bool useFakePV = false;

  if (useFakePV)
  {
    fixToPV = false; //Constraining to a fake PV results in some gibberish variables
    kfparticle->useFakePrimaryVertex(true);
  }

  if (fixToPV)
  {
    kfparticle->constrainToPrimaryVertex(true);
    kfparticle->setMotherIPchi2(50);
    kfparticle->setFlightDistancechi2(10.0);
    kfparticle->setMinDIRA(-1.0);
  }

  //Track parameters
  kfparticle->setMinimumTrackPT(0.2);
  kfparticle->setMinimumTrackIPchi2(2);
  kfparticle->setMinimumTrackIP(0.00);
  kfparticle->setMaximumTrackchi2nDOF(3);

  //Vertex parameters
  kfparticle->setMaximumVertexchi2nDOF(3);
  kfparticle->setMaximumDaughterDCA(0.03);

  //Parent parameters
  kfparticle->setMotherPT(1);
  kfparticle->setMinimumMass(1.7);
  kfparticle->setMaximumMass(2.0);

  kfparticle->setOutputName(outputRecoFile);

  se->registerSubsystem(kfparticle);
}
