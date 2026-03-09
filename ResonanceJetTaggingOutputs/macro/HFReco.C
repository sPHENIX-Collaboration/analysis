#include <fun4all/Fun4AllServer.h>

#pragma GCC diagnostic push

#pragma GCC diagnostic ignored "-Wundefined-internal"

#include <kfparticle_sphenix/KFParticle_sPHENIX.h>
#include <resonancejettagging/ResonanceJetTagging.h>
#include <fun4all/Fun4AllReturnCodes.h>

#pragma GCC diagnostic pop

R__LOAD_LIBRARY(libkfparticle_sphenix.so)

namespace HFjets
{
namespace Enable
{
  bool KFPARTICLE_SAVE_NTUPLE = false;
  bool KFPARTICLE_APPEND_TO_DST = true;
  bool KFPARTICLE_TRUTH_MATCH = true;
  bool KFPARTICLE_DETECTOR_INFO = false;
  bool KFPARTICLE_CALO_INFO = false;
  bool KFPARTICLE_ALL_PV_INFO = false;
  int KFPARTICLE_VERBOSITY = INT_MAX;
  bool runTracking = false;
  bool runTruthTrigger = true;
  string reconstructionName = "myTestReco";         //Used for naming output folder, file and nodes
}  // namespace Enable


  

namespace KFPARTICLE
{
  bool runQA = false;

  std::string D0Name = "D0";
  std::string D0toK3piName = "D0TOK3PI";
  std::pair<float, float> D0MassRange(1.7, 2.0);
  bool D0Trigger = false;
  bool runD0toKpiReco = false;
  std::string D0toKpiDecayDescriptor = "[" + D0Name + " -> K^- pi^+]cc";
  bool runD0toK3piReco = false;
  std::string D0toK3piDecayDescriptor = "[" + D0Name + " -> K^- pi^- pi^+ pi^+]cc";

  std::string DplusName = "Dplus";
  std::pair<float, float> DplusMassRange(1.7, 2.00);
  bool DplusTrigger = false;
  bool runDplustoK2piReco = false;
  std::string DplustoK2piDecayDescriptor = "[" + DplusName + " -> K^- pi^+ pi^+]cc";

