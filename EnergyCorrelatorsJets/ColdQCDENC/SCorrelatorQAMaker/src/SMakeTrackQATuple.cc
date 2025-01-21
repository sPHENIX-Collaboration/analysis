// ----------------------------------------------------------------------------
// 'SMakeTrackQATuple.cc'
// Derek Anderson
// 03.10.2024
//
// SCorrelatorQAMaker plugin to produce QA tuples
// for tracks.
// ----------------------------------------------------------------------------

#define SCORRELATORQAMAKER_SMAKETRACKQATUPLE_CC

// plugin definition
#include "SMakeTrackQATuple.h"

// make common namespaces implicit
using namespace std;
using namespace findNode;



namespace SColdQcdCorrelatorAnalysis {

  // SMakeTrackQATuple public methods ------------------------------------------

  int SMakeTrackQATuple::Init(PHCompositeNode* topNode) {

    InitOutput();
    InitTuple();
    return Fun4AllReturnCodes::EVENT_OK;

  }  // end 'Init(PHCompositeNode*)'



  int SMakeTrackQATuple::process_event(PHCompositeNode* topNode) {

    DoTrackLoop(topNode);
    return Fun4AllReturnCodes::EVENT_OK;

  }  // end 'process_event(PHCompositeNode* topNode)'



  int SMakeTrackQATuple::End(PHCompositeNode* topNode) {

    SaveOutput();
    CloseOutput();
    return Fun4AllReturnCodes::EVENT_OK;

  }  // end 'End(PHCompositeNode*)'



  // SMakeTrackQATuple internal methods -----------------------------------------

  void SMakeTrackQATuple::InitTuple() {

    if (m_isDebugOn && (m_verbosity > 2)) {
      cout << "SColdQcdCorrelatorAnalysis::SMakeTrackQATuple::InitTuple(): initializing output tuple." << endl;
    }

    // create leaf lists for event and track info
    vector<string> vecRecLeaves = Types::RecoInfo::GetListOfMembers();
    vector<string> vecGenLeaves = Types::GenInfo::GetListOfMembers();
    vector<string> vecTrkLeaves = Types::TrkInfo::GetListOfMembers();

    // combine leaf lists
    vector<string> vecOutLeaves;
    Interfaces::CombineLeafLists(vecRecLeaves, vecOutLeaves);
    Interfaces::CombineLeafLists(vecGenLeaves, vecOutLeaves);
    Interfaces::CombineLeafLists(vecTrkLeaves, vecOutLeaves);

    // compress leaves into a color-separated list
    string argOutLeaves = Interfaces::FlattenLeafList(vecOutLeaves);

    // create tuple and return
    m_ntTrackQA = new TNtuple("ntTrackQA", "Track QA", argOutLeaves.data());
    m_vecTrackLeaves.reserve(vecOutLeaves.size());
    return;

  }  // end 'InitTuple()'



  void SMakeTrackQATuple::SaveOutput() {

    if (m_isDebugOn && (m_verbosity > 2)) {
      cout << "SColdQcdCorrelatorAnalysis::SMakeTrackQATuple::SaveOutput(): saving output." << endl;
    }

    m_outDir    -> cd();
    m_ntTrackQA -> Write();
    return;

  }  // end 'SaveOutput()'



