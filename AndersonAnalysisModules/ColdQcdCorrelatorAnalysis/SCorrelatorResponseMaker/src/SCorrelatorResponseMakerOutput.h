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



namespace SColdQcdCorrelatorAnalysis {

  // --------------------------------------------------------------------------
  //! Response tree output
  // --------------------------------------------------------------------------
  struct SCorrelatorResponseMakerOutput {

    /* TODO fill in */

  };  // end SCorrelatorResponseMakerOutput



  // --------------------------------------------------------------------------
  //! Legacy response tree output
  // --------------------------------------------------------------------------
  struct SCorrelatorResponseMakerLegacyOutput {

    // output response tree event-level address members
    //   - FIXME should be consolidated into a single class/struct for ease-of-maintenance
    //   - TODO add non-pair values (e.g. sum of particle energy, fraction of cst.s matched)
    pair<int,    int>    m_matchNumJets;
    pair<int,    int>    m_matchNumTrks;
    pair<double, double> m_matchVtxX;
    pair<double, double> m_matchVtxY;
    pair<double, double> m_matchVtxZ;
    // output response tree jet-level address members
    pair<vector<uint32_t>, vector<uint32_t>> m_matchJetID;
    pair<vector<uint64_t>, vector<uint64_t>> m_matchJetNumCst;
    pair<vector<double>,   vector<double>>   m_matchJetEne;
    pair<vector<double>,   vector<double>>   m_matchJetPt;
    pair<vector<double>,   vector<double>>   m_matchJetEta;
    pair<vector<double>,   vector<double>>   m_matchJetPhi;
    pair<vector<double>,   vector<double>>   m_matchJetArea;
    // output response tree cst-level address members
    pair<vector<vector<int>>,    vector<vector<int>>>    m_matchCstID;
    pair<vector<vector<double>>, vector<vector<double>>> m_matchCstZ;
    pair<vector<vector<double>>, vector<vector<double>>> m_matchCstDr;
    pair<vector<vector<double>>, vector<vector<double>>> m_matchCstEne;
    pair<vector<vector<double>>, vector<vector<double>>> m_matchCstJt;
    pair<vector<vector<double>>, vector<vector<double>>> m_matchCstEta;
    pair<vector<vector<double>>, vector<vector<double>>> m_matchCstPhi;


    // ------------------------------------------------------------------------
    //! Reset variables
    // ------------------------------------------------------------------------
    void Reset() {

      /* TODO fill in */
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

