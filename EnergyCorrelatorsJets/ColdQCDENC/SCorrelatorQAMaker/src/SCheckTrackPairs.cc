// ----------------------------------------------------------------------------
// 'SCheckTrackPairs.cc'
// Derek Anderson
// 102.21.2024
//
// SCorrelatorQAMaker plugin to iterate through
// all pairs of tracks in an event and fill
// tuples/histograms comparing them.
// ----------------------------------------------------------------------------

#define SCORRELATORQAMAKER_SCHECKTRACKPAIRS_CC

// plugin definition
#include "SCheckTrackPairs.h"

// make common namespaces implicit
using namespace std;
using namespace findNode;



namespace SColdQcdCorrelatorAnalysis {

  // SCheckTrackPairs public methods ------------------------------------------

  int SCheckTrackPairs::Init(PHCompositeNode* topNode) {

    InitOutput();
    InitTuples();
    return Fun4AllReturnCodes::EVENT_OK;

  }  // end 'Init(PHCompositeNode*)'



  int SCheckTrackPairs::process_event(PHCompositeNode* topNode) {

    ResetVectors();
    DoDoubleTrackLoop(topNode);
    return Fun4AllReturnCodes::EVENT_OK;

  }  // end 'process_event(PHCompositeNode* topNode)'



  int SCheckTrackPairs::End(PHCompositeNode* topNode) {

    SaveOutput();
    CloseOutput();
    return Fun4AllReturnCodes::EVENT_OK;

  }  // end 'End(PHCompositeNode*)'



  // SCheckTrackPairs internal methods ----------------------------------------

  void SCheckTrackPairs::InitTuples() {

    if (m_isDebugOn && (m_verbosity > 2)) {
      cout << "SColdQcdCorrelatorAnalysis::SCheckTrackPairs::InitTuples(): initializing output tuple." << endl;
    }

    // create leaf lists and add leaves
    vector<string> vecTrkPairLeavesA = Types::TrkInfo::GetListOfMembers();
    vector<string> vecTrkPairLeavesB = Types::TrkInfo::GetListOfMembers();
    Interfaces::AddTagToLeaves("_a", vecTrkPairLeavesA); 
    Interfaces::AddTagToLeaves("_b", vecTrkPairLeavesB); 
    vecTrkPairLeavesA.push_back("nClustKey_a");
    vecTrkPairLeavesB.push_back("nClustKey_b");

    // combine lists
    vector<string> vecTrkPairLeaves;
    Interfaces::CombineLeafLists(vecTrkPairLeavesA, vecTrkPairLeaves);
    Interfaces::CombineLeafLists(vecTrkPairLeavesB, vecTrkPairLeaves);

    // add leaves for no. of same cluster keys b/n pair and
    // distance between pair
    vecTrkPairLeaves.push_back("nSameClustKey");
    vecTrkPairLeaves.push_back("trackDeltaR");

    // compress leaves into a colon-separated list
    string argTrkPairLeaves = Interfaces::FlattenLeafList(vecTrkPairLeaves);

    // create tuple and return
    m_ntTrackPairs = new TNtuple("ntTrackPairs", "Pairs of tracks", argTrkPairLeaves.data());
    m_vecTrackPairLeaves.reserve(vecTrkPairLeaves.size());
    return;

  }  // end 'InitTuples()'



  void SCheckTrackPairs::SaveOutput() {

    if (m_isDebugOn && (m_verbosity > 2)) {
      cout << "SColdQcdCorrelatorAnalysis::SCheckTrackPairs::SaveOutput(): saving output." << endl;
    }

    m_outDir       -> cd();
    m_ntTrackPairs -> Write();
    return;

  }  // end 'SaveOutput()'



  void SCheckTrackPairs::ResetVectors() {

    if (m_isDebugOn && (m_verbosity > 4)) {
      cout << "SColdQcdCorrelatorAnalysis::SCheckTrackPairs::ResetVectors(): resetting vectors." << endl;
    }

    // set leaf values to a default
    const size_t nLeaves = m_vecTrackPairLeaves.size();
    for (size_t iLeaf = 0; iLeaf < nLeaves; iLeaf++) {
      m_vecTrackPairLeaves[iLeaf] = -999.;
    }

    // clear cluster keys
    m_vecClustKeysA.clear();
    m_vecClustKeysB.clear();
    return;

  }  // end 'ResetVectors()'



