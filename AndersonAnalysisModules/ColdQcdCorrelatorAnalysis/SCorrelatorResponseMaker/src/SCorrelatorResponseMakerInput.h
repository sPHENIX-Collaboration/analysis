/// ---------------------------------------------------------------------------
/*! \file   SCorrelatorResponseMakerInput.h
 *  \author Derek Anderson
 *  \date   05.20.2024
 *
 *  Inputs for the `SCorrelatorResponseMaker' module.
 */
/// ---------------------------------------------------------------------------

#ifndef SCORRELATORJETTREEMAKERINPUT_H
#define SCORRELATORJETTREEMAKERINPUT_H

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // --------------------------------------------------------------------------
  //! Reco jet tree input
  // --------------------------------------------------------------------------
  struct SCorrelatorResponseMakerRecoInput {

    // event level info
    Types::RecoInfo evt;

    // jet and constituent info
    vector<Types::JetInfo>         jets;
    vector<vector<Types::CstInfo>> csts; 

    // ------------------------------------------------------------------------
    //! Reset addresses
    // ------------------------------------------------------------------------
    void Reset() {
      evt.Reset();
      jets.clear();
      csts.clear();
      return;
    }  // end 'Reset()'

    // -----------------------------------------------------------------------
    //! Set addresses on a TTree
    // ------------------------------------------------------------------------
    void SetTreeAddresses(TTree* tree) {
      /* TODO fill in */
      return;
    }  // end 'SetTreeAddresses(TTree*)'

  };  // end SCorrelatorResponseMakerRecoInput



  // --------------------------------------------------------------------------
  //! Truth jet tree input
  // --------------------------------------------------------------------------
  struct SCorrelatorResponseMakerTruthInput {

    // event level info
    Types::GenInfo evt;

    // jet and constituent info
    vector<Types::JetInfo>         jets;
    vector<vector<Types::CstInfo>> csts; 

    // ------------------------------------------------------------------------
    //! Reset addresses
    // ------------------------------------------------------------------------
    void Reset() {
      evt.Reset();
      jets.clear();
      csts.clear();
      return;
    }  // end 'Reset()'

    // -----------------------------------------------------------------------
    //! Set addresses on a TTree
    // ------------------------------------------------------------------------
    void SetTreeAddresses(TTree* tree) {
      /* TODO fill in */
      return;
    }  // end 'SetTreeAddresses(TTree*)'

  };  // end SCorrelatorResponseMakerTruthInput



  // --------------------------------------------------------------------------
  //! Legacy reco jet tree input
  // --------------------------------------------------------------------------
  struct SCorrelatorResponseMakerLegacyRecoInput {

    // event level info
    int    nJets     = numeric_limits<int>::max();
    int    nChrgPars = numeric_limits<int>::max();
    double eSumPar   = numeric_limits<double>::max();
    double vtxX      = numeric_limits<double>::max();
    double vtxY      = numeric_limits<double>::max();
    double vtxZ      = numeric_limits<double>::max();

    // jet and constituent info
    vector<unsigned long>*  jetNCst    = NULL;
    vector<unsigned int>*   jetID      = NULL;
    vector<double>*         jetE       = NULL;
    vector<double>*         jetPt      = NULL;
    vector<double>*         jetEta     = NULL;
    vector<double>*         jetPhi     = NULL;
    vector<double>*         jetArea    = NULL;
    vector<vector<int>>*    cstType    = NULL;
    vector<vector<int>>*    cstMatchID = NULL;
    vector<vector<double>>* cstZ       = NULL;
    vector<vector<double>>* cstDr      = NULL;
    vector<vector<double>>* cstE       = NULL;
    vector<vector<double>>* cstPt      = NULL;
    vector<vector<double>>* cstEta     = NULL;
    vector<vector<double>>* cstPhi     = NULL;



    // ------------------------------------------------------------------------
    //! Reset addresses
    // ------------------------------------------------------------------------
    void Reset() {

      nJets      = numeric_limits<int>::max();
      nChrgPars  = numeric_limits<int>::max();
      eSumPar    = numeric_limits<double>::max();
      vtxX       = numeric_limits<double>::max();
      vtxY       = numeric_limits<double>::max();
      vtxZ       = numeric_limits<double>::max();
      jetNCst    = NULL;
      jetID      = NULL;
      jetE       = NULL;
      jetPt      = NULL;
      jetEta     = NULL;
      jetPhi     = NULL;
      jetArea    = NULL;
      cstType    = NULL;
      cstEmbedID = NULL;
      cstZ       = NULL;
      cstDr      = NULL;
      cstE       = NULL;
      cstPt      = NULL;
      cstEta     = NULL;
      cstPhi     = NULL;
      return;

    }  // end 'Reset()'



    // ------------------------------------------------------------------------
    //! Set addresses on a TTree
    // ------------------------------------------------------------------------
    void SetTreeAddresses(TTree* tree) {

      tree -> SetBranchAddress("EvtNumTrks",    &evtNumTrks);
      tree -> SetBranchAddress("EvtSumECalEne", &evtSumECal);
      tree -> SetBranchAddress("EvtSumHCalEne", &evtSumHCal);
      tree -> SetBranchAddress("EvtVtxX",       &evtVtxX);
      tree -> SetBranchAddress("EvtVtxY",       &evtVtxY);
      tree -> SetBranchAddress("EvtVtxZ",       &evtVtxZ);
      tree -> SetBranchAddress("EvtNumJets",    &evtNumJets);
      tree -> SetBranchAddress("JetNumCst",     &jetNumCst);
      tree -> SetBranchAddress("JetID",         &jetID);
      tree -> SetBranchAddress("JetEnergy",     &jetEnergy);
      tree -> SetBranchAddress("JetPt",         &jetPt);
      tree -> SetBranchAddress("JetEta",        &jetEta);
      tree -> SetBranchAddress("JetPhi",        &jetPhi);
      tree -> SetBranchAddress("JetArea",       &jetArea);
      tree -> SetBranchAddress("CstType",       &cstType);
      tree -> SetBranchAddress("CstMatchID",    &cstMatchID);
      tree -> SetBranchAddress("CstZ",          &cstZ);
      tree -> SetBranchAddress("CstDr",         &cstDr);
      tree -> SetBranchAddress("CstEnergy",     &cstEnergy);
      tree -> SetBranchAddress("CstJt",         &cstPt);
      tree -> SetBranchAddress("CstEta",        &cstEta);
      tree -> SetBranchAddress("CstPhi",        &cstPhi);
      return;

    }  // end 'SetTreeAddresses(TTree*)'



    // ------------------------------------------------------------------------
    //! Translate legacy reco input into normal reco input
    // ------------------------------------------------------------------------
    void SetInput(SCorrelatorResponseMakerRecoInput& input) {

      // make sure container is empty
      input.Reset();

      // set event-level variables
      input.evt.SetNTrks( evtNumTrks );
      input.evt.SetESumEMCal( evtSumECal );
      input.evt.SetESumIHCal( evtSumHCal );
      input.evt.SetESumOHCal( evtSumHCal );
      input.evt.SetVX( evtVtxX );
      input.evt.SetVY( evtVtxY );
      input.evt.SetVZ( evtVtxZ );
      input.evt.SetVR( hypot(evtVtxX, evtVtxY) );

      // set jet variables
      input.jets.resize( jetNumCst -> size() );
      for (size_t iJet = 0; iJet < jetNumCst -> size(); iJet++) {
        input.jets.at(iJet).SetJetID( jetID -> at(iJet) );
        input.jets.at(iJet).SetNCsts( jetNumCst -> at(iJet) );
        input.jets.at(iJet).SetEne( jetEnergy -> at(iJet) );
        input.jets.at(iJet).SetPT( jetPt -> at(iJet) );
        input.jets.at(iJet).SetEta( jetEta -> at(iJet) );
        input.jets.at(iJet).SetPhi( jetPhi -> at(iJet) );
        input.jets.at(iJet).SetArea( jetArea -> at(iJet) );
      }  // end jet loop

      // set cst variables
      input.csts.resize( jetNumCst -> size() );
      for (size_t iJet = 0; iJet < jetNumCst -> size(); iJet++) {

        // loop over constituents
        input.csts.at(iJet).resize( (cstZ -> at(iJet)).size() );
        for (size_t iCst = 0; iCst < (cstZ -> at(iJet)).size(); iCst++) {
          input.csts.at(iJet).at(iCst).SetType( (cstType -> at(iJet)).at(iCst) );
          input.csts.at(iJet).at(iCst).SetJetID( jetID -> at(iJet) );
          input.csts.at(iJet).at(iCst).SetCstID( (cstMatchID -> at(iJet)).at(iCst) );
          input.csts.at(iJet).at(iCst).SetZ( (cstZ -> at(iJet)).at(iCst) );
          input.csts.at(iJet).at(iCst).SetDR( (cstDr -> at(iJet)).at(iCst) );
          input.csts.at(iJet).at(iCst).SetPT( (cstPt -> at(iJet)).at(iCst) );
          input.csts.at(iJet).at(iCst).SetEta( (cstEta -> at(iJet)).at(iCst) );
          input.csts.at(iJet).at(iCst).SetPhi( (cstPhi -> at(iJet)).at(iCst) );

          // set cst. energy
          ROOT::Math::PtEtaPhiMVector pCst(
            input.csts.at(iJet).at(iCst).GetPT(),
            input.csts.at(iJet).at(iCst).GetEta(),
            input.csts.at(iJet).at(iCst).GetPhi(),
            Const::MassPion()
          );
          input.csts.at(iJet).at(iCst).SetEne( pCst.E() );
        }  // end cst loop
      }  // end jet loop
      return;

    }  // end 'SetInput(SCorrelatorResponseMakerRecoInput&)'

  };  // end SCorrelatorResponseMakerLegacyRecoInput



  // --------------------------------------------------------------------------
  //! Legacy truth jet tree input
  // --------------------------------------------------------------------------
  struct SCorrelatorResponseMakerLegacyTruthInput {

    // event level info
    int                  evtNumJets     = numeric_limits<int>::min();
    int                  evtNumChrgPars = numeric_limits<int>::min();
    double               evtVtxX        = numeric_limits<double>::min();
    double               evtVtxY        = numeric_limits<double>::min();
    double               evtVtxZ        = numeric_limits<double>::min();
    double               evtSumPar      = numeric_limits<double>::min();
    pair<int, int>       partonID       = {numeric_limits<int>::min(),    numeric_limits<int>::min()};
    pair<double, double> partonMomX     = {numeric_limits<double>::min(), numeric_limits<double>::min()};
    pair<double, double> partonMomY     = {numeric_limits<double>::min(), numeric_limits<double>::min()};
    pair<double, double> partonMomZ     = {numeric_limits<double>::min(), numeric_limits<double>::min()};

    // jet and constituent info
    vector<unsigned long>*  jetNumCst  = NULL;
    vector<unsigned int>*   jetID      = NULL;
    vector<unsigned int>*   jetTruthID = NULL;
    vector<double>*         jetEnergy  = NULL;
    vector<double>*         jetPt      = NULL;
    vector<double>*         jetEta     = NULL;
    vector<double>*         jetPhi     = NULL;
    vector<double>*         jetArea    = NULL;
    vector<vector<int>>*    cstID      = NULL;
    vector<vector<int>>*    cstEmbedID = NULL;
    vector<vector<double>>* cstZ       = NULL;
    vector<vector<double>>* cstDr      = NULL;
    vector<vector<double>>* cstEnergy  = NULL;
    vector<vector<double>>* cstPt      = NULL;
    vector<vector<double>>* cstEta     = NULL;
    vector<vector<double>>* cstPhi     = NULL;



    // ------------------------------------------------------------------------
    //! Reset addresses
    // ------------------------------------------------------------------------
    void Reset() {

      partonID       = make_pair(numeric_limits<int>::min(),    numeric_limits<int>::min());
      partonMomX     = make_pair(numeric_limits<double>::min(), numeric_limits<double>::min());
      partonMomY     = make_pair(numeric_limits<double>::min(), numeric_limits<double>::min());
      partonMomZ     = make_pair(numeric_limits<double>::min(), numeric_limits<double>::min());
      evtNumChrgPars = numeric_limits<int>::min();
      evtSumPar      = numeric_limits<double>::min();
      evtNumJets     = numeric_limits<int>::min();
      evtVtxX        = numeric_limits<double>::min();
      evtVtxY        = numeric_limits<double>::min();
      evtVtxZ        = numeric_limits<double>::min();
      jetNumCst      = NULL;
      jetID          = NULL;
      jetTruthID     = NULL;
      jetEnergy      = NULL;
      jetPt          = NULL;
      jetEta         = NULL;
      jetPhi         = NULL;
      jetArea        = NULL;
      cstID          = NULL;
      cstEmbedID     = NULL;
      cstZ           = NULL;
      cstDr          = NULL;
      cstEnergy      = NULL;
      cstPt          = NULL;
      cstEta         = NULL;
      cstPhi         = NULL;
      return;

    }  // end 'Reset()'



    // ------------------------------------------------------------------------
    //! Set addresses on a TTree
    // ------------------------------------------------------------------------
    void SetTreeAddresses(TTree* tree) {

      tree -> SetBranchAddress("Parton3_ID",   &partonID.first);
      tree -> SetBranchAddress("Parton4_ID",   &partonID.second);
      tree -> SetBranchAddress("Parton3_MomX", &partonMomX.first);
      tree -> SetBranchAddress("Parton3_MomY", &partonMomY.first);
      tree -> SetBranchAddress("Parton3_MomZ", &partonMomZ.first);
      tree -> SetBranchAddress("Parton4_MomX", &partonMomX.second);
      tree -> SetBranchAddress("Parton4_MomY", &partonMomY.second);
      tree -> SetBranchAddress("Parton4_MomZ", &partonMomZ.second);
      tree -> SetBranchAddress("EvtSumParEne", &evtSumPar);
      tree -> SetBranchAddress("EvtVtxX",      &evtVtxX);
      tree -> SetBranchAddress("EvtVtxY",      &evtVtxY);
      tree -> SetBranchAddress("EvtVtxZ",      &evtVtxZ);
      tree -> SetBranchAddress("EvtNumJets",   &evtNumJets);
      tree -> SetBranchAddress("JetNumCst",    &jetNumCst);
      tree -> SetBranchAddress("JetID",        &jetID);
      tree -> SetBranchAddress("JetEnergy",    &jetEnergy);
      tree -> SetBranchAddress("JetPt",        &jetPt);
      tree -> SetBranchAddress("JetEta",       &jetEta);
      tree -> SetBranchAddress("JetPhi",       &jetPhi);
      tree -> SetBranchAddress("JetArea",      &jetArea);
      tree -> SetBranchAddress("CstID",        &cstID);
      tree -> SetBranchAddress("CstEmbedID",   &cstEmbedID);
      tree -> SetBranchAddress("CstZ",         &cstZ);
      tree -> SetBranchAddress("CstDr",        &cstDr);
      tree -> SetBranchAddress("CstEnergy",    &cstEnergy);
      tree -> SetBranchAddress("CstJt",        &cstPt);
      tree -> SetBranchAddress("CstEta",       &cstEta);
      tree -> SetBranchAddress("CstPhi",       &cstPhi);
      return;

    }  // end 'SetTreeAddresses(TTree*)'



    // ------------------------------------------------------------------------
    //! Translate legacy truth input into normal truth input
    // ------------------------------------------------------------------------
    void SetInput(SCorrelatorResponseMakerTruthInput& input, optional<bool> isEmbed = nullopt) {

      // make sure container is empty
      input.Reset();

      // set parton variables
      pair<Types::ParInfo, Types::ParInfo> partons;
      partons.first.SetBarcode( partonID.first );
      partons.first.SetPX( partonMomX.first );
      partons.first.SetPY( partonMomY.first );
      partons.first.SetPZ( partonMomZ.first );
      partons.first.SetPT( hypot(partonMomX.first, partonMomY.first) );
      partons.first.SetVX( evtVtxX );
      partons.first.SetVY( evtVtxY );
      partons.first.SetVZ( evtVtxZ );
      partons.first.SetVR( hypot(evtVtxX, evtVtxY) );
      partons.second.SetBarcode( partonID.second );
      partons.second.SetPX( partonMomX.second );
      partons.second.SetPY( partonMomY.second );
      partons.second.SetPZ( partonMomZ.second );
      partons.second.SetPT( hypot(partonMomX.second, partonMomY.second) );
      partons.second.SetVX( evtVtxX );
      partons.second.SetVY( evtVtxY );
      partons.second.SetVZ( evtVtxZ );
      partons.second.SetVR( hypot(evtVtxX, evtVtxY) );

      // set event variables
      input.gen.SetESumChrg( evtSumPar );
      input.gen.SetESumNeu( evtSumPar );
      input.gen.SetPartons(partons);
      if (isEmbed.has_value()) {
        input.gen.SetIsEmbed( isEmbed.value() );
      }

      // set jet variables
      input.jets.resize( jetNumCst -> size() );
      for (size_t iJet = 0; iJet < jetNumCst -> size(); iJet++) {
        input.jets.at(iJet).SetJetID( jetID -> at(iJet) );
        input.jets.at(iJet).SetNCsts( jetNumCst -> at(iJet) );
        input.jets.at(iJet).SetEne( jetEnergy -> at(iJet) );
        input.jets.at(iJet).SetPT( jetPt -> at(iJet) );
        input.jets.at(iJet).SetEta( jetEta -> at(iJet) );
        input.jets.at(iJet).SetPhi( jetPhi -> at(iJet) );
        input.jets.at(iJet).SetArea( jetArea -> at(iJet) );
      }  // end jet loop

      // set cst variables
      input.csts.resize( jetNumCst -> size() );
      for (size_t iJet = 0; iJet < jetNumCst -> size(); iJet++) {

        // loop over constituents
        input.csts.at(iJet).resize( (cstZ -> at(iJet)).size() );
        for (size_t iCst = 0; iCst < (cstZ -> at(iJet)).size(); iCst++) {
          input.csts.at(iJet).at(iCst).SetCstID( (cstID -> at(iJet)).at(iCst) );
          input.csts.at(iJet).at(iCst).SetJetID( jetID -> at(iJet) );
          input.csts.at(iJet).at(iCst).SetEmbedID( (cstEmbedID -> at(iJet)).at(iCst) );
          input.csts.at(iJet).at(iCst).SetZ( (cstZ -> at(iJet)).at(iCst) );
          input.csts.at(iJet).at(iCst).SetDR( (cstDr -> at(iJet)).at(iCst) );
          input.csts.at(iJet).at(iCst).SetPT( (cstPt -> at(iJet)).at(iCst) );
          input.csts.at(iJet).at(iCst).SetEta( (cstEta -> at(iJet)).at(iCst) );
          input.csts.at(iJet).at(iCst).SetPhi( (cstPhi -> at(iJet)).at(iCst) );

          // set cst. energy
          ROOT::Math::PtEtaPhiMVector pCst(
            input.csts.at(iJet).at(iCst).GetPT(),
            input.csts.at(iJet).at(iCst).GetEta(),
            input.csts.at(iJet).at(iCst).GetPhi(),
            Const::MassPion()
          );
          input.csts.at(iJet).at(iCst).SetEne( pCst.E() );
        }  // end cst loop
      }  // end jet loop
      return;

    }  // end 'SetInput(SCorrelatorResponseMakerTruthInput&, optional<bool>)'

  };  // end SCorrelatorResponseMakerLegacyTruthInput

}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------