  std::string LambdacName = "Lambdac";
  std::pair<float, float> LambdacMassRange(2.15, 2.45);
  bool LambdacTrigger = false;
  bool runLambdactopKpiReco = false;
  std::string LambdacDecayDescriptor = "[" + LambdacName + " -> proton^+ K^- pi^+]cc";
}

namespace KFParticleBaseCut
{
  bool KFPARTICLE_ALLOW_ZERO_MASS_TRACKS = true;
  bool constrainToPV = true;
  float minTrackPT = 0.7; // GeV
  float maxTrackchi2nDoF = 5; // An 3
  float minTrackIPchi2 = 0; // IP = DCA of track with vertex AuAu 9, An 9
  float minTrackIP = 0.0025; // cm, An 0 AuAu 0.008
  float maxVertexchi2nDoF = 5;
  float maxTrackTrackDCA = 0.008; // cm An 0.03
  float minMotherPT = 1.5; // GeV  An 0
}  // namespace KFParticleBaseCut


int KFParticle_Set_Reco(ResonanceJetTagging::TAG tag)
{

  Fun4AllServer *se = Fun4AllServer::instance();

  std::string particle_name;
  switch (tag) {
    case ResonanceJetTagging::TAG::D0:
      particle_name = KFPARTICLE::D0Name;
      break;
    case ResonanceJetTagging::TAG::D0TOK3PI:
      particle_name = KFPARTICLE::D0toK3piName;
      break;
    case ResonanceJetTagging::TAG::DPLUS:
      particle_name = KFPARTICLE::DplusName;
      break;
    case ResonanceJetTagging::TAG::LAMBDAC:
      particle_name = KFPARTICLE::LambdacName;
      break;
    default:
      std::cout<<"ERROR:decay parameters not set, ABORTING!";
      return Fun4AllReturnCodes::ABORTRUN;
      break;

  }

  KFParticle_sPHENIX *kfparticle = new KFParticle_sPHENIX("KFParticle_" + particle_name + "_Reco");
  
  switch (tag) {
    case ResonanceJetTagging::TAG::D0:
      kfparticle->setDecayDescriptor(KFPARTICLE::D0toKpiDecayDescriptor);
      kfparticle->setContainerName(KFPARTICLE::D0Name);
      kfparticle->setOutputName("KFParticleOutput_" + KFPARTICLE::D0Name + "_reconstruction.root");
      break;
    case ResonanceJetTagging::TAG::D0TOK3PI:
      kfparticle->setDecayDescriptor(KFPARTICLE::D0toK3piDecayDescriptor);
      kfparticle->setContainerName(KFPARTICLE::D0toK3piName);
      kfparticle->setOutputName("KFParticleOutput_" + KFPARTICLE::D0toK3piName + "_reconstruction.root");
      break;
    case ResonanceJetTagging::TAG::DPLUS:
      kfparticle->setDecayDescriptor(KFPARTICLE::DplustoK2piDecayDescriptor);
      kfparticle->setContainerName(KFPARTICLE::DplusName);
      kfparticle->setOutputName("KFParticleOutput_" + KFPARTICLE::DplusName + "_reconstruction.root");
      break;
    case ResonanceJetTagging::TAG::LAMBDAC:
      kfparticle->setDecayDescriptor(KFPARTICLE::LambdacDecayDescriptor);
      kfparticle->setContainerName(KFPARTICLE::LambdacName);
      kfparticle->setOutputName("KFParticleOutput_" + KFPARTICLE::LambdacName + "_reconstruction.root");
      break;
    default:
      std::cout<<"ERROR:decay parameters not set, ABORTING!";
      return Fun4AllReturnCodes::ABORTRUN;
      break;
  }

  kfparticle->Verbosity(Enable::KFPARTICLE_VERBOSITY);
  kfparticle->saveDST(Enable::KFPARTICLE_APPEND_TO_DST);
  kfparticle->saveOutput(Enable::KFPARTICLE_SAVE_NTUPLE);
  kfparticle->doTruthMatching(Enable::KFPARTICLE_TRUTH_MATCH);
  kfparticle->getDetectorInfo(Enable::KFPARTICLE_DETECTOR_INFO);
  kfparticle->getCaloInfo(Enable::KFPARTICLE_CALO_INFO);
  kfparticle->getAllPVInfo(Enable::KFPARTICLE_ALL_PV_INFO);
  

  //Track parameters
  kfparticle->setMinimumTrackPT(KFParticleBaseCut::minTrackPT);
  kfparticle->setMaximumTrackchi2nDOF(KFParticleBaseCut::maxTrackchi2nDoF);
  kfparticle->setMinimumTrackIPchi2(KFParticleBaseCut::minTrackIPchi2);
  kfparticle->setMinimumTrackIP(KFParticleBaseCut::minTrackIP);
  kfparticle->allowZeroMassTracks(KFParticleBaseCut::KFPARTICLE_ALLOW_ZERO_MASS_TRACKS);

  //Vertex parameters
  kfparticle->setMaximumVertexchi2nDOF(KFParticleBaseCut::maxVertexchi2nDoF);
  kfparticle->setMaximumDaughterDCA(KFParticleBaseCut::maxTrackTrackDCA);
  kfparticle->constrainToPrimaryVertex(KFParticleBaseCut::constrainToPV);

  //Parent param
  kfparticle->setMotherPT(KFParticleBaseCut::minMotherPT);

  switch (tag) {
    case ResonanceJetTagging::TAG::D0:
      kfparticle->setMinimumMass(KFPARTICLE::D0MassRange.first);
      kfparticle->setMaximumMass(KFPARTICLE::D0MassRange.second);
      kfparticle->setMinDIRA(0.9); //AuAu 0.98 An 0.98
      kfparticle->setMotherIPchi2(3); 
      kfparticle->setFlightDistancechi2(0);
     // kfparticle->setNumberOfTracks(2);
      kfparticle->setMaximumDaughterDCA(KFParticleBaseCut::maxTrackTrackDCA); //AuAu 0.005 An 0.03
      break;
    case ResonanceJetTagging::TAG::D0TOK3PI:
      kfparticle->setMinimumMass(KFPARTICLE::D0MassRange.first);
      kfparticle->setMaximumMass(KFPARTICLE::D0MassRange.second);
      kfparticle->setMinDIRA(0.95);
      kfparticle->setMotherIPchi2(15); 
      kfparticle->setFlightDistancechi2(0);
     // kfparticle->setNumberOfTracks(4);
      kfparticle->setMaximumDaughterDCA(KFParticleBaseCut::maxTrackTrackDCA);
      break;
    case ResonanceJetTagging::TAG::DPLUS:
      kfparticle->setMinimumMass(KFPARTICLE::DplusMassRange.first);
      kfparticle->setMaximumMass(KFPARTICLE::DplusMassRange.second);
      std::cout<<"ERROR: TAG::DPLUS decay parameters not set, ABORTING!";
      return Fun4AllReturnCodes::ABORTRUN;
      break;
    case ResonanceJetTagging::TAG::LAMBDAC:
      kfparticle->setMinimumMass(KFPARTICLE::LambdacMassRange.first);
      kfparticle->setMaximumMass(KFPARTICLE::LambdacMassRange.second);
      std::cout<<"ERROR: TAG::Lambdac decay parameters not set, ABORTING!";
      return Fun4AllReturnCodes::ABORTRUN;
      break;
    default:
      std::cout<<"ERROR:decay parameters not set, ABORTING!";
      return Fun4AllReturnCodes::ABORTRUN;
      break;
  }

  se->registerSubsystem(kfparticle);

  //KFPARTICLE::runD0Reco = true;

  return 0;
}
} // namespace HFjets