  void SCheckTrackPairs::DoDoubleTrackLoop(PHCompositeNode* topNode) {

    if (m_isDebugOn && (m_verbosity > 2)) {
      cout << "SColdQcdCorrelatorAnalysis::SCheckTrackPairs::DoDoubleTrackLoop(): looping over all pairs of tracks." << endl;
    }

    // loop over tracks
    SvtxTrack*    trackA  = NULL;
    SvtxTrack*    trackB  = NULL;
    SvtxTrackMap* mapTrks = Interfaces::GetTrackMap(topNode);
    for (
      SvtxTrackMap::Iter itTrkA = mapTrks -> begin();
      itTrkA != mapTrks -> end();
      ++itTrkA
    ) {

      // grab track A and skip if bad
      trackA = itTrkA -> second;
      if (!trackA) continue;

      const bool isGoodTrackA = IsGoodTrack(trackA, topNode);
      if (!isGoodTrackA) continue;

      // grab track info
      Types::TrkInfo trkInfoA(trackA, topNode);

      // loop over tracks again
      for (SvtxTrackMap::Iter itTrkB = mapTrks -> begin(); itTrkB != mapTrks -> end(); ++itTrkB) {

        // grab track B and skip if bad
        trackB = itTrkB -> second;
        if (!trackB) continue;

        const bool isGoodTrackB = IsGoodTrack(trackB, topNode);
        if (!isGoodTrackB) continue;

        // skip if same as track A
        if ((trackA -> get_id()) == (trackB -> get_id())) continue;

        // grab track info
        Types::TrkInfo trkInfoB(trackB, topNode);

        // calculate delta-R
        const double dfTrkAB = trkInfoA.GetPhi() - trkInfoB.GetPhi();
        const double dhTrkAB = trkInfoA.GetEta() - trkInfoB.GetEta();
        const double drTrkAB = sqrt((dfTrkAB * dfTrkAB) + (dhTrkAB * dhTrkAB));

        // clear vectors for checking cluster keys
        m_vecClustKeysA.clear();
        m_vecClustKeysB.clear();

        // loop over clusters from track A
        auto seedTpcA = trackA -> get_tpc_seed();
        if (seedTpcA) {
          for (auto local_iterA = seedTpcA -> begin_cluster_keys(); local_iterA != seedTpcA -> end_cluster_keys(); ++local_iterA) {
            TrkrDefs::cluskey cluster_keyA = *local_iterA;
            m_vecClustKeysA.push_back(cluster_keyA);
          }
        }

        // loop over clusters from track B
        auto seedTpcB = trackB -> get_tpc_seed();
        if (seedTpcB) {
          for (auto local_iterB = seedTpcB -> begin_cluster_keys(); local_iterB != seedTpcB -> end_cluster_keys(); ++local_iterB) {
            TrkrDefs::cluskey cluster_keyB = *local_iterB;
            m_vecClustKeysB.push_back(cluster_keyB);
          }
        }

        // calculate no. of same cluster keys
        uint64_t nSameKey = 0;
        for (auto keyA : m_vecClustKeysA) {
          for (auto keyB : m_vecClustKeysB) {
            if (keyA == keyB) {
              ++nSameKey;
              break;
            }
          }  // end cluster key B loop
        }  // end cluster key A loop

        // set tuple leaves
        m_vecTrackPairLeaves[0]  = (float) trkInfoA.GetID();
        m_vecTrackPairLeaves[1]  = (float) trkInfoA.GetNMvtxLayer();
        m_vecTrackPairLeaves[2]  = (float) trkInfoA.GetNInttLayer();
        m_vecTrackPairLeaves[3]  = (float) trkInfoA.GetNTpcLayer();
        m_vecTrackPairLeaves[4]  = (float) trkInfoA.GetNMvtxClust();
        m_vecTrackPairLeaves[5]  = (float) trkInfoA.GetNInttClust();
        m_vecTrackPairLeaves[6]  = (float) trkInfoA.GetNTpcClust();
        m_vecTrackPairLeaves[7]  = (float) trkInfoA.GetEta();
        m_vecTrackPairLeaves[8]  = (float) trkInfoA.GetPhi();
        m_vecTrackPairLeaves[9]  = (float) trkInfoA.GetPX();
        m_vecTrackPairLeaves[10] = (float) trkInfoA.GetPY();
        m_vecTrackPairLeaves[11] = (float) trkInfoA.GetPZ();
        m_vecTrackPairLeaves[12] = (float) trkInfoA.GetPT();
        m_vecTrackPairLeaves[13] = (float) trkInfoA.GetEne();
        m_vecTrackPairLeaves[14] = (float) trkInfoA.GetDcaXY();
        m_vecTrackPairLeaves[15] = (float) trkInfoA.GetDcaZ();
        m_vecTrackPairLeaves[16] = (float) trkInfoA.GetPtErr();
        m_vecTrackPairLeaves[17] = (float) trkInfoA.GetQuality();
        m_vecTrackPairLeaves[18] = (float) trkInfoA.GetVX();
        m_vecTrackPairLeaves[19] = (float) trkInfoA.GetVY();
        m_vecTrackPairLeaves[20] = (float) trkInfoA.GetVZ();
        m_vecTrackPairLeaves[21] = (float) m_vecClustKeysA.size();
        m_vecTrackPairLeaves[22] = (float) trkInfoB.GetID();
        m_vecTrackPairLeaves[23] = (float) trkInfoB.GetNMvtxLayer();
        m_vecTrackPairLeaves[24] = (float) trkInfoB.GetNInttLayer();
        m_vecTrackPairLeaves[25] = (float) trkInfoB.GetNTpcLayer();
        m_vecTrackPairLeaves[26] = (float) trkInfoB.GetNMvtxClust();
        m_vecTrackPairLeaves[27] = (float) trkInfoB.GetNInttClust();
        m_vecTrackPairLeaves[28] = (float) trkInfoB.GetNTpcClust();
        m_vecTrackPairLeaves[29] = (float) trkInfoB.GetEta();
        m_vecTrackPairLeaves[30] = (float) trkInfoB.GetPhi();
        m_vecTrackPairLeaves[31] = (float) trkInfoB.GetPX();
        m_vecTrackPairLeaves[32] = (float) trkInfoB.GetPY();
        m_vecTrackPairLeaves[33] = (float) trkInfoB.GetPZ();
        m_vecTrackPairLeaves[34] = (float) trkInfoB.GetPT();
        m_vecTrackPairLeaves[35] = (float) trkInfoB.GetEne();
        m_vecTrackPairLeaves[36] = (float) trkInfoB.GetDcaXY();
        m_vecTrackPairLeaves[37] = (float) trkInfoB.GetDcaZ();
        m_vecTrackPairLeaves[38] = (float) trkInfoB.GetPtErr();
        m_vecTrackPairLeaves[39] = (float) trkInfoB.GetQuality();
        m_vecTrackPairLeaves[40] = (float) trkInfoB.GetVX();
        m_vecTrackPairLeaves[41] = (float) trkInfoB.GetVY();
        m_vecTrackPairLeaves[42] = (float) trkInfoB.GetVZ();
        m_vecTrackPairLeaves[43] = (float) m_vecClustKeysB.size();
        m_vecTrackPairLeaves[44] = (float) nSameKey;
        m_vecTrackPairLeaves[45] = (float) drTrkAB;

        // fill track pair tuple
        m_ntTrackPairs -> Fill(m_vecTrackPairLeaves.data());

      }  // end 2nd track loop
    }  // end track loop
    return;

  }  // end 'DoDoubleTrackLoop(PHCompositeNode*)'



