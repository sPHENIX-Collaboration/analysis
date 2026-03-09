/// ---------------------------------------------------------------------------
/*! \file    SEnergyCorrelatorInput.h
 *  \authors Derek Anderson, Alex Clarke
 *  \date    03.15.2023
 *
 *  A set of structs to define module input
 *  and how to interface with the input trees.
 */
/// ---------------------------------------------------------------------------

#ifndef SENERGYCORRELATORINPUT_H
#define SENERGYCORRELATORINPUT_H

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // --------------------------------------------------------------------------
  //! Module input
  // --------------------------------------------------------------------------
  struct SEnergyCorrelatorInput {

    // event level info
    Types::REvtInfo reco;
    Types::GEvtInfo gen;

    // jet and constituent info
    vector<Types::JetInfo>         jets;
    vector<vector<Types::CstInfo>> csts; 

    // ------------------------------------------------------------------------
    //! Reset variables
    // ------------------------------------------------------------------------
    void Reset() {

      reco.Reset();
      gen.Reset();
      jets.clear();
      csts.clear();
      return;

    }  // end 'Reset()'

  };  // end SEnergyCorrelatorInput 



  // --------------------------------------------------------------------------
  //! Interface to input tree
  // --------------------------------------------------------------------------
  struct SCorrelatorInputInterface {

    // input truth tree addresses
    int                  evtNumChrgPars = numeric_limits<int>::min();
    double               evtSumPar      = numeric_limits<double>::min();
    pair<int, int>       partonID       = {numeric_limits<int>::min(),    numeric_limits<int>::min()};
    pair<double, double> partonMomX     = {numeric_limits<double>::min(), numeric_limits<double>::min()};
    pair<double, double> partonMomY     = {numeric_limits<double>::min(), numeric_limits<double>::min()};
    pair<double, double> partonMomZ     = {numeric_limits<double>::min(), numeric_limits<double>::min()};
    vector<vector<int>>* cstID          = NULL;
    vector<vector<int>>* cstEmbedID     = NULL;

    // input reco. tree addresses
    int                  evtNumTrks = numeric_limits<int>::min();
    double               evtSumECal = numeric_limits<double>::min();
    double               evtSumHCal = numeric_limits<double>::min();
    vector<vector<int>>* cstMatchID = NULL;

    // generic input tree address members
    int                     evtNumJets = numeric_limits<int>::min();
    double                  evtVtxX    = numeric_limits<double>::min();
    double                  evtVtxY    = numeric_limits<double>::min();
    double                  evtVtxZ    = numeric_limits<double>::min();
    vector<unsigned long>*  jetNumCst  = NULL;
    vector<unsigned int>*   jetID      = NULL;
    vector<unsigned int>*   jetTruthID = NULL;
    vector<double>*         jetEnergy  = NULL;
    vector<double>*         jetPt      = NULL;
    vector<double>*         jetEta     = NULL;
    vector<double>*         jetPhi     = NULL;
    vector<double>*         jetArea    = NULL;
    vector<vector<double>>* cstZ       = NULL;
    vector<vector<double>>* cstDr      = NULL;
    vector<vector<double>>* cstEnergy  = NULL;
    vector<vector<double>>* cstPt      = NULL;
    vector<vector<double>>* cstEta     = NULL;
    vector<vector<double>>* cstPhi     = NULL;

    // ------------------------------------------------------------------------
    //! Reset variables
    // ------------------------------------------------------------------------
    void Reset() {

      evtNumChrgPars = numeric_limits<int>::min();
      evtSumPar      = numeric_limits<double>::min();
      partonID       = make_pair(numeric_limits<int>::min(),    numeric_limits<int>::min());
      partonMomX     = make_pair(numeric_limits<double>::min(), numeric_limits<double>::min());
      partonMomY     = make_pair(numeric_limits<double>::min(), numeric_limits<double>::min());
      partonMomZ     = make_pair(numeric_limits<double>::min(), numeric_limits<double>::min());
      cstID          = NULL;
      cstEmbedID     = NULL;
      evtNumTrks     = numeric_limits<int>::min();
      evtSumECal     = numeric_limits<double>::min();
      evtSumHCal     = numeric_limits<double>::min();
      cstMatchID     = NULL;
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
      cstZ           = NULL;
      cstDr          = NULL;
      cstEnergy      = NULL;
      cstPt          = NULL;
      cstEta         = NULL;
      cstPhi         = NULL;
      return;

    }  // end 'Reset()'

    // ------------------------------------------------------------------------
    //! Set tree addresses
    // ------------------------------------------------------------------------
    void SetChainAddresses(TChain* chain, const bool isInTreeTruth = true) {

      if (isInTreeTruth) {
        chain -> SetBranchAddress("Parton3_ID",   &partonID.first);
        chain -> SetBranchAddress("Parton4_ID",   &partonID.second);
        chain -> SetBranchAddress("Parton3_MomX", &partonMomX.first);
        chain -> SetBranchAddress("Parton3_MomY", &partonMomY.first);
        chain -> SetBranchAddress("Parton3_MomZ", &partonMomZ.first);
        chain -> SetBranchAddress("Parton4_MomX", &partonMomX.second);
        chain -> SetBranchAddress("Parton4_MomY", &partonMomY.second);
        chain -> SetBranchAddress("Parton4_MomZ", &partonMomZ.second);
        chain -> SetBranchAddress("EvtSumParEne", &evtSumPar);
        chain -> SetBranchAddress("CstID",        &cstID);
        chain -> SetBranchAddress("CstEmbedID",   &cstEmbedID);
      } else {
        chain -> SetBranchAddress("EvtNumTrks",    &evtNumTrks);
        chain -> SetBranchAddress("EvtSumECalEne", &evtSumECal);
        chain -> SetBranchAddress("EvtSumHCalEne", &evtSumHCal);
        chain -> SetBranchAddress("CstMatchID",    &cstMatchID);
      }
      chain -> SetBranchAddress("EvtVtxX",    &evtVtxX);
      chain -> SetBranchAddress("EvtVtxY",    &evtVtxY);
      chain -> SetBranchAddress("EvtVtxZ",    &evtVtxZ);
      chain -> SetBranchAddress("EvtNumJets", &evtNumJets);
      chain -> SetBranchAddress("JetNumCst",  &jetNumCst);
      chain -> SetBranchAddress("JetID",      &jetID);
      chain -> SetBranchAddress("JetEnergy",  &jetEnergy);
      chain -> SetBranchAddress("JetPt",      &jetPt);
      chain -> SetBranchAddress("JetEta",     &jetEta);
      chain -> SetBranchAddress("JetPhi",     &jetPhi);
      chain -> SetBranchAddress("JetArea",    &jetArea);
      chain -> SetBranchAddress("CstZ",       &cstZ);
      chain -> SetBranchAddress("CstDr",      &cstDr);
      chain -> SetBranchAddress("CstEnergy",  &cstEnergy);
      chain -> SetBranchAddress("CstPt",      &cstPt);
      chain -> SetBranchAddress("CstEta",     &cstEta);
      chain -> SetBranchAddress("CstPhi",     &cstPhi);
      return;

    }  // end 'SetChainAddresses(TChain*)'

    // ------------------------------------------------------------------------
    //! Collect tree values into input variables
    // ------------------------------------------------------------------------
    void SetCorrelatorInput(
      SEnergyCorrelatorInput& input,
      const bool isInTreeTruth = true,
      optional<bool> isEmbed = nullopt
    ) {

      // make sure container is empty
      input.Reset();

      // set reco event-level variables
      input.reco.SetNTrks( evtNumTrks );
      input.reco.SetESumEMCal( evtSumECal );
      input.reco.SetESumIHCal( evtSumHCal );
      input.reco.SetESumOHCal( evtSumHCal );
      input.reco.SetVX( evtVtxX );
      input.reco.SetVY( evtVtxY );
      input.reco.SetVZ( evtVtxZ );
      input.reco.SetVR( hypot(evtVtxX, evtVtxY) );

      // set generator event-level variables
      if (isInTreeTruth) {

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
          input.csts.at(iJet).at(iCst).SetJetID( jetID -> at(iJet) );
          input.csts.at(iJet).at(iCst).SetZ( (cstZ -> at(iJet)).at(iCst) );
          input.csts.at(iJet).at(iCst).SetDR( (cstDr -> at(iJet)).at(iCst) );
          input.csts.at(iJet).at(iCst).SetPT( (cstPt -> at(iJet)).at(iCst) );
          input.csts.at(iJet).at(iCst).SetEta( (cstEta -> at(iJet)).at(iCst) );
          input.csts.at(iJet).at(iCst).SetPhi( (cstPhi -> at(iJet)).at(iCst) );
          if (isInTreeTruth) {
            input.csts.at(iJet).at(iCst).SetEmbedID( (cstEmbedID -> at(iJet)).at(iCst) );
            input.csts.at(iJet).at(iCst).SetCstID( (cstID -> at(iJet)).at(iCst) );
          } else {
            input.csts.at(iJet).at(iCst).SetCstID( (cstMatchID -> at(iJet)).at(iCst) );
          }

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

    }  // end 'SetCorrelatorInput(SEnergyCorrelatorInput&, bool, optional<bool>)'

  };  // end SCorrelatorInputInterface

}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------
