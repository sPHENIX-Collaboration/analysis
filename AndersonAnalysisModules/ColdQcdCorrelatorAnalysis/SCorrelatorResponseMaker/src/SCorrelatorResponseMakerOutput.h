/// ---------------------------------------------------------------------------
/*! \file   SCorrelatorResponseMakerOutput.h
 *  \author Derek Anderson
 *  \date   05.20.2024
 *
 *  Outputs for the `SCorrelatorResponseMaker' module.
 */
/// ---------------------------------------------------------------------------

#ifndef SCORRELATORJETTREEMAKEROUTPUT_H
#define SCORRELATORJETTREEMAKEROUTPUT_H

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // --------------------------------------------------------------------------
  //! Response tree output
  // --------------------------------------------------------------------------
  struct SCorrelatorResponseMakerOutput {

    // event level info
    Types::GenInfo  evtGen;
    Types::RecoInfo evtRec;

    // jet level info
    vector<Types::JetInfo> jetGen;
    vector<Types::JetInfo> jetRec;

    // constituent level info
    vector<vector<Types::CstInfo>> cstGen;
    vector<vector<Types::CstInfo>> cstRec;

    // ------------------------------------------------------------------------
    //! Reset variables
    // ------------------------------------------------------------------------
    void Reset()
      evtGen.Reset();
      evtRec.Reset();
      jetGen.clear();
      jetRec.clear();
      cstGen.clear();
      cstRec.clear();
      return;
    }  // end 'Reset()'

    // ------------------------------------------------------------------------
    //! Set tree addresses
    // ------------------------------------------------------------------------
    void SetTreeAddresses(TTree* tree) {
      /* TODO fill in */
      return;
    }  // end 'SetTreeAddresses(TTree*)'

  };  // end SCorrelatorResponseMakerOutput



  // --------------------------------------------------------------------------
  //! Legacy response tree output
  // --------------------------------------------------------------------------
  struct SCorrelatorResponseMakerLegacyOutput {

    // event level info 
    //   - TODO add non-pair values
    pair<int,    int>    numJets = {numeric_limits<int>::max(),    numeric_limits<int>::max()};
    pair<int,    int>    numTrks = {numeric_limits<int>::max(),    numeric_limits<int>::max()};
    pair<double, double> vtxX    = {numeric_limits<double>::max(), numeric_limits<double>::max()};
    pair<double, double> vtxY    = {numeric_limits<double>::max(), numeric_limits<double>::max()};
    pair<double, double> vtxZ    = {numeric_limits<double>::max(), numeric_limits<double>::max()};

    // jet level info
    pair<vector<uint32_t>, vector<uint32_t>> jetID;
    pair<vector<uint64_t>, vector<uint64_t>> jetNumCst;
    pair<vector<double>,   vector<double>>   jetEne;
    pair<vector<double>,   vector<double>>   jetPt;
    pair<vector<double>,   vector<double>>   jetEta;
    pair<vector<double>,   vector<double>>   jetPhi;
    pair<vector<double>,   vector<double>>   jetArea;

    // output response tree cst-level address members
    pair<vector<vector<int>>,    vector<vector<int>>>    cstID;
    pair<vector<vector<double>>, vector<vector<double>>> cstZ;
    pair<vector<vector<double>>, vector<vector<double>>> cstDr;
    pair<vector<vector<double>>, vector<vector<double>>> cstEne;
    pair<vector<vector<double>>, vector<vector<double>>> cstJt;
    pair<vector<vector<double>>, vector<vector<double>>> cstEta;
    pair<vector<vector<double>>, vector<vector<double>>> cstPhi;



    // ------------------------------------------------------------------------
    //! Reset variables
    // ------------------------------------------------------------------------
    void Reset() {

      numJets = {numeric_limits<int>::max(),    numeric_limits<int>::max()};
      numTrks = {numeric_limits<int>::max(),    numeric_limits<int>::max()};
      vtxX    = {numeric_limits<double>::max(), numeric_limits<double>::max()};
      vtxY    = {numeric_limits<double>::max(), numeric_limits<double>::max()};
      vtxZ    = {numeric_limits<double>::max(), numeric_limits<double>::max()};
      jetID.first.clear();
      jetID.second.clear();
      jetNumCst.first.clear();
      jetNumCst.second.clear();
      jetEne.first.clear();
      jetEne.second.clear();
      jetPt.first.clear();
      jetPt.second.clear();
      jetEta.first.clear();
      jetEta.second.clear();
      jetPhi.first.clear();
      jetPhi.second.clear();
      jetArea.first.clear();
      jetArea.second.clear();
      cstID.first.clear();
      cstID.second.clear();
      cstZ.first.clear();
      cstZ.second.clear();
      cstDr.first.clear();
      cstDr.second.clear();
      cstEne.first.clear();
      cstEne.second.clear();
      cstJt.first.clear();
      cstJt.second.clear();
      cstEta.first.clear();
      cstEta.second.clear();
      cstPhi.first.clear();
      cstPhi.second.clear();
      return;

    }  // end 'Reset()'



    // ------------------------------------------------------------------------
    //! Set tree addresses
    // ------------------------------------------------------------------------
    void SetTreeAddresses(TTree* tree) {

      tree -> Branch("EvtTrueNumJets",     &m_matchNumJets.first,  "EvtTrueNumJets/I");
      tree -> Branch("EvtRecoNumJets",     &m_matchNumJets.second, "EvtRecoNumJets/I");
      tree -> Branch("EvtTrueNumChrgPars", &m_matchNumTrks.first,  "EvtTrueNumTrks/I");
      tree -> Branch("EvtRecoNumChrgPars", &m_matchNumTrks.second, "EvtRecoNumTrks/I");
      tree -> Branch("EvtTrueVtxX",        &m_matchVtxX.first,     "EvtTrueVtxX/D");
      tree -> Branch("EvtRecoVtxX",        &m_matchVtxX.second,    "EvtRecoVtxX/D");
      tree -> Branch("EvtTrueVtxY",        &m_matchVtxY.first,     "EvtTrueVtxY/D");
      tree -> Branch("EvtRecoVtxY",        &m_matchVtxY.second,    "EvtRecoVtxY/D");
      tree -> Branch("EvtTrueVtxZ",        &m_matchVtxZ.first,     "EvtTrueVtxZ/D");
      tree -> Branch("EvtRecoVtxZ",        &m_matchVtxZ.second,    "EvtRecoVtxZ/D");
      tree -> Branch("JetTrueJetID",       &m_matchJetID.first);
      tree -> Branch("JetRecoJetID",       &m_matchJetID.second);
      tree -> Branch("JetTrueNumCst",      &m_matchJetNumCst.first);
      tree -> Branch("JetRecoNumCst",      &m_matchJetNumCst.second);
      tree -> Branch("JetTrueEne",         &m_matchJetEne.first);
      tree -> Branch("JetRecoEne",         &m_matchJetEne.second);
      tree -> Branch("JetTruePt",          &m_matchJetPt.first);
      tree -> Branch("JetRecoPt",          &m_matchJetPt.second);
      tree -> Branch("JetTrueEta",         &m_matchJetEta.first);
      tree -> Branch("JetRecoEta",         &m_matchJetEta.second);
      tree -> Branch("JetTruePhi",         &m_matchJetPhi.first);
      tree -> Branch("JetRecoPhi",         &m_matchJetPhi.second);
      tree -> Branch("JetTrueArea",        &m_matchJetArea.first);
      tree -> Branch("JetRecoArea",        &m_matchJetArea.second);
      tree -> Branch("CstTrueCstID",       &m_matchCstID.first);
      tree -> Branch("CstRecoCstID",       &m_matchCstID.second);
      tree -> Branch("CstTrueZ",           &m_matchCstZ.first);
      tree -> Branch("CstRecoZ",           &m_matchCstZ.second);
      tree -> Branch("CstTrueDr",          &m_matchCstDr.first);
      tree -> Branch("CstRecoDr",          &m_matchCstDr.second);
      tree -> Branch("CstRecoEne",         &m_matchCstEne.second);
      tree -> Branch("CstTrueEne",         &m_matchCstEne.first);
      tree -> Branch("CstTrueJt",          &m_matchCstJt.first);
      tree -> Branch("CstRecoJt",          &m_matchCstJt.second);
      tree -> Branch("CstTrueEta",         &m_matchCstEta.first);
      tree -> Branch("CstRecoEta",         &m_matchCstEta.second);
      tree -> Branch("CstTruePhi",         &m_matchCstPhi.first);
      tree -> Branch("CstRecoPhi",         &m_matchCstPhi.second);
      return;

    }  // end 'SetTreeAddresses(TTree* tree)'



    // ------------------------------------------------------------------------
    //! Translate normal output into legacy output
    // ------------------------------------------------------------------------
    void GetOutput(const SCorrelatorResponseMakerOutput& output) {

      /* TODO fill in */
      return;

    }  // end 'GetOutput(SCorrelatorResponseMakerOutput&)'

  };  // end SCorrelatorResponseMakerLegacyOutput

}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------

