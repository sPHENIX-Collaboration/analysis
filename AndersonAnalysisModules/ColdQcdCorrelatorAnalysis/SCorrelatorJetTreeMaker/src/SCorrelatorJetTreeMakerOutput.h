// ----------------------------------------------------------------------------
// 'SCorrelatorJetTreeMakerOutput.h'
// Derek Anderson
// 03.22.2024
//
// A module to produce a tree of jets for the sPHENIX
// Cold QCD Energy-Energy Correlator analysis.
//
// Initially derived from code by Antonio Silva (thanks!!)
// ----------------------------------------------------------------------------

#ifndef SCORRELATORJETTREEMAKEROUTPUT_H
#define SCORRELATORJETTREEMAKEROUTPUT_H

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // SCorrelatorJetTreeMakerRecoOutput definition -----------------------------

  struct SCorrelatorJetTreeMakerRecoOutput {

    // event-level variables
    Types::RecoInfo evt;

    // jet-level variables
    vector<Types::JetInfo> jets;

    // cst-level variables
    vector<vector<Types::CstInfo>> csts;

    void Reset() {
      evt.Reset();
      jets.clear();
      csts.clear();
      return;
    }  // end 'Reset()'

    void SetTreeAddresses(TTree* tree) {
      tree -> Branch("Evt",  "SColdQcdCorrelatorAnalysis::Types::RecoInfo", &evt, 32000, 0);
      tree -> Branch("Jets", "std::vector<SColdQcdCorrelatorAnalysis::Types::JetInfo>", &jets, 32000, 0);
      tree -> Branch("Csts", "std::vector<std::vector<SColdQcdCorrelatorAnalysis::Types::CstInfo>>", &csts, 32000, 0);
      return;
    }  // end 'SetTreeAddresses(TTree*)'

  };  // end SCorrelatorJetTreeMakerRecoOutput



  // SCorrelatorJetTreeMakerTruthOutput definition ----------------------------

  struct SCorrelatorJetTreeMakerTruthOutput {

    // event-level variables
    Types::GenInfo evt;

    // jet-level variables
    vector<Types::JetInfo> jets;

    // cst-level variables
    vector<vector<Types::CstInfo>> csts;

    void Reset() {
      evt.Reset();
      jets.clear();
      csts.clear();
      return;
    }  // end 'Reset()'

    void SetTreeAddresses(TTree* tree) {
      tree -> Branch("Evt",  "SColdQcdCorrelatorAnalysis::Types::GenInfo", &evt, 32000, 0);
      tree -> Branch("Jets", "std::vector<SColdQcdCorrelatorAnalysis::Types::JetInfo>", &jets, 32000, 0);
      tree -> Branch("Csts", "std::vector<std::vector<SColdQcdCorrelatorAnalysis::Types::CstInfo>>", &csts, 32000, 0);
      return;
    }  // end 'SetTreeAddresses(TTree*)'

  };  // end SCorrelatorJetTreeMakerTruthOutput



  // SCorrelatorJetTreeMakerLegacyRecoOutput definition ------------------------

  struct SCorrelatorJetTreeMakerLegacyRecoOutput {

    // output reco tree event variables
    int    nJets    = numeric_limits<int>::max();
    int    nTrks    = numeric_limits<int>::max();
    double vtxX     = numeric_limits<double>::max();
    double vtxY     = numeric_limits<double>::max();
    double vtxZ     = numeric_limits<double>::max();
    double eSumECal = numeric_limits<double>::max();
    double eSumHCal = numeric_limits<double>::max();

    // output reco tree jet variables
    vector<unsigned long> jetNCst;
    vector<unsigned int>  jetID;
    vector<double>        jetE;
    vector<double>        jetPt;
    vector<double>        jetEta;
    vector<double>        jetPhi;
    vector<double>        jetArea;

    // output reco tree constituent variables
    vector<vector<int>>    cstType;
    vector<vector<int>>    cstMatchID;
    vector<vector<double>> cstZ;
    vector<vector<double>> cstDr;
    vector<vector<double>> cstE;
    vector<vector<double>> cstPt;
    vector<vector<double>> cstEta;
    vector<vector<double>> cstPhi;



    void Reset() {
      nJets    = numeric_limits<int>::max();
      nTrks    = numeric_limits<int>::max();
      vtxX     = numeric_limits<double>::max();
      vtxY     = numeric_limits<double>::max();
      vtxZ     = numeric_limits<double>::max();
      eSumECal = numeric_limits<double>::max();
      eSumHCal = numeric_limits<double>::max();
      jetNCst.clear();
      jetID.clear();
      jetE.clear();
      jetPt.clear();
      jetEta.clear();
      jetPhi.clear();
      jetArea.clear();
      cstType.clear();
      cstMatchID.clear();
      cstZ.clear();
      cstDr.clear();
      cstE.clear();
      cstPt.clear();
      cstEta.clear();
      cstPhi.clear();
      return;
    }  // end 'Reset()'



    void SetTreeAddresses(TTree* tree) {
      tree -> Branch("EvtNumJets",    &nJets,    "EvtNumJets/I");
      tree -> Branch("EvtNumTrks",    &nTrks,    "EvtNumTrks/I");
      tree -> Branch("EvtVtxX",       &vtxX,     "EvtVtxX/D");
      tree -> Branch("EvtVtxY",       &vtxY,     "EvtVtxY/D");
      tree -> Branch("EvtVtxZ",       &vtxZ,     "EvtVtxZ/D");
      tree -> Branch("EvtSumECalEne", &eSumECal, "EvtSumECalEne/D");
      tree -> Branch("EvtSumHCalEne", &eSumHCal, "EvtSumHCalEne/D");
      tree -> Branch("JetNumCst",     &jetNCst);
      tree -> Branch("JetID",         &jetID);
      tree -> Branch("JetEnergy",     &jetE);
      tree -> Branch("JetPt",         &jetPt);
      tree -> Branch("JetEta",        &jetEta);
      tree -> Branch("JetPhi",        &jetPhi);
      tree -> Branch("JetArea",       &jetArea);
      tree -> Branch("CstType",       &cstType);
      tree -> Branch("CstMatchID",    &cstMatchID);
      tree -> Branch("CstZ",          &cstZ);
      tree -> Branch("CstDr",         &cstDr);
      tree -> Branch("CstEnergy",     &cstE);
      tree -> Branch("CstPt",         &cstPt);
      tree -> Branch("CstEta",        &cstEta);
      tree -> Branch("CstPhi",        &cstPhi);
      return;
    }  // end 'SetTreeAddresses(TTree*)'



    void GetTreeMakerOutput(SCorrelatorJetTreeMakerRecoOutput& output) {

      // get event variables
      nJets     = output.jets.size();
      nTrks     = output.evt.GetNTrks();
      eSumECal  = output.evt.GetESumEMCal();
      eSumHCal  = output.evt.GetESumIHCal() + output.evt.GetESumOHCal();
      vtxX      = output.evt.GetVX();
      vtxY      = output.evt.GetVY();
      vtxZ      = output.evt.GetVZ();

      // get jet variables
      for (Types::JetInfo& jet : output.jets) {
        jetNCst.push_back( jet.GetNCsts() );
        jetID.push_back( jet.GetJetID() );
        jetE.push_back( jet.GetEne() );
        jetPt.push_back( jet.GetPT() );
        jetEta.push_back( jet.GetEta() );
        jetPhi.push_back( jet.GetPhi() );
        jetArea.push_back( jet.GetArea() );
      }

      // get constituent variables
      cstType.resize( output.csts.size() );
      cstMatchID.resize( output.csts.size() );
      cstZ.resize( output.csts.size() );
      cstDr.resize( output.csts.size() );
      cstE.resize( output.csts.size() );
      cstPt.resize( output.csts.size() );
      cstEta.resize( output.csts.size() );
      cstPhi.resize( output.csts.size() );
      for (uint64_t iJet = 0; iJet < output.csts.size(); iJet++) {
        for (Types::CstInfo& cst : output.csts.at(iJet)) {
          cstType.at(iJet).push_back( cst.GetType() );
          cstMatchID.at(iJet).push_back( cst.GetCstID() );
          cstZ.at(iJet).push_back( cst.GetZ() );
          cstDr.at(iJet).push_back( cst.GetDR() );
          cstE.at(iJet).push_back( cst.GetEne() );
          cstPt.at(iJet).push_back( cst.GetPT() );
          cstEta.at(iJet).push_back( cst.GetEta() );
          cstPhi.at(iJet).push_back( cst.GetPhi() );
        }
      }
      return;

    }  // end 'GetTreeMakerOutput(SCorrelatorJetTreeMakerRecoOutput&)'

  };  // end SCorrelatorJetTreeMakerLegacyRecoOutput



  // SCorrelatorJetTreeMakerLegacyTruthOutput definition ----------------------

  struct SCorrelatorJetTreeMakerLegacyTruthOutput {

    // output truth tree event variables
    int    nJets     = numeric_limits<int>::max();
    int    nChrgPars = numeric_limits<int>::max();
    double eSumPar   = numeric_limits<double>::max();
    double vtxX      = numeric_limits<double>::max();
    double vtxY      = numeric_limits<double>::max();
    double vtxZ      = numeric_limits<double>::max();

    // output truth tree parton variables
    pair<int, int>        partonID = {numeric_limits<int>::max(),    numeric_limits<int>::max()};
    pair<double, double>  partonPX = {numeric_limits<double>::max(), numeric_limits<double>::max()};
    pair<double, double>  partonPY = {numeric_limits<double>::max(), numeric_limits<double>::max()};
    pair<double, double>  partonPZ = {numeric_limits<double>::max(), numeric_limits<double>::max()};

    // output truth tree jet variables
    vector<unsigned long> jetNCst;
    vector<unsigned int>  jetID;
    vector<double>        jetE;
    vector<double>        jetPt;
    vector<double>        jetEta;
    vector<double>        jetPhi;
    vector<double>        jetArea;

    // output truth tree constituent variables
    vector<vector<int>>    cstID;
    vector<vector<int>>    cstPID;
    vector<vector<int>>    cstType;
    vector<vector<int>>    cstEmbedID;
    vector<vector<double>> cstZ;
    vector<vector<double>> cstDr;
    vector<vector<double>> cstE;
    vector<vector<double>> cstPt;
    vector<vector<double>> cstEta;
    vector<vector<double>> cstPhi;



    void Reset() {
      nJets     = numeric_limits<int>::max();
      nChrgPars = numeric_limits<int>::max();
      eSumPar   = numeric_limits<double>::max();
      vtxX      = numeric_limits<double>::max();
      vtxY      = numeric_limits<double>::max();
      vtxZ      = numeric_limits<double>::max();
      partonID  = make_pair(numeric_limits<int>::max(), numeric_limits<int>::max());
      partonPX  = make_pair(numeric_limits<int>::max(), numeric_limits<int>::max());
      partonPY  = make_pair(numeric_limits<int>::max(), numeric_limits<int>::max());
      partonPZ  = make_pair(numeric_limits<int>::max(), numeric_limits<int>::max());
      jetNCst.clear();
      jetID.clear();
      jetE.clear();
      jetPt.clear();
      jetEta.clear();
      jetPhi.clear();
      jetArea.clear();
      cstID.clear();
      cstPID.clear();
      cstType.clear();
      cstEmbedID.clear();
      cstZ.clear();
      cstDr.clear();
      cstE.clear();
      cstPt.clear();
      cstEta.clear();
      cstPhi.clear();
      return;
    }  // end 'Reset()'



    void SetTreeAddresses(TTree* tree) {
      tree -> Branch("EvtNumJets",     &nJets,           "EvtNumJets/I");
      tree -> Branch("EvtNumChrgPars", &nChrgPars,       "EvtNumChrgPars/I");
      tree -> Branch("EvtVtxX",        &vtxX,            "EvtVtxX/D");
      tree -> Branch("EvtVtxY",        &vtxY,            "EvtVtxY/D");
      tree -> Branch("EvtVtxZ",        &vtxZ,            "EvtVtxZ/D");
      tree -> Branch("EvtSumParEne",   &eSumPar,         "EvtSumParEne/D");
      tree -> Branch("Parton3_ID",     &partonID.first,  "Parton3_ID/I");
      tree -> Branch("Parton4_ID",     &partonID.second, "Parton4_ID/I");
      tree -> Branch("Parton3_MomX",   &partonPX.first,  "Parton3_MomX/D");
      tree -> Branch("Parton3_MomY",   &partonPY.first,  "Parton3_MomY/D");
      tree -> Branch("Parton3_MomZ",   &partonPZ.first,  "Parton3_MomZ/D");
      tree -> Branch("Parton4_MomX",   &partonPX.second, "Parton4_MomX/D");
      tree -> Branch("Parton4_MomY",   &partonPY.second, "Parton4_MomY/D");
      tree -> Branch("Parton4_MomZ",   &partonPZ.second, "Parton4_MomZ/D");
      tree -> Branch("JetNumCst",      &jetNCst);
      tree -> Branch("JetID",          &jetID);
      tree -> Branch("JetEnergy",      &jetE);
      tree -> Branch("JetPt",          &jetPt);
      tree -> Branch("JetEta",         &jetEta);
      tree -> Branch("JetPhi",         &jetPhi);
      tree -> Branch("JetArea",        &jetArea);
      tree -> Branch("CstID",          &cstID);
      tree -> Branch("CstPID",         &cstPID);
      tree -> Branch("CstType",        &cstType);
      tree -> Branch("CstEmbedID",     &cstEmbedID);
      tree -> Branch("CstZ",           &cstZ);
      tree -> Branch("CstDr",          &cstDr);
      tree -> Branch("CstEnergy",      &cstE);
      tree -> Branch("CstPt",          &cstPt);
      tree -> Branch("CstEta",         &cstEta);
      tree -> Branch("CstPhi",         &cstPhi);
      return;
    }  // end 'SetTreeAddresses(TTree*)'



    void GetTreeMakerOutput(SCorrelatorJetTreeMakerTruthOutput& output) {

      // get event variables
      nJets     = output.jets.size();
      nChrgPars = output.evt.GetNChrgPar();
      eSumPar   = output.evt.GetESumChrg() + output.evt.GetESumNeu();
      vtxX      = output.evt.GetPartonA().GetVX();
      vtxY      = output.evt.GetPartonA().GetVY();
      vtxZ      = output.evt.GetPartonA().GetVZ();

      // get parton variables
      partonID = make_pair(output.evt.GetPartonA().GetPID(), output.evt.GetPartonB().GetPID());
      partonPX = make_pair(output.evt.GetPartonA().GetPX(),  output.evt.GetPartonB().GetPX());
      partonPY = make_pair(output.evt.GetPartonA().GetPY(),  output.evt.GetPartonB().GetPY());
      partonPZ = make_pair(output.evt.GetPartonA().GetPZ(),  output.evt.GetPartonB().GetPZ());

      // get jet variables
      for (Types::JetInfo& jet : output.jets) {
        jetNCst.push_back( jet.GetNCsts() );
        jetID.push_back( jet.GetJetID() );
        jetE.push_back( jet.GetEne() );
        jetPt.push_back( jet.GetPT() );
        jetEta.push_back( jet.GetEta() );
        jetPhi.push_back( jet.GetPhi() );
        jetArea.push_back( jet.GetArea() );
      }

      // get constituent variables
      cstID.resize( output.csts.size() );
      cstPID.resize( output.csts.size() );
      cstType.resize( output.csts.size() );
      cstEmbedID.resize( output.csts.size() );
      cstZ.resize( output.csts.size() );
      cstDr.resize( output.csts.size() );
      cstE.resize( output.csts.size() );
      cstPt.resize( output.csts.size() );
      cstEta.resize( output.csts.size() );
      cstPhi.resize( output.csts.size() );
      for (uint64_t iJet = 0; iJet < output.csts.size(); iJet++) {
        for (Types::CstInfo& cst : output.csts.at(iJet)) {
          cstID.at(iJet).push_back( cst.GetCstID() );
          cstPID.at(iJet).push_back( cst.GetPID() );
          cstType.at(iJet).push_back( cst.GetType() );
          cstEmbedID.at(iJet).push_back( cst.GetEmbedID() );
          cstZ.at(iJet).push_back( cst.GetZ() );
          cstDr.at(iJet).push_back( cst.GetDR() );
          cstE.at(iJet).push_back( cst.GetEne() );
          cstPt.at(iJet).push_back( cst.GetPT() );
          cstEta.at(iJet).push_back( cst.GetEta() );
          cstPhi.at(iJet).push_back( cst.GetPhi() );
        }
      }
      return;

    }  // end 'GetTreeMakerOutput(SCorrelatorJetTreeMakerTruthOutput&)'

  };  // end SCorrelatorJetTreeMakerLegacyTruthOutput

}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------