  void SMakeTrackQATuple::DoTrackLoop(PHCompositeNode* topNode) {

    if (m_isDebugOn && (m_verbosity > 2)) {
      cout << "SColdQcdCorrelatorAnalysis::SMakeTrackQATuple::DoTrackLoop(PHCompositeNode*): looping over tracks." << endl;
    }

    // grab event info
    //   - FIXME add in subevent selection
    Types::RecoInfo recInfo(topNode);
    Types::GenInfo  genInfo(topNode, m_config.isEmbed, {2});

    // loop over tracks
    SvtxTrack*    track   = NULL;
    SvtxTrackMap* mapTrks = Interfaces::GetTrackMap(topNode);
    for (
      SvtxTrackMap::Iter itTrk = mapTrks -> begin();
      itTrk != mapTrks -> end();
      ++itTrk
    ) {

      // grab track and skip if bad
      track = itTrk -> second;
      if (!track) continue;

      const bool isGoodTrack = IsGoodTrack(track, topNode);
      if (!isGoodTrack) continue;

      // grab track info
      Types::TrkInfo trkInfo(track, topNode);

      // set tuple leaves
      m_vecTrackLeaves[0]  = (float) recInfo.GetNTrks();
      m_vecTrackLeaves[1]  = (float) recInfo.GetPSumTrks();
      m_vecTrackLeaves[2]  = (float) recInfo.GetESumEMCal();
      m_vecTrackLeaves[3]  = (float) recInfo.GetESumIHCal();
      m_vecTrackLeaves[4]  = (float) recInfo.GetESumOHCal();
      m_vecTrackLeaves[5]  = (float) recInfo.GetVX();
      m_vecTrackLeaves[6]  = (float) recInfo.GetVY();
      m_vecTrackLeaves[7]  = (float) recInfo.GetVZ();
      m_vecTrackLeaves[8]  = (float) genInfo.GetNChrgPar();
      m_vecTrackLeaves[9]  = (float) genInfo.GetNNeuPar();
      m_vecTrackLeaves[10] = (float) genInfo.GetIsEmbed();
      m_vecTrackLeaves[11] = (float) genInfo.GetESumChrg();
      m_vecTrackLeaves[12] = (float) genInfo.GetESumNeu();
      m_vecTrackLeaves[13] = (float) genInfo.GetPartons().first.GetPID();
      m_vecTrackLeaves[14] = (float) genInfo.GetPartons().first.GetStatus();
      m_vecTrackLeaves[15] = (float) genInfo.GetPartons().first.GetBarcode();
      m_vecTrackLeaves[16] = (float) genInfo.GetPartons().first.GetEmbedID();
      m_vecTrackLeaves[17] = (float) genInfo.GetPartons().first.GetCharge();
      m_vecTrackLeaves[18] = (float) genInfo.GetPartons().first.GetMass();
      m_vecTrackLeaves[19] = (float) genInfo.GetPartons().first.GetEta();
      m_vecTrackLeaves[20] = (float) genInfo.GetPartons().first.GetPhi();
      m_vecTrackLeaves[21] = (float) genInfo.GetPartons().first.GetEne();
      m_vecTrackLeaves[22] = (float) genInfo.GetPartons().first.GetPX();
      m_vecTrackLeaves[23] = (float) genInfo.GetPartons().first.GetPY();
      m_vecTrackLeaves[24] = (float) genInfo.GetPartons().first.GetPZ();
      m_vecTrackLeaves[25] = (float) genInfo.GetPartons().first.GetPT();
      m_vecTrackLeaves[26] = (float) genInfo.GetPartons().first.GetVX();
      m_vecTrackLeaves[27] = (float) genInfo.GetPartons().first.GetVY();
      m_vecTrackLeaves[28] = (float) genInfo.GetPartons().first.GetVZ();
      m_vecTrackLeaves[29] = (float) genInfo.GetPartons().second.GetPID();
      m_vecTrackLeaves[30] = (float) genInfo.GetPartons().second.GetStatus();
      m_vecTrackLeaves[31] = (float) genInfo.GetPartons().second.GetBarcode();
      m_vecTrackLeaves[32] = (float) genInfo.GetPartons().second.GetEmbedID();
      m_vecTrackLeaves[33] = (float) genInfo.GetPartons().second.GetCharge();
      m_vecTrackLeaves[34] = (float) genInfo.GetPartons().second.GetMass();
      m_vecTrackLeaves[35] = (float) genInfo.GetPartons().second.GetEta();
      m_vecTrackLeaves[36] = (float) genInfo.GetPartons().second.GetPhi();
      m_vecTrackLeaves[37] = (float) genInfo.GetPartons().second.GetEne();
      m_vecTrackLeaves[38] = (float) genInfo.GetPartons().second.GetPX();
      m_vecTrackLeaves[39] = (float) genInfo.GetPartons().second.GetPY();
      m_vecTrackLeaves[40] = (float) genInfo.GetPartons().second.GetPZ();
      m_vecTrackLeaves[41] = (float) genInfo.GetPartons().second.GetPT();
      m_vecTrackLeaves[42] = (float) genInfo.GetPartons().second.GetVX();
      m_vecTrackLeaves[43] = (float) genInfo.GetPartons().second.GetVY();
      m_vecTrackLeaves[44] = (float) genInfo.GetPartons().second.GetVZ();
      m_vecTrackLeaves[45] = (float) trkInfo.GetID();
      m_vecTrackLeaves[46] = (float) trkInfo.GetNMvtxLayer();
      m_vecTrackLeaves[47] = (float) trkInfo.GetNInttLayer();
      m_vecTrackLeaves[48] = (float) trkInfo.GetNTpcLayer();
      m_vecTrackLeaves[49] = (float) trkInfo.GetNMvtxClust();
      m_vecTrackLeaves[50] = (float) trkInfo.GetNInttClust();
      m_vecTrackLeaves[51] = (float) trkInfo.GetNTpcClust();
      m_vecTrackLeaves[52] = (float) trkInfo.GetEta();
      m_vecTrackLeaves[53] = (float) trkInfo.GetPhi();
      m_vecTrackLeaves[54] = (float) trkInfo.GetPX();
      m_vecTrackLeaves[55] = (float) trkInfo.GetPY();
      m_vecTrackLeaves[56] = (float) trkInfo.GetPZ();
      m_vecTrackLeaves[57] = (float) trkInfo.GetPT();
      m_vecTrackLeaves[58] = (float) trkInfo.GetEne();
      m_vecTrackLeaves[59] = (float) trkInfo.GetDcaXY();
      m_vecTrackLeaves[60] = (float) trkInfo.GetDcaZ();
      m_vecTrackLeaves[61] = (float) trkInfo.GetPtErr();
      m_vecTrackLeaves[62] = (float) trkInfo.GetQuality();
      m_vecTrackLeaves[63] = (float) trkInfo.GetVX();
      m_vecTrackLeaves[64] = (float) trkInfo.GetVY();
      m_vecTrackLeaves[65] = (float) trkInfo.GetVZ();

      // fill track tuple
      m_ntTrackQA -> Fill(m_vecTrackLeaves.data());

    }  // end track loop
    return;

  }  // end 'DoTrackLoop(PHCompositeNode*)'



  bool SMakeTrackQATuple::IsGoodTrack(SvtxTrack* track, PHCompositeNode* topNode) {

    // print debug statement
    if (m_isDebugOn && (m_verbosity > 4)) {
      cout << "SMakeTrackQATuple::IsGoodTrack(SvtxTrack*, PHCompositeNode*) Checking if track is good..." << endl;
    }

    // grab track info
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

  }  // end 'IsGoodTrack(SvtxTrack* track, PHCompositeNode* topNode)'

}  // end SColdQcdCorrelatorAnalysis namespace

// end -----------------------------------------------------------------------