  bool SCheckTrackPairs::IsGoodTrack(SvtxTrack* track, PHCompositeNode* topNode) {

    // print debug statement
    if (m_isDebugOn && (m_verbosity > 4)) {
      cout << "SCheckTrackPairs::IsGoodTrack(SvtxTrack* track, PHCompositeNode*) Checking if track is good..." << endl;
    }

    // grab info
    Types::TrkInfo info(track, topNode);

    // if needed, check if dca is in pt-dependent range
    bool isInDcaSigma = true;
    if (m_config.doDcaSigCut) {
      isInDcaSigma = info.IsInSigmaDcaCut(m_config.nSigCut, m_config.ptFitMax, m_config.fSigDca);
    }

    // if needed, check if track is from primary vertex
    const bool isFromPrimVtx = m_config.useOnlyPrimVtx ? Tools::IsFromPrimaryVtx(track, topNode) : true;

    // check if seed is good & track is in acceptance
    const bool isSeedGood = Tools::IsGoodTrackSeed(track, m_config.requireSiSeed);
    const bool isInAccept = info.IsInAcceptance(m_config.trkAccept);

    // return overall goodness of track
    return (isFromPrimVtx && isInDcaSigma && isSeedGood && isInAccept);

  }  // end 'IsGoodTrack(SvtxTrack*, PHCompositeNode* topNode)'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------

