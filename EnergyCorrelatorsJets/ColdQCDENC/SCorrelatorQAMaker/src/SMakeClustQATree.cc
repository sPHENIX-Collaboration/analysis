// ----------------------------------------------------------------------------
// 'SMakeClustQATree.cc'
// Derek Anderson
// 03.10.2024
//
// SCorrelatorQAMaker plugin to produce the QA tree
// for calorimeter clusters.
// ----------------------------------------------------------------------------

#define SCORRELATORQAMAKER_SMAKECLUSTQATREE_CC

// plugin definition
#include "SMakeClustQATree.h"

// make common namespaces implicit
using namespace std;
using namespace findNode;



namespace SColdQcdCorrelatorAnalysis {



  // SMakeClustQATree public methods ------------------------------------------

  int SMakeClustQATree::Init(PHCompositeNode* topNode) {

    InitOutput();
    InitTree();
    return Fun4AllReturnCodes::EVENT_OK;

  }  // end 'Init(PHCompositeNode*)'



  int SMakeClustQATree::process_event(PHCompositeNode* topNode) {

    // make sure output container is empty
    m_output.Reset();

    // grab event info
    //   FIXME add in subevent selection
    m_output.recInfo.SetInfo(topNode);
    m_output.genInfo.SetInfo(topNode, m_config.isEmbed, {2});

    // grab cluster info
    DoClustLoop(topNode, "CLUSTER_CEMC");
    DoClustLoop(topNode, "CLUSTER_HCALIN");
    DoClustLoop(topNode, "CLUSTER_HCALOUT");

    // fill output tree and reset
    m_tClustQA -> Fill();
    return Fun4AllReturnCodes::EVENT_OK;

  }  // end 'process_event(PHCompositeNode* topNode)'



  int SMakeClustQATree::End(PHCompositeNode* topNode) {

    SaveOutput();
    CloseOutput();
    return Fun4AllReturnCodes::EVENT_OK;

  }  // end 'End(PHCompositeNode*)'



  // SMakeClustQATree internal methods -----------------------------------------

  void SMakeClustQATree::InitTree() {

    if (m_isDebugOn && (m_verbosity > 2)) {
      cout << "SColdQcdCorrelatorAnalysis::SMakeClustQATree::InitTree(): initializing output tree." << endl;
    }

    // initialize tree
    //   FIXME still having issues with vectors of types
    m_tClustQA = new TTree("tClustQA", "Cluster QA");
    m_tClustQA -> Branch("EvtRecoInfo", "Types::RecoInfo", &m_output.recInfo, 6400, 99);
    m_tClustQA -> Branch("EvtGenInfo", "Types::GenInfo", &m_output.genInfo, 6400, 99);
    m_tClustQA -> Branch("EMCalInfo", "vector<Types::ClustInfo>", &m_output.emCalInfo, 6400, 99);
    m_tClustQA -> Branch("IHCalInfo", "vector<Types::ClustInfo>", &m_output.ihCalInfo, 6400, 99);
    m_tClustQA -> Branch("OHCalInfo", "vector<Types::ClustInfo>", &m_output.ohCalInfo, 6400, 99);
    return;

  }  // end 'InitTree()'



  void SMakeClustQATree::SaveOutput() {

    if (m_isDebugOn && (m_verbosity > 2)) {
      cout << "SColdQcdCorrelatorAnalysis::SMakeClustQATree::SaveOutput(): saving output." << endl;
    }

    m_outDir   -> cd();
    m_tClustQA -> Write();
    return;

  }  // end 'SaveOutput()'



  void SMakeClustQATree::DoClustLoop(PHCompositeNode* topNode, const string node) {

    if (m_isDebugOn && (m_verbosity > 2)) {
      cout << "SColdQcdCorrelatorAnalysis::SMakeClustQATree::DoClustLoop(PHCompositeNode*, string): looping over clusters." << endl;
    }

    // grab clusters
    RawClusterContainer::ConstRange clusters = Interfaces::GetClusters(topNode, node);

    // loop over clusters
    for (
      RawClusterContainer::ConstIterator itClust = clusters.first;
      itClust != clusters.second;
      itClust++
    ) {

      // grab cluster
      const RawCluster* cluster = itClust -> second;
      if (!cluster) continue;

      // skip if bad
      const bool isGoodClust = IsGoodCluster(cluster);
      if (!isGoodClust) continue;

      // grab cluster info
      Types::ClustInfo clustInfo(cluster, ROOT::Math::XYZVector(0., 0., 0.), Const::MapNodeOntoIndex()[node]);

      // add to relevant list
      switch (Const::MapNodeOntoIndex()[node]) {

        case Const::Subsys::EMCal:
          m_output.emCalInfo.push_back(clustInfo);
          break;

        case Const::Subsys::IHCal:
          m_output.ihCalInfo.push_back(clustInfo);
          break;

        case Const::Subsys::OHCal:
          m_output.ohCalInfo.push_back(clustInfo);
          break;

        default:
          cerr << "SColdQcdCorrealtorAnalysis::SMakeClustQATree::DoClustLoop(PHCompositeNode*, string) WARNING: trying to add clusters from unknown node to output!" << endl;
          break;
      }
    }  // end cluster loop
    return;

  }  // end 'DoClustLoop(PHCompositeNode*, string)'



  bool SMakeClustQATree::IsGoodCluster(const RawCluster* cluster) {

    // print debug statement
    if (m_isDebugOn && (m_verbosity > 4)) {
      cout << "SMakeClustQATree::IsGoodCluster(RawCluster*) Checking if cluster is good..." << endl;
    }

    // grab cluster info
    Types::ClustInfo info(cluster);

    // check if cluster is in acceptance and return overall goodness
    const bool isInAccept = info.IsInAcceptance(m_config.clustAccept);
    return isInAccept;

  }  // end 'IsGoodTrack(RawCluster*)'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------
