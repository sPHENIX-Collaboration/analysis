/// ---------------------------------------------------------------------------
/*! \file   SCorrelatorJetTreeMaker.h
 *  \author Derek Anderson
 *  \date   12.04.2022
 *
 *  A module to produce a tree of jets for the sPHENIX
 *  Cold QCD Energy-Energy Correlator analysis. Initially
 *  derived from code by Antonio Silva.
 */
/// ---------------------------------------------------------------------------

#ifndef SCORRELATORJETTREEMAKER_H
#define SCORRELATORJETTREEMAKER_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

// c++ utilities
#include <map>
#include <string>
#include <vector>
#include <cassert>
#include <cstdlib>
#include <utility>
// root libraries
#include <TF1.h>
#include <TFile.h>
#include <TTree.h>
#include <Math/Vector3D.h>
// fastjet libraries
#include <fastjet/PseudoJet.hh>
#include <fastjet/JetDefinition.hh>
#include <fastjet/ClusterSequence.hh>
#include <fastjet/FunctionOfPseudoJet.hh>
// hepmc libraries
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>
#include <HepMC/GenParticle.h>
// f4a libraries
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllHistoManager.h>
// phool libraries
#include <phool/phool.h>
#include <phool/getClass.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHCompositeNode.h>
// truth libraries
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>
// jet libraries
#include <jetbase/Jet.h>
#include <jetbase/FastJetAlgo.h>
#include <jetbase/JetContainer.h>
// calo libraries
#include <calobase/RawCluster.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoContainer.h>
#include <calotrigger/CaloTriggerInfo.h>
// tracking utilities
#include <g4eval/SvtxTrackEval.h>
#include <g4eval/SvtxEvalStack.h>
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/TrackAnalysisUtils.h>
// particle flow libraries
#include <particleflowreco/ParticleFlowElement.h>
#include <particleflowreco/ParticleFlowElementContainer.h>
// vtx libraries
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
// analysis utilities
#include "scorrelatorutilities/Tools.h"
#include "scorrelatorutilities/Types.h"
#include "scorrelatorutilities/Constants.h"
#include "scorrelatorutilities/Interfaces.h"
// analysis definitions
#include "SCorrelatorJetTreeMakerConfig.h"
#include "SCorrelatorJetTreeMakerOutput.h"

#pragma GCC diagnostic pop

// make common namespaces implicit
using namespace std;
using namespace fastjet;



namespace SColdQcdCorrelatorAnalysis {

  // --------------------------------------------------------------------------
  //! Correlator jet tree maker
  // --------------------------------------------------------------------------
  /*! A module to produce compact trees of jets and their
   *  constituents for downstream ENC analysis.  Can either
   *  reconstruct jets from provided tracks, clusters, etc.
   *  or read in a node of already-constructed jets.
   */ 
  class SCorrelatorJetTreeMaker : public SubsysReco {

    public:

      // ctor/dtor
      SCorrelatorJetTreeMaker(const string& name = "SCorrelatorJetTreeMaker", const bool debug = false);
      SCorrelatorJetTreeMaker(SCorrelatorJetTreeMakerConfig& config);
      ~SCorrelatorJetTreeMaker() override;

      // F4A methods
      int Init(PHCompositeNode*)          override;
      int process_event(PHCompositeNode*) override;
      int End(PHCompositeNode*)           override;

      // setters
      void SetConfig(const SCorrelatorJetTreeMakerConfig& config) {m_config = config;}

      // getters
      SCorrelatorJetTreeMakerConfig GetConfig() const {return m_config;}

    private:

      // analysis methods (*.ana.h)
      void GetEventVariables(PHCompositeNode* topNode);
      void GetJetVariables(PHCompositeNode* topNode);
      void ReadJetNodes(PHCompositeNode* topNode);
      void ReadGivenJetNode(const string node, PHCompositeNode* topNode);
      void MakeJets(PHCompositeNode* topNode);
      void MakeRecoJets(PHCompositeNode* topNode);
      void MakeTrueJets(PHCompositeNode* topNode);
      void AddTracks(PHCompositeNode* topNode);
      void AddFlow(PHCompositeNode* topNode);
      void AddTowers(PHCompositeNode* topNode, vector<Const::Subsys> vecSubsysToAdd);
      void AddClusters(PHCompositeNode* topNode, vector<Const::Subsys> vecSubsysToAdd);
      void AddParticles(PHCompositeNode* topNode);
      bool IsGoodTrack(Types::TrkInfo& info, SvtxTrack* track, PHCompositeNode* topNode);
      bool IsGoodFlow(Types::FlowInfo& info);
      bool IsGoodTower(Types::TwrInfo& info, const Const::Subsys subsys);
      bool IsGoodCluster(Types::ClustInfo& info, const Const::Subsys subsys);
      bool IsGoodParticle(Types::ParInfo& info);
      bool IsGoodVertex(const ROOT::Math::XYZVector vtx);
      int  GetRecoCstType();

      // system methods (*.sys.h)
      void OpenOutFile();
      void InitTrees();
      void InitFastJet();
      void InitEvals(PHCompositeNode* topNode);
      void FillTrees();
      void SaveOutput();
      void CloseOutFile();
      void ResetSysVariables();
      void ResetOutVariables();
      void ResetJetVariables();

      // configuration
      SCorrelatorJetTreeMakerConfig m_config;

      // outputs
      SCorrelatorJetTreeMakerRecoOutput  m_recoOutput;
      SCorrelatorJetTreeMakerTruthOutput m_trueOutput;
      SJetTreeMakerRecoOutputInterface   m_recoInterface;
      SJetTreeMakerTruthOutputInterface  m_trueInterface;

      // io members
      TFile*        m_outFile  = NULL;
      TTree*        m_recoTree = NULL;
      TTree*        m_trueTree = NULL;
      JetContainer* m_recoNode = NULL;
      JetContainer* m_trueNode = NULL;

      // track evaluator members
      SvtxEvalStack* m_evalStack = NULL;
      SvtxTrackEval* m_trackEval = NULL;

      // system members
      vector<int>   m_vecEvtsToGrab;
      map<int, int> m_mapCstToEmbedID;

      // jet members
      //   - TODO add area definition
      unique_ptr<JetDefinition>   m_recoJetDef  = NULL;
      unique_ptr<JetDefinition>   m_trueJetDef  = NULL;
      unique_ptr<ClusterSequence> m_recoCluster = NULL;
      unique_ptr<ClusterSequence> m_trueCluster = NULL;

      // vectors/maps for jet finding
      vector<PseudoJet> m_recoJets;
      vector<PseudoJet> m_trueJets;
      vector<PseudoJet> m_recoJetInput;
      vector<PseudoJet> m_trueJetInput;

  };

}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------
