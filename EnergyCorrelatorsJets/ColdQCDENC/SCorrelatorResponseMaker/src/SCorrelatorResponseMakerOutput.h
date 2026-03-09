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
    Types::GEvtInfo evtGen;
    Types::REvtInfo evtRec;

    // jet level info
    vector<Types::JetInfo> jetGen;
    vector<Types::JetInfo> jetRec;

    // constituent level info
    vector<vector<Types::CstInfo>> cstGen;
    vector<vector<Types::CstInfo>> cstRec;

    // ------------------------------------------------------------------------
    //! Reset variables
    // ------------------------------------------------------------------------
    void Reset() {
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
    pair<vector<vector<double>>, vector<vector<double>>> cstPt;
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
      cstPt.first.clear();
      cstPt.second.clear();
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

      tree -> Branch("EvtTrueNumJets",     &numJets.first,  "EvtTrueNumJets/I");
      tree -> Branch("EvtRecoNumJets",     &numJets.second, "EvtRecoNumJets/I");
      tree -> Branch("EvtTrueNumChrgPars", &numTrks.first,  "EvtTrueNumTrks/I");
      tree -> Branch("EvtRecoNumChrgPars", &numTrks.second, "EvtRecoNumTrks/I");
      tree -> Branch("EvtTrueVtxX",        &vtxX.first,     "EvtTrueVtxX/D");
      tree -> Branch("EvtRecoVtxX",        &vtxX.second,    "EvtRecoVtxX/D");
      tree -> Branch("EvtTrueVtxY",        &vtxY.first,     "EvtTrueVtxY/D");
      tree -> Branch("EvtRecoVtxY",        &vtxY.second,    "EvtRecoVtxY/D");
      tree -> Branch("EvtTrueVtxZ",        &vtxZ.first,     "EvtTrueVtxZ/D");
      tree -> Branch("EvtRecoVtxZ",        &vtxZ.second,    "EvtRecoVtxZ/D");
      tree -> Branch("JetTrueJetID",       &jetID.first);
      tree -> Branch("JetRecoJetID",       &jetID.second);
      tree -> Branch("JetTrueNumCst",      &jetNumCst.first);
      tree -> Branch("JetRecoNumCst",      &jetNumCst.second);
      tree -> Branch("JetTrueEne",         &jetEne.first);
      tree -> Branch("JetRecoEne",         &jetEne.second);
      tree -> Branch("JetTruePt",          &jetPt.first);
      tree -> Branch("JetRecoPt",          &jetPt.second);
      tree -> Branch("JetTrueEta",         &jetEta.first);
      tree -> Branch("JetRecoEta",         &jetEta.second);
      tree -> Branch("JetTruePhi",         &jetPhi.first);
      tree -> Branch("JetRecoPhi",         &jetPhi.second);
      tree -> Branch("JetTrueArea",        &jetArea.first);
      tree -> Branch("JetRecoArea",        &jetArea.second);
      tree -> Branch("CstTrueCstID",       &cstID.first);
      tree -> Branch("CstRecoCstID",       &cstID.second);
      tree -> Branch("CstTrueZ",           &cstZ.first);
      tree -> Branch("CstRecoZ",           &cstZ.second);
      tree -> Branch("CstTrueDr",          &cstDr.first);
      tree -> Branch("CstRecoDr",          &cstDr.second);
      tree -> Branch("CstRecoEne",         &cstEne.second);
      tree -> Branch("CstTrueEne",         &cstEne.first);
      tree -> Branch("CstTruePt",          &cstPt.first);
      tree -> Branch("CstRecoPt",          &cstPt.second);
      tree -> Branch("CstTrueEta",         &cstEta.first);
      tree -> Branch("CstRecoEta",         &cstEta.second);
      tree -> Branch("CstTruePhi",         &cstPhi.first);
      tree -> Branch("CstRecoPhi",         &cstPhi.second);
      return;

    }  // end 'SetTreeAddresses(TTree* tree)'



    // ------------------------------------------------------------------------
    //! Translate normal output into legacy output
    // ------------------------------------------------------------------------
    void GetOutput(const SCorrelatorResponseMakerOutput& output) {

      // grab no.s of jets
      const int64_t nTrueJets = output.jetGen.size();
      const int64_t nRecoJets = output.jetRec.size();

      // set event info
      numJets = make_pair( nTrueJets,                          nRecoJets );
      numTrks = make_pair( output.evtGen.GetNChrgPar(),        output.evtRec.GetNTrks() );
      vtxX    = make_pair( output.evtGen.GetPartonA().GetVX(), output.evtRec.GetVX() );
      vtxY    = make_pair( output.evtGen.GetPartonA().GetVY(), output.evtRec.GetVY() );
      vtxZ    = make_pair( output.evtGen.GetPartonA().GetVZ(), output.evtRec.GetVZ() );

      // set true jet info
      for (const Types::JetInfo& jet : output.jetGen) {
        jetID.first.push_back( jet.GetJetID() );
        jetNumCst.first.push_back( jet.GetNCsts() );
        jetEne.first.push_back( jet.GetEne() );
        jetPt.first.push_back( jet.GetPT() );
        jetEta.first.push_back( jet.GetEta() );
        jetPhi.first.push_back( jet.GetPhi() );
        jetArea.first.push_back( jet.GetArea() );
      }

      // set reco jet info
      for (const Types::JetInfo& jet : output.jetRec) {
        jetID.second.push_back( jet.GetJetID() );
        jetNumCst.second.push_back( jet.GetNCsts() );
        jetEne.second.push_back( jet.GetEne() );
        jetPt.second.push_back( jet.GetPT() );
        jetEta.second.push_back( jet.GetEta() );
        jetPhi.second.push_back( jet.GetPhi() );
        jetArea.second.push_back( jet.GetArea() );
      }

      // set true cst info
      cstID.first.resize( nTrueJets );
      cstZ.first.resize( nTrueJets );
      cstDr.first.resize( nTrueJets );
      cstEne.first.resize( nTrueJets );
      cstPt.first.resize( nTrueJets );
      cstEta.first.resize( nTrueJets );
      cstPhi.first.resize( nTrueJets );
      for (size_t iTrueJet = 0; iTrueJet < output.cstGen.size(); ++iTrueJet) {
        for (const Types::CstInfo& cst : output.cstGen[iTrueJet]) {
          cstID.first.at(iTrueJet).push_back( cst.GetCstID() );
          cstZ.first.at(iTrueJet).push_back( cst.GetZ() );
          cstDr.first.at(iTrueJet).push_back( cst.GetDR() );
          cstEne.first.at(iTrueJet).push_back( cst.GetEne() );
          cstPt.first.at(iTrueJet).push_back( cst.GetPT() );
          cstEta.first.at(iTrueJet).push_back( cst.GetEta() );
          cstPhi.first.at(iTrueJet).push_back( cst.GetPhi() );
        }
      }

      // set reco cst info
      cstID.second.resize( nRecoJets );
      cstZ.second.resize( nRecoJets );
      cstDr.second.resize( nRecoJets );
      cstEne.second.resize( nRecoJets );
      cstPt.second.resize( nRecoJets );
      cstEta.second.resize( nRecoJets );
      cstPhi.second.resize( nRecoJets );
      for (size_t iRecoJet = 0; iRecoJet < output.cstRec.size(); ++iRecoJet) {
        for (const Types::CstInfo& cst : output.cstRec[iRecoJet]) {
          cstID.second.at(iRecoJet).push_back( cst.GetCstID() );
          cstZ.second.at(iRecoJet).push_back( cst.GetZ() );
          cstDr.second.at(iRecoJet).push_back( cst.GetDR() );
          cstEne.second.at(iRecoJet).push_back( cst.GetEne() );
          cstPt.second.at(iRecoJet).push_back( cst.GetPT() );
          cstEta.second.at(iRecoJet).push_back( cst.GetEta() );
          cstPhi.second.at(iRecoJet).push_back( cst.GetPhi() );
        }
      }
      return;

    }  // end 'GetOutput(SCorrelatorResponseMakerOutput&)'

  };  // end SCorrelatorResponseMakerLegacyOutput

}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------

