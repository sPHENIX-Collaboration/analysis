// ----------------------------------------------------------------------------
// 'SCorrelatorJetTree.system.h'
// Derek Anderson
// 01.18.2023
//
// Class to construct a tree of
// jets from a specified set of
// events.
//
// Methods relevant to various
// internal operations are
// collected here.
//
// Derived from code by Antonio
// Silva (thanks!!)
// ----------------------------------------------------------------------------

#pragma once

using namespace std;
using namespace findNode;



// system methods -------------------------------------------------------------

void SCorrelatorJetTree::InitVariables() {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::InitVariables() Initializing class members..." << endl;
  }

  // initialize class members as needed
  m_histMan              = 0x0;
  m_evalStack            = 0x0;
  m_trackEval            = 0x0;
  m_outFile              = 0x0;
  m_trueTree             = 0x0;
  m_recoTree             = 0x0;
  m_matchTree            = 0x0;
  m_doQualityPlots       = true;
  m_saveDST              = false;
  m_addTracks            = true;
  m_addFlow              = false;
  m_addECal              = false;
  m_addHCal              = false;
  m_parPtRange[0]        = 0.;
  m_parPtRange[1]        = 9999.;
  m_parEtaRange[0]       = -1.1;
  m_parEtaRange[1]       = 1.1;
  m_trkPtRange[0]        = 0.;
  m_trkPtRange[1]        = 9999.;
  m_trkEtaRange[0]       = -1.1;
  m_trkEtaRange[1]       = 1.1;
  m_flowPtRange[0]       = 0.;
  m_flowPtRange[1]       = 9999.;
  m_flowEtaRange[0]      = -1.1;
  m_flowEtaRange[1]      = 1.1;
  m_ecalPtRange[0]       = 0.;
  m_ecalPtRange[1]       = 9999;
  m_ecalEtaRange[0]      = -1.1;
  m_ecalEtaRange[1]      = 1.1;
  m_hcalPtRange[0]       = 0.;
  m_hcalPtRange[1]       = 9999.;
  m_hcalEtaRange[0]      = -1.1;
  m_hcalEtaRange[1]      = 1.1;
  m_jetR                 = 0.4;
  m_jetType              = 0;
  m_jetAlgo              = antikt_algorithm;
  m_recombScheme         = pt_scheme;
  m_partonID[0]           = -9999;
  m_partonID[1]           = -9999;
  m_partonMom[0]          = CLHEP::Hep3Vector(-9999., -9999., -9999.);
  m_partonMom[1]          = CLHEP::Hep3Vector(-9999., -9999., -9999.);
  m_recoVtx               = CLHEP::Hep3Vector(-9999., -9999., -9999.);
  m_trueVtx               = CLHEP::Hep3Vector(-9999., -9999., -9999.);
  m_trueNumJets           = 0;
  m_trueNumChrgPars       = -9999;
  m_trueSumPar            = -9999.;
  m_truePartonID[0]       = -9999;
  m_truePartonID[1]       = -9999;
  m_truePartonMomX[0]     = -9999.;
  m_truePartonMomX[1]     = -9999.;
  m_truePartonMomY[0]     = -9999.;
  m_truePartonMomY[1]     = -9999.;
  m_truePartonMomZ[0]     = -9999.;
  m_truePartonMomZ[1]     = -9999.;
  m_trueVtxX              = -9999.;
  m_trueVtxY              = -9999.;
  m_trueVtxZ              = -9999.;
  m_recoNumJets           = 0;
  m_recoVtxX              = -9999.;
  m_recoVtxY              = -9999.;
  m_recoVtxZ              = -9999.;
  m_recoNumTrks           = -9999;
  m_recoSumECal           = -9999.;
  m_recoSumHCal           = -9999.;
  m_trueJets.clear();
  m_recoJets.clear();
  m_trueJetNCst.clear();
  m_trueJetID.clear();
  m_trueJetE.clear();
  m_trueJetPt.clear();
  m_trueJetEta.clear();
  m_trueJetPhi.clear();
  m_trueJetArea.clear();
  m_trueCstZ.clear();
  m_trueCstDr.clear();
  m_trueCstE.clear();
  m_trueCstJt.clear();
  m_trueCstEta.clear();
  m_trueCstPhi.clear();
  m_recoJetNCst.clear();
  m_recoJetID.clear();
  m_recoJetE.clear();
  m_recoJetPt.clear();
  m_recoJetEta.clear();
  m_recoJetPhi.clear();
  m_recoJetArea.clear();
  m_recoCstZ.clear();
  m_recoCstDr.clear();
  m_recoCstE.clear();
  m_recoCstJt.clear();
  m_recoCstEta.clear();
  m_recoCstPhi.clear();
  return;

}  // end 'InitVariables()'



void SCorrelatorJetTree::InitHists() {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::InitHists() Initializing QA histograms..." << endl;
  }

  // binning
  const unsigned long nNumBins(500);
  const unsigned long nEneBins(200);
  const unsigned long nPhiBins(63);
  const unsigned long nEtaBins(40);
  const unsigned long nPtBins(200);
  const unsigned long nAreaBins(1000);
  const double        rNumBins[NRange]  = {0.,    500.};
  const double        rPhiBins[NRange]  = {-3.15, 3.15};
  const double        rEtaBins[NRange]  = {-2.,   2.};
  const double        rEneBins[NRange]  = {0.,    100.};
  const double        rPtBins[NRange]   = {0.,    100.};
  const double        rAreaBins[NRange] = {0.,    10.};

  m_outFile -> cd();
  // no. of objects in acceptance
  m_hNumObject[OBJECT::TRACK]             = new TH1D("hNumTrks",       "N_{accept} (tracks)",       nNumBins,  rNumBins[0],  rNumBins[1]);
  m_hNumObject[OBJECT::ECLUST]            = new TH1D("hNumEClust",     "N_{accept} (EMCal clust.)", nNumBins,  rNumBins[0],  rNumBins[1]);
  m_hNumObject[OBJECT::HCLUST]            = new TH1D("hNumHClust",     "N_{accept} (HCal clust.)",  nNumBins,  rNumBins[0],  rNumBins[1]);
  m_hNumObject[OBJECT::FLOW]              = new TH1D("hNumFlow",       "N_{accept} (flow)",         nNumBins,  rNumBins[0],  rNumBins[1]);
  m_hNumObject[OBJECT::PART]              = new TH1D("hNumPart",       "N_{accept} (par.s)",        nNumBins,  rNumBins[0],  rNumBins[1]);
  m_hNumObject[OBJECT::TJET]              = new TH1D("hNumTruthJet",   "N_{jet} (truth)",           nNumBins,  rNumBins[0],  rNumBins[1]);
  m_hNumObject[OBJECT::RJET]              = new TH1D("hNumRecoJets",   "N_{jet} (reco.)",           nNumBins,  rNumBins[0],  rNumBins[1]);
  m_hNumObject[OBJECT::TCST]              = new TH1D("hNumTruthCst",   "N_{cst} (truth)",           nNumBins,  rNumBins[0],  rNumBins[1]);
  m_hNumObject[OBJECT::RCST]              = new TH1D("hNumRecoCst",    "N_{cst} (reco.)",           nNumBins,  rNumBins[0],  rNumBins[1]);
  // sum of cst. energies
  m_hSumCstEne[CST_TYPE::PART_CST]        = new TH1D("hSumPartEne",    "#SigmaE (cst. par.s)",      nEneBins,  rEneBins[0],  rEneBins[1]);
  m_hSumCstEne[CST_TYPE::FLOW_CST]        = new TH1D("hSumFlowEne",    "#SigmaE (cst. flow)",       nEneBins,  rEneBins[0],  rEneBins[1]);
  m_hSumCstEne[CST_TYPE::TRACK_CST]       = new TH1D("hSumTrackEne",   "#SigmaE (cst. track)",      nEneBins,  rEneBins[0],  rEneBins[1]);
  m_hSumCstEne[CST_TYPE::ECAL_CST]        = new TH1D("hSumECalEne",    "#SigmaE (cst. ecal)",       nEneBins,  rEneBins[0],  rEneBins[1]);
  m_hSumCstEne[CST_TYPE::HCAL_CST]        = new TH1D("hSumHCalEne",    "#SigmaE (cst. hcal)",       nEneBins,  rEneBins[0],  rEneBins[1]);
  // particle QA
  m_hObjectQA[OBJECT::PART][INFO::PT]     = new TH1D("hPartPt",        "p_{T} (par.s)",             nPtBins,   rPtBins[0],   rPtBins[1]);
  m_hObjectQA[OBJECT::PART][INFO::ETA]    = new TH1D("hPartEta",       "#eta (par.s)",              nEtaBins,  rEtaBins[0],  rEtaBins[1]);
  m_hObjectQA[OBJECT::PART][INFO::PHI]    = new TH1D("hPartPhi",       "#phi (par.s)",              nPhiBins,  rPhiBins[0],  rPhiBins[1]);
  m_hObjectQA[OBJECT::PART][INFO::ENE]    = new TH1D("hPartEne",       "E (par.s)",                 nEneBins,  rEneBins[0],  rEneBins[1]);
  // track QA
  m_hObjectQA[OBJECT::TRACK][INFO::PT]    = new TH1D("hTrackPt",       "p_{T} (tracks)",            nPtBins,   rPtBins[0],   rPtBins[1]);
  m_hObjectQA[OBJECT::TRACK][INFO::ETA]   = new TH1D("hTrackEta",      "#eta (tracks)",             nEtaBins,  rEtaBins[0],  rEtaBins[1]);
  m_hObjectQA[OBJECT::TRACK][INFO::PHI]   = new TH1D("hTrackPhi",      "#phi (tracks)",             nPhiBins,  rPhiBins[0],  rPhiBins[1]);
  m_hObjectQA[OBJECT::TRACK][INFO::ENE]   = new TH1D("hTrackEne",      "E (tracks)",                nEneBins,  rEneBins[0],  rEneBins[1]);
  // particle flow QA
  m_hObjectQA[OBJECT::FLOW][INFO::PT]     = new TH1D("hFlowPt",        "p_{T} (flow)",              nPtBins,   rPtBins[0],   rPtBins[1]);
  m_hObjectQA[OBJECT::FLOW][INFO::ETA]    = new TH1D("hFlowEta",       "#eta (flow)",               nEtaBins,  rEtaBins[0],  rEtaBins[1]);
  m_hObjectQA[OBJECT::FLOW][INFO::PHI]    = new TH1D("hFlowPhi",       "#phi (flow)",               nPhiBins,  rPhiBins[0],  rPhiBins[1]);
  m_hObjectQA[OBJECT::FLOW][INFO::ENE]    = new TH1D("hFlowEne",       "E (flow)",                  nEneBins,  rEneBins[0],  rEneBins[1]);
  // emcal cluster QA
  m_hObjectQA[OBJECT::ECLUST][INFO::PT]   = new TH1D("hEClustPt",      "p_{T} (EMCal clust.)",      nPtBins,   rPtBins[0],   rPtBins[1]);
  m_hObjectQA[OBJECT::ECLUST][INFO::ETA]  = new TH1D("hEClustEta",     "#eta (EMCal clust.)",       nEtaBins,  rEtaBins[0],  rEtaBins[1]);
  m_hObjectQA[OBJECT::ECLUST][INFO::PHI]  = new TH1D("hEClustPhi",     "#phi (EMCal clust.)",       nPhiBins,  rPhiBins[0],  rPhiBins[1]);
  m_hObjectQA[OBJECT::ECLUST][INFO::ENE]  = new TH1D("hEClustEne",     "E (EMCal clust.)",          nEneBins,  rEneBins[0],  rEneBins[1]);
  // hcal cluster QA
  m_hObjectQA[OBJECT::HCLUST][INFO::PT]   = new TH1D("hHClustPt",      "p_{T} (HCal clust.)",       nPtBins,   rPtBins[0],   rPtBins[1]);
  m_hObjectQA[OBJECT::HCLUST][INFO::ETA]  = new TH1D("hHClustEta",     "#eta (HCal clust.)",        nEtaBins,  rEtaBins[0],  rEtaBins[1]);
  m_hObjectQA[OBJECT::HCLUST][INFO::PHI]  = new TH1D("hHClustPhi",     "#phi (HCal clust.)",        nPhiBins,  rPhiBins[0],  rPhiBins[1]);
  m_hObjectQA[OBJECT::HCLUST][INFO::ENE]  = new TH1D("hHClustEne",     "E (HCal clust.)",           nEneBins,  rEneBins[0],  rEneBins[1]);
  // truth jet QA
  m_hObjectQA[OBJECT::TJET][INFO::PT]     = new TH1D("hTruthJetPt",    "p_{T} (truth jet)",         nPtBins,   rPtBins[0],   rPtBins[1]);
  m_hObjectQA[OBJECT::TJET][INFO::ETA]    = new TH1D("hTruthJetEta",   "#eta (truth jet)",          nEtaBins,  rEtaBins[0],  rEtaBins[1]);
  m_hObjectQA[OBJECT::TJET][INFO::PHI]    = new TH1D("hTruthJetPhi",   "#phi (truth jet)",          nPhiBins,  rPhiBins[0],  rPhiBins[1]);
  m_hObjectQA[OBJECT::TJET][INFO::ENE]    = new TH1D("hTruthJetEne",   "E (truth jet)",             nEneBins,  rEneBins[0],  rEneBins[1]);
  m_hJetArea[0]                           = new TH1D("hTruthJetArea",  "Area (truth jet)",          nAreaBins, rAreaBins[0], rAreaBins[1]);
  m_hJetNumCst[0]                         = new TH1D("hTruthJetNCst",  "N_{cst} (truth jet)",       nNumBins,  rNumBins[0],  rNumBins[1]);
  // reco jet QA
  m_hObjectQA[OBJECT::RJET][INFO::PT]     = new TH1D("hRecoJetPt",     "p_{T} (reco. jet)",         nPtBins,   rPtBins[0],   rPtBins[1]);
  m_hObjectQA[OBJECT::RJET][INFO::ETA]    = new TH1D("hRecoJetEta",    "#eta (reco. jet)",          nEtaBins,  rEtaBins[0],  rEtaBins[1]);
  m_hObjectQA[OBJECT::RJET][INFO::PHI]    = new TH1D("hRecoJetPhi",    "#phi (reco. jet)",          nPhiBins,  rPhiBins[0],  rPhiBins[1]);
  m_hObjectQA[OBJECT::RJET][INFO::ENE]    = new TH1D("hRecoJetEne",    "E (reco. jet)",             nEneBins,  rEneBins[0],  rEneBins[1]);
  m_hJetArea[1]                           = new TH1D("hRecoJetArea",   "Area (reco. jet)",          nAreaBins, rAreaBins[0], rAreaBins[1]);
  m_hJetNumCst[1]                         = new TH1D("hRecoJetNCst",   "N_{cst} (reco. jet)",       nNumBins,  rNumBins[0],  rNumBins[1]);
  // truth cst. QA
  m_hObjectQA[OBJECT::TCST][INFO::PT]     = new TH1D("hTruthCstPt",    "p_{T} (truth cst.)",        nPtBins,   rPtBins[0],   rPtBins[1]);
  m_hObjectQA[OBJECT::TCST][INFO::ETA]    = new TH1D("hTruthCstEta",   "#eta (truth cst.)",         nEtaBins,  rEtaBins[0],  rEtaBins[1]);
  m_hObjectQA[OBJECT::TCST][INFO::PHI]    = new TH1D("hTruthCstPhi",   "#phi (truth cst.)",         nPhiBins,  rPhiBins[0],  rPhiBins[1]);
  m_hObjectQA[OBJECT::TCST][INFO::ENE]    = new TH1D("hTruthCstEne",   "E (truth cst.)",            nEneBins,  rEneBins[0],  rEneBins[1]);
  // reco cst. QA
  m_hObjectQA[OBJECT::RCST][INFO::PT]     = new TH1D("hRecoCstPt",     "p_{T} (reco. cst.)",        nPtBins,   rPtBins[0],   rPtBins[1]);
  m_hObjectQA[OBJECT::RCST][INFO::ETA]    = new TH1D("hRecoCstEta",    "#eta (reco. cst.)",         nEtaBins,  rEtaBins[0],  rEtaBins[1]);
  m_hObjectQA[OBJECT::RCST][INFO::PHI]    = new TH1D("hRecoCstPhi",    "#phi (reco. cst.)",         nPhiBins,  rPhiBins[0],  rPhiBins[1]);
  m_hObjectQA[OBJECT::RCST][INFO::ENE]    = new TH1D("hRecoCstEne",    "E (reco. cst.)",            nEneBins,  rEneBins[0],  rEneBins[1]);
  // no. of cst.s
  m_hNumCstAccept[CST_TYPE::PART_CST][0]  = new TH1D("hNumPartCstTot", "N_{cst}^{par} total",       nNumBins,  rNumBins[0],  rNumBins[1]);
  m_hNumCstAccept[CST_TYPE::PART_CST][1]  = new TH1D("hNumPartCstAcc", "N_{cst}^{par} accepted",    nNumBins,  rNumBins[0],  rNumBins[1]);
  m_hNumCstAccept[CST_TYPE::TRACK_CST][0] = new TH1D("hNumTrkCstTot",  "N_{cst}^{trk} total",       nNumBins,  rNumBins[0],  rNumBins[1]);
  m_hNumCstAccept[CST_TYPE::TRACK_CST][1] = new TH1D("hNumTrkCstAcc",  "N_{cst}^{trk} accepted",    nNumBins,  rNumBins[0],  rNumBins[1]);
  m_hNumCstAccept[CST_TYPE::FLOW_CST][0]  = new TH1D("hNumFlowCstTot", "N_{cst}^{flow} total",      nNumBins,  rNumBins[0],  rNumBins[1]);
  m_hNumCstAccept[CST_TYPE::FLOW_CST][1]  = new TH1D("hNumFlowCstAcc", "N_{cst}^{flow} accepted",   nNumBins,  rNumBins[0],  rNumBins[1]);
  m_hNumCstAccept[CST_TYPE::ECAL_CST][0]  = new TH1D("hNumECalCstTot", "N_{cst}^{clust} total",     nNumBins,  rNumBins[0],  rNumBins[1]);
  m_hNumCstAccept[CST_TYPE::ECAL_CST][1]  = new TH1D("hNumECalCstAcc", "N_{cst}^{clust} accepted",  nNumBins,  rNumBins[0],  rNumBins[1]);
  m_hNumCstAccept[CST_TYPE::HCAL_CST][0]  = new TH1D("hNumHCalCstTot", "N_{cst}^{clust} total",     nNumBins,  rNumBins[0],  rNumBins[1]);
  m_hNumCstAccept[CST_TYPE::HCAL_CST][1]  = new TH1D("hNumHCalCstAcc", "N_{cst}^{clust} accepted",  nNumBins,  rNumBins[0],  rNumBins[1]);

}  // end 'InitHists()'



void SCorrelatorJetTree::InitTrees() {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::InitTrees() Initializing output trees..." << endl;
  }

  // initialize true jet tree
  m_trueTree = new TTree("TruthJetTree", "A tree of truth jets");
  m_trueTree -> Branch("EvtNumJets",     &m_trueNumJets,       "EvtNumJets/I");
  m_trueTree -> Branch("EvtNumChrgPars", &m_trueNumChrgPars,   "EvtNumChrgPars/I");
  m_trueTree -> Branch("Parton3_ID",     &m_truePartonID[0],   "Parton3_ID/I");
  m_trueTree -> Branch("Parton4_ID",     &m_truePartonID[1],   "Parton4_ID/I");
  m_trueTree -> Branch("Parton3_MomX",   &m_truePartonMomX[0], "Parton3_MomX/D");
  m_trueTree -> Branch("Parton3_MomY",   &m_truePartonMomY[0], "Parton3_MomY/D");
  m_trueTree -> Branch("Parton3_MomZ",   &m_truePartonMomZ[0], "Parton3_MomZ/D");
  m_trueTree -> Branch("Parton4_MomX",   &m_truePartonMomX[1], "Parton4_MomX/D");
  m_trueTree -> Branch("Parton4_MomY",   &m_truePartonMomY[1], "Parton4_MomY/D");
  m_trueTree -> Branch("Parton4_MomZ",   &m_truePartonMomZ[1], "Parton4_MomZ/D");
  m_trueTree -> Branch("EvtVtxX",        &m_trueVtxX,          "EvtVtxX/D");
  m_trueTree -> Branch("EvtVtxY",        &m_trueVtxY,          "EvtVtxY/D");
  m_trueTree -> Branch("EvtVtxZ",        &m_trueVtxZ,          "EvtVtxZ/D");
  m_trueTree -> Branch("EvtSumParEne",   &m_trueSumPar,        "EvtSumParEne/D");
  m_trueTree -> Branch("JetNumCst",      &m_trueJetNCst);
  m_trueTree -> Branch("JetID",          &m_trueJetID);
  m_trueTree -> Branch("JetEnergy",      &m_trueJetE);
  m_trueTree -> Branch("JetPt",          &m_trueJetPt);
  m_trueTree -> Branch("JetEta",         &m_trueJetEta);
  m_trueTree -> Branch("JetPhi",         &m_trueJetPhi);
  m_trueTree -> Branch("JetArea",        &m_trueJetArea);
  m_trueTree -> Branch("CstID",          &m_trueCstID);
  m_trueTree -> Branch("CstZ",           &m_trueCstZ);
  m_trueTree -> Branch("CstDr",          &m_trueCstDr);
  m_trueTree -> Branch("CstEnergy",      &m_trueCstE);
  m_trueTree -> Branch("CstJt",          &m_trueCstJt);
  m_trueTree -> Branch("CstEta",         &m_trueCstEta);
  m_trueTree -> Branch("CstPhi",         &m_trueCstPhi);

  // initialize reco jet tree
  m_recoTree = new TTree("RecoJetTree", "A tree of reconstructed jets");
  m_recoTree -> Branch("EvtNumJets",    &m_recoNumJets, "EvtNumJets/I");
  m_recoTree -> Branch("EvtNumTrks",    &m_recoNumTrks, "EvtNumTrks/I");
  m_recoTree -> Branch("EvtVtxX",       &m_recoVtxX,    "EvtVtxX/D");
  m_recoTree -> Branch("EvtVtxY",       &m_recoVtxY,    "EvtVtxY/D");
  m_recoTree -> Branch("EvtVtxZ",       &m_recoVtxZ,    "EvtVtxZ/D");
  m_recoTree -> Branch("EvtSumECalEne", &m_recoSumECal, "EvtSumECalEne/D");
  m_recoTree -> Branch("EvtSumHCalEne", &m_recoSumHCal, "EvtSumHCalEne/D");
  m_recoTree -> Branch("JetNumCst",     &m_recoJetNCst);
  m_recoTree -> Branch("JetID",         &m_recoJetID);
  m_recoTree -> Branch("JetEnergy",     &m_recoJetE);
  m_recoTree -> Branch("JetPt",         &m_recoJetPt);
  m_recoTree -> Branch("JetEta",        &m_recoJetEta);
  m_recoTree -> Branch("JetPhi",        &m_recoJetPhi);
  m_recoTree -> Branch("JetArea",       &m_recoJetArea);
  m_recoTree -> Branch("CstMatchID",    &m_recoCstMatchID);
  m_recoTree -> Branch("CstZ",          &m_recoCstZ);
  m_recoTree -> Branch("CstDr",         &m_recoCstDr);
  m_recoTree -> Branch("CstEnergy",     &m_recoCstE);
  m_recoTree -> Branch("CstJt",         &m_recoCstJt);
  m_recoTree -> Branch("CstEta",        &m_recoCstEta);
  m_recoTree -> Branch("CstPhi",        &m_recoCstPhi);
  return;

}  // end 'InitTrees()'



void SCorrelatorJetTree::InitEvals(PHCompositeNode *topNode) {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::InitEvals(PHCompositeNode*) Initializing evaluators..." << endl;
  }

  m_evalStack = new SvtxEvalStack(topNode);
  if (!m_evalStack) {
    cerr << "SCorrelatorJetTree::InitEvals(PHCompositeNode*) PANIC: couldn't grab SvtxEvalStack! Aborting!" << endl;
    assert(m_evalStack);
  } else {
    m_evalStack -> next_event(topNode);
  }

  m_trackEval = m_evalStack -> get_track_eval();
  if (!m_trackEval) {
    cerr << "SCorrelatorJetTree::InitEvals(PHCompositeNode*) PANIC: couldn't grab track evaluator! Aborting!" << endl;
    assert(m_trackEval);
  }
  return;

}  // end 'InitEvals(PHCompositeNode*)'



void SCorrelatorJetTree::FillTrueTree() {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::FillTrueTree() Filling truth jet tree..." << endl;
  }

  // prepare vectors for filling
  m_trueJetNCst.clear();
  m_trueJetID.clear();
  m_trueJetE.clear();
  m_trueJetPt.clear();
  m_trueJetEta.clear();
  m_trueJetPhi.clear();
  m_trueJetArea.clear();
  m_trueCstID.clear();
  m_trueCstZ.clear();
  m_trueCstDr.clear();
  m_trueCstE.clear();
  m_trueCstJt.clear();
  m_trueCstEta.clear();
  m_trueCstPhi.clear();

  // declare vectors to storing constituents
  vector<int>    vecTruCstID;
  vector<double> vecTruCstZ;
  vector<double> vecTruCstDr;
  vector<double> vecTruCstE;
  vector<double> vecTruCstJt;
  vector<double> vecTruCstEta;
  vector<double> vecTruCstPhi;
  vecTruCstID.clear();
  vecTruCstZ.clear();
  vecTruCstDr.clear();
  vecTruCstE.clear();
  vecTruCstJt.clear();
  vecTruCstEta.clear();
  vecTruCstPhi.clear();

  // fill jets/constituent variables
  unsigned int nTruJet(0);
  unsigned int nTruCst(0);
  for (unsigned int iTruJet = 0; iTruJet < m_trueJets.size(); ++iTruJet) {

    // get jet info
    const unsigned int jetNCst  = m_trueJets[iTruJet].constituents().size();
    const unsigned int jetTruID = iTruJet;
    const double       jetPhi   = m_trueJets[iTruJet].phi_std();
    const double       jetEta   = m_trueJets[iTruJet].pseudorapidity();
    const double       jetArea  = 0.;  // FIXME: jet area needs to be defined
    const double       jetE     = m_trueJets[iTruJet].E();
    const double       jetPt    = m_trueJets[iTruJet].perp();
    const double       jetPx    = m_trueJets[iTruJet].px();
    const double       jetPy    = m_trueJets[iTruJet].py();
    const double       jetPz    = m_trueJets[iTruJet].pz();
    const double       jetP     = sqrt((jetPx * jetPx) + (jetPy * jetPy) + (jetPz * jetPz));

    // clear constituent vectors
    vecTruCstID.clear();
    vecTruCstZ.clear();
    vecTruCstDr.clear();
    vecTruCstE.clear();
    vecTruCstJt.clear();
    vecTruCstEta.clear();
    vecTruCstPhi.clear();

    // loop over constituents
    vector<fastjet::PseudoJet> trueCsts = m_trueJets[iTruJet].constituents();
    for (unsigned int iTruCst = 0; iTruCst < trueCsts.size(); ++iTruCst) {

      // get constituent info
      const int    cstID  = trueCsts[iTruCst].user_index();
      const double cstPhi = trueCsts[iTruCst].phi_std();
      const double cstEta = trueCsts[iTruCst].pseudorapidity();
      const double cstE   = trueCsts[iTruCst].E();
      const double cstJt  = trueCsts[iTruCst].perp();
      const double cstJx  = trueCsts[iTruCst].px();
      const double cstJy  = trueCsts[iTruCst].py();
      const double cstJz  = trueCsts[iTruCst].pz();
      const double cstJ   = ((cstJx * cstJx) + (cstJy * cstJy) + (cstJz * cstJz));
      const double cstZ   = cstJ / jetP;
      const double cstDf  = cstPhi - jetPhi;
      const double cstDh  = cstEta - jetEta;
      const double cstDr  = sqrt((cstDf * cstDf) + (cstDh * cstDh));

      // add csts to vectors
      vecTruCstID.push_back(cstID);
      vecTruCstZ.push_back(cstZ);
      vecTruCstDr.push_back(cstDr);
      vecTruCstE.push_back(cstE);
      vecTruCstJt.push_back(cstJt);
      vecTruCstEta.push_back(cstEta);
      vecTruCstPhi.push_back(cstPhi);

      // fill QA histograms and increment counters
      m_hObjectQA[OBJECT::TCST][INFO::PT]  -> Fill(cstJt);
      m_hObjectQA[OBJECT::TCST][INFO::ETA] -> Fill(cstEta);
      m_hObjectQA[OBJECT::TCST][INFO::PHI] -> Fill(cstPhi);
      m_hObjectQA[OBJECT::TCST][INFO::ENE] -> Fill(cstE);
      ++nTruCst;
    }  // end constituent loop

    // store jet/cst output
    m_trueJetNCst.push_back(jetNCst);
    m_trueJetID.push_back(jetTruID);
    m_trueJetE.push_back(jetE);
    m_trueJetPt.push_back(jetPt);
    m_trueJetEta.push_back(jetEta);
    m_trueJetPhi.push_back(jetPhi);
    m_trueJetArea.push_back(jetArea);
    m_trueCstID.push_back(vecTruCstID);
    m_trueCstZ.push_back(vecTruCstZ);
    m_trueCstDr.push_back(vecTruCstDr);
    m_trueCstE.push_back(vecTruCstE);
    m_trueCstJt.push_back(vecTruCstJt);
    m_trueCstEta.push_back(vecTruCstEta);
    m_trueCstPhi.push_back(vecTruCstPhi);

    // fill QA histograms and increment counters
    m_hJetArea[0]                        -> Fill(jetArea);
    m_hJetNumCst[0]                      -> Fill(jetNCst);
    m_hObjectQA[OBJECT::TJET][INFO::PT]  -> Fill(jetPt);
    m_hObjectQA[OBJECT::TJET][INFO::ETA] -> Fill(jetEta);
    m_hObjectQA[OBJECT::TJET][INFO::PHI] -> Fill(jetPhi);
    m_hObjectQA[OBJECT::TJET][INFO::ENE] -> Fill(jetE);
    ++nTruJet;
  }  // end jet loop

  // fill QA histograms
  m_hNumObject[OBJECT::TJET] -> Fill(nTruJet);
  m_hNumObject[OBJECT::TCST] -> Fill(nTruCst);

  // store evt info
  m_trueNumJets       = nTruJet;
  m_truePartonID[0]   = m_partonID[0];
  m_truePartonID[1]   = m_partonID[1];
  m_truePartonMomX[0] = m_partonMom[0].x();
  m_truePartonMomX[1] = m_partonMom[1].x();
  m_truePartonMomY[0] = m_partonMom[0].y();
  m_truePartonMomY[1] = m_partonMom[1].y();
  m_truePartonMomZ[0] = m_partonMom[0].z();
  m_truePartonMomZ[1] = m_partonMom[1].z(); 
  m_trueVtxX          = m_trueVtx.x();
  m_trueVtxY          = m_trueVtx.y();
  m_trueVtxZ          = m_trueVtx.z();

  // fill output tree
  m_trueTree -> Fill();
  return;

}  // end 'FillTrueTree()'



void SCorrelatorJetTree::FillRecoTree() {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::FillRecoTree() Filling reco jet tree..." << endl;
  }

  // prepare vectors for filling
  m_recoJetNCst.clear();
  m_recoJetID.clear();
  m_recoJetE.clear();
  m_recoJetPt.clear();
  m_recoJetEta.clear();
  m_recoJetPhi.clear();
  m_recoJetArea.clear();
  m_recoCstMatchID.clear();
  m_recoCstZ.clear();
  m_recoCstDr.clear();
  m_recoCstE.clear();
  m_recoCstJt.clear();
  m_recoCstEta.clear();
  m_recoCstPhi.clear();

  // declare vectors for storing constituents
  vector<int>    vecRecCstMatchID;
  vector<double> vecRecCstZ;
  vector<double> vecRecCstDr;
  vector<double> vecRecCstE;
  vector<double> vecRecCstJt;
  vector<double> vecRecCstEta;
  vector<double> vecRecCstPhi;
  vecRecCstMatchID.clear();
  vecRecCstZ.clear();
  vecRecCstDr.clear();
  vecRecCstE.clear();
  vecRecCstJt.clear();
  vecRecCstEta.clear();
  vecRecCstPhi.clear();

  // fill jet/constituent variables
  unsigned long nRecJet(0);
  unsigned long nRecCst(0);
  for (unsigned int iJet = 0; iJet < m_recoJets.size(); ++iJet) {

    // get jet info
    const unsigned int jetNCst  = m_recoJets[iJet].constituents().size();
    const unsigned int jetRecID = iJet;
    const double       jetPhi   = m_recoJets[iJet].phi_std();
    const double       jetEta   = m_recoJets[iJet].pseudorapidity();
    const double       jetArea  = 0.;  // FIXME: jet area needs to be defined
    const double       jetE     = m_recoJets[iJet].E();
    const double       jetPt    = m_recoJets[iJet].perp();
    const double       jetPx    = m_recoJets[iJet].px();
    const double       jetPy    = m_recoJets[iJet].py();
    const double       jetPz    = m_recoJets[iJet].pz();
    const double       jetP     = sqrt((jetPx * jetPx) + (jetPy * jetPy) + (jetPz * jetPz));

    // clear constituent vectors
    vecRecCstMatchID.clear();
    vecRecCstZ.clear();
    vecRecCstDr.clear();
    vecRecCstE.clear();
    vecRecCstJt.clear();
    vecRecCstEta.clear();
    vecRecCstPhi.clear();

    // loop over constituents
    vector<fastjet::PseudoJet> recoCsts = m_recoJets[iJet].constituents();
    for (unsigned int iCst = 0; iCst < recoCsts.size(); ++iCst) {

      // get constituent info
      const double cstMatchID = recoCsts[iCst].user_index();
      const double cstPhi     = recoCsts[iCst].phi_std();
      const double cstEta     = recoCsts[iCst].pseudorapidity();
      const double cstE       = recoCsts[iCst].E();
      const double cstJt      = recoCsts[iCst].perp();
      const double cstJx      = recoCsts[iCst].px();
      const double cstJy      = recoCsts[iCst].py();
      const double cstJz      = recoCsts[iCst].pz();
      const double cstJ       = ((cstJx * cstJx) + (cstJy * cstJy) + (cstJz * cstJz));
      const double cstZ       = cstJ / jetP;
      const double cstDf      = cstPhi - jetPhi;
      const double cstDh      = cstEta - jetEta;
      const double cstDr      = sqrt((cstDf * cstDf) + (cstDh * cstDh));

      // add csts to vectors
      vecRecCstMatchID.push_back(cstMatchID);
      vecRecCstZ.push_back(cstZ);
      vecRecCstDr.push_back(cstDr);
      vecRecCstE.push_back(cstE);
      vecRecCstJt.push_back(cstJt);
      vecRecCstEta.push_back(cstEta);
      vecRecCstPhi.push_back(cstPhi);

      // fill QA histograms and increment counters
      m_hObjectQA[OBJECT::RCST][INFO::PT]  -> Fill(cstJt);
      m_hObjectQA[OBJECT::RCST][INFO::ETA] -> Fill(cstEta);
      m_hObjectQA[OBJECT::RCST][INFO::PHI] -> Fill(cstPhi);
      m_hObjectQA[OBJECT::RCST][INFO::ENE] -> Fill(cstE);
      ++nRecCst;
    }  // end constituent loop

    // store jet/cst output
    m_recoJetNCst.push_back(jetNCst);
    m_recoJetID.push_back(jetRecID);
    m_recoJetE.push_back(jetE);
    m_recoJetPt.push_back(jetPt);
    m_recoJetEta.push_back(jetEta);
    m_recoJetPhi.push_back(jetPhi);
    m_recoJetArea.push_back(jetArea);
    m_recoCstMatchID.push_back(vecRecCstMatchID);
    m_recoCstZ.push_back(vecRecCstZ);
    m_recoCstDr.push_back(vecRecCstDr);
    m_recoCstE.push_back(vecRecCstE);
    m_recoCstJt.push_back(vecRecCstJt);
    m_recoCstEta.push_back(vecRecCstEta);
    m_recoCstPhi.push_back(vecRecCstPhi);

    // fill QA histograms and increment counters
    m_hJetArea[1]                        -> Fill(jetArea);
    m_hJetNumCst[1]                      -> Fill(jetNCst);
    m_hObjectQA[OBJECT::RJET][INFO::PT]  -> Fill(jetPt);
    m_hObjectQA[OBJECT::RJET][INFO::ETA] -> Fill(jetEta);
    m_hObjectQA[OBJECT::RJET][INFO::PHI] -> Fill(jetPhi);
    m_hObjectQA[OBJECT::RJET][INFO::ENE] -> Fill(jetE);
    ++nRecJet;
  }  // end jet loop

  // fill QA histograms
  m_hNumObject[OBJECT::RJET] -> Fill(nRecJet);
  m_hNumObject[OBJECT::RCST] -> Fill(nRecCst);

  // store event info
  m_recoNumJets = nRecJet;
  m_recoVtxX    = m_recoVtx.x();
  m_recoVtxY    = m_recoVtx.y();
  m_recoVtxZ    = m_recoVtx.z();

  // fill object tree
  m_recoTree -> Fill();
  return;

}  // end 'FillRecoTree()'



void SCorrelatorJetTree::SaveOutput() {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::SaveOutput() Saving output trees and histograms..." << endl;
  }

  // save QA histograms if need be
  const string  sQuality[NDirectory + 1] = {"Tracks", "CaloClusters", "ParticleFlow", "Particles", "TruthJets", "RecoJets", "QA"};
  TDirectory   *dQuality[NDirectory + 1];
  if (m_doQualityPlots) {

    // create QA directories
    dQuality[NDirectory] = (TDirectory*) m_outFile -> mkdir(sQuality[NDirectory].data());
    for (size_t iDirect = 0; iDirect < NDirectory; iDirect++) {
      dQuality[iDirect] = (TDirectory*) dQuality[NDirectory] -> mkdir(sQuality[iDirect].data());
    }

    // save object-specific QA hists
    for (size_t iObj = OBJECT::TRACK; iObj < NObjType; iObj++) {
      switch (iObj) {
        case OBJECT::TRACK:
          dQuality[0] -> cd();
          break;
        case OBJECT::HCLUST:
          dQuality[1] -> cd();
          break;
        case OBJECT::ECLUST:
          dQuality[1] -> cd();
          break;
        case OBJECT::FLOW:
          dQuality[2] -> cd();
          break;
        case OBJECT::PART:
          dQuality[3] -> cd();
          break;
        case OBJECT::TJET:
          dQuality[4] -> cd();
          break;
        case OBJECT::RJET:
          dQuality[5] -> cd();
          break;
        case OBJECT::TCST:
          dQuality[4] -> cd();
          break;
        case OBJECT::RCST:
          dQuality[5] -> cd();
          break;
      }
      m_hNumObject[iObj] -> Write();
      for (size_t iInfo = INFO::PT; iInfo < NInfoQA; iInfo++) {
        m_hObjectQA[iObj][iInfo] -> Write();
      }
    }  // end object loop

    // save cst-specific histograms
    for (size_t iCst = CST_TYPE::TRACK_CST; iCst < NCstType; iCst++) {
      switch (iCst) {
        case CST_TYPE::TRACK_CST:
          dQuality[0] -> cd();
          break;
        case CST_TYPE::ECAL_CST:
          dQuality[1] -> cd();
          break;
        case CST_TYPE::HCAL_CST:
          dQuality[1] -> cd();
          break;
        case CST_TYPE::FLOW_CST:
          dQuality[2] -> cd();
          break;
        case CST_TYPE::PART_CST:
          dQuality[3] -> cd();
          break;
      }
      m_hNumCstAccept[iCst][0] -> Write();
      m_hNumCstAccept[iCst][1] -> Write();
      m_hSumCstEne[iCst]       -> Write();
    }  // end cst loop

    // save jet-specific histograms
    dQuality[4]     -> cd();
    m_hJetArea[0]   -> Write();
    m_hJetNumCst[0] -> Write();
    dQuality[5]     -> cd();
    m_hJetArea[1]   -> Write();
    m_hJetNumCst[1] -> Write();
  }

  // save output trees
  m_outFile -> cd();
  m_trueTree -> Write();
  m_recoTree -> Write();
  return;

}  // end 'SaveOutput()'



void SCorrelatorJetTree::ResetVariables() {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::ResetTreeVariables() Resetting tree variables..." << endl;
  }

  // reset fastjet members
  m_trueJetDef = 0x0;
  m_recoJetDef = 0x0;
  m_trueClust  = 0x0;
  m_recoClust  = 0x0;

  // reset output variables
  m_partonID[0]           = -9999;
  m_partonID[1]           = -9999;
  m_partonMom[0]          = CLHEP::Hep3Vector(-9999., -9999., -9999.);
  m_partonMom[1]          = CLHEP::Hep3Vector(-9999., -9999., -9999.);
  m_recoVtx               = CLHEP::Hep3Vector(-9999., -9999., -9999.);
  m_trueVtx               = CLHEP::Hep3Vector(-9999., -9999., -9999.);
  m_trueNumJets           = 0;
  m_trueNumChrgPars       = -9999;
  m_trueSumPar            = -9999.;
  m_truePartonID[0]       = -9999;
  m_truePartonID[1]       = -9999;
  m_truePartonMomX[0]     = -9999.;
  m_truePartonMomX[1]     = -9999.;
  m_truePartonMomY[0]     = -9999.;
  m_truePartonMomY[1]     = -9999.;
  m_truePartonMomZ[0]     = -9999.;
  m_truePartonMomZ[1]     = -9999.;
  m_recoNumJets           = 0;
  m_recoNumTrks           = -9999;
  m_recoSumECal           = -9999.;
  m_recoSumHCal           = -9999.;
  m_trueJets.clear();
  m_recoJets.clear();
  m_trueJetNCst.clear();
  m_trueJetID.clear();
  m_trueJetE.clear();
  m_trueJetPt.clear();
  m_trueJetEta.clear();
  m_trueJetPhi.clear();
  m_trueJetArea.clear();
  m_trueCstZ.clear();
  m_trueCstDr.clear();
  m_trueCstE.clear();
  m_trueCstJt.clear();
  m_trueCstEta.clear();
  m_trueCstPhi.clear();
  m_recoJetNCst.clear();
  m_recoJetID.clear();
  m_recoJetE.clear();
  m_recoJetPt.clear();
  m_recoJetEta.clear();
  m_recoJetPhi.clear();
  m_recoJetArea.clear();
  m_recoCstZ.clear();
  m_recoCstDr.clear();
  m_recoCstE.clear();
  m_recoCstJt.clear();
  m_recoCstEta.clear();
  m_recoCstPhi.clear();
  return;

}  // end 'ResetTreeVariables()



int SCorrelatorJetTree::CreateJetNode(PHCompositeNode* topNode) {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::CreateJetNode(PHCompositeNode*) Creating jet node..." << endl;
  }

  // create iterator & DST node
  PHNodeIterator   iter(topNode);
  PHCompositeNode *lowerNode = dynamic_cast<PHCompositeNode*>(iter.findFirst("PHCompositeNode", "DST"));
  if (!lowerNode) {
    lowerNode = new PHCompositeNode("DST");
    topNode   -> addNode(lowerNode);
    cout << "DST node added" << endl;
  }

  // construct jet tree name
  string baseName;
  string recoNodeName;
  string trueNodeName;
  if (m_jetTreeName.empty()) {
    baseName = "JetTree";
  } else {
    baseName = m_jetTreeName;
  }

  // can't have forward slashes in DST or else you make a subdirectory on save!!!
  string undrscr = "_";
  string nothing = "";

  // define good strings to replace bad ones
  map<string, string> forbiddenStrings;
  forbiddenStrings["/"] = undrscr;
  forbiddenStrings["("] = undrscr;
  forbiddenStrings[")"] = nothing;
  forbiddenStrings["+"] = "plus";
  forbiddenStrings["-"] = "minus";
  forbiddenStrings["*"] = "star";
  for (auto const& [badString, goodString] : forbiddenStrings) {
    size_t pos;
    while ((pos = baseName.find(badString)) != string::npos) {
      baseName.replace(pos, 1, goodString);
    }
  }

  // construct jet node name
  recoNodeName = baseName + "_RecoJets";
  trueNodeName = baseName + "_TruthJets";

  // construct jet maps
  m_recoJetMap = new JetMapv1();
  if (m_isMC && m_saveDST) {
    m_trueJetMap = new JetMapv1();
  }

  // add jet node
  if (m_saveDST) {
    PHIODataNode<PHObject>* recoJetNode = new PHIODataNode<PHObject>(m_recoJetMap, recoNodeName.c_str(), "PHObject");
    lowerNode -> addNode(recoJetNode);
    cout << recoNodeName << " node added" << endl;
  }

  // save truth DST if needed
  if(m_isMC && m_saveDST) {
    PHIODataNode<PHObject> *trueJetNode = new PHIODataNode<PHObject>(m_trueJetMap, trueNodeName.c_str(), "PHObject");
    lowerNode -> addNode(trueJetNode);
    cout << trueNodeName << " node added" << endl;
  }
  return Fun4AllReturnCodes::EVENT_OK;

}  // end 'CreateJetNode(PHCompositeNode*)'



SvtxTrackMap* SCorrelatorJetTree::GetTrackMap(PHCompositeNode *topNode) {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::GetTrackMap(PHCompositeNode*) Grabbing track map..." << endl;
  }

  // grab track map
  SvtxTrackMap *mapTrks = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
  if (!mapTrks) {
    cerr << PHWHERE
         << "PANIC: SvtxTrackMap node is missing!"
         << endl;
    assert(mapTrks);
  }
  return mapTrks;

}  // end 'GetTrackMap(PHCompositeNode*)'



GlobalVertex* SCorrelatorJetTree::GetGlobalVertex(PHCompositeNode *topNode) {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::GetGlobalVertex(PHCompositeNode*) Getting global vertex..." << endl;
  }

  // get vertex map & check if good
  GlobalVertexMap *mapVtx = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");

  const bool isVtxMapGood = (mapVtx && !(mapVtx -> empty()));
  if (!isVtxMapGood) {
    cerr << PHWHERE
         << "PANIC: GlobalVertexMap node is missing or empty!\n"
         << "       Please turn on the do_global flag in the main macro in order to reconstruct the global vertex!"
         << endl;
    assert(isVtxMapGood);
  }

  // grab vertex
  GlobalVertex *vtx = mapVtx -> begin() -> second;
  if (!vtx) {
    cerr << PHWHERE
         << "PANIC: no vertex!"
         << endl;
    assert(vtx);
  }
  return vtx;

}  // end 'GetGlobalVertex(PHCompositeNode*)'



HepMC::GenEvent* SCorrelatorJetTree::GetMcEvent(PHCompositeNode *topNode) {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::GetMcEvent(PHCompositeNode*) Grabbing mc event..." << endl;
  }

  // grab mc event map
  PHHepMCGenEventMap *mapMcEvts = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
  if (!mapMcEvts) {
    cerr << PHWHERE
         << "PANIC: HEPMC event map node is missing!"
         << endl;
    assert(mapMcEvts);
  }

  // grab mc event & check if good
  PHHepMCGenEvent *mcEvtStart = mapMcEvts -> get(1);
  if (!mcEvtStart) {
    cerr << PHWHERE
         << "PANIC: Couldn't grab start of mc events!"
         << endl;
    assert(mcEvtStart);
  }

  HepMC::GenEvent *mcEvt = mcEvtStart -> getEvent();
  if (!mcEvt) {
    cerr << PHWHERE
         << "PANIC: Couldn't grab HepMC event!"
         << endl;
    assert(mcEvt);
  }
  return mcEvt;

}  // end 'GetMcEvent(PHCompositeNode*)'



RawClusterContainer* SCorrelatorJetTree::GetClusterStore(PHCompositeNode *topNode, const TString sNodeName) {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::GetClusterStore(PHCompositeNode*, TString) Grabbing calorimeter cluster container..." << endl;
  }

  // grab clusters
  RawClusterContainer *clustStore = findNode::getClass<RawClusterContainer>(topNode, sNodeName.Data());
  if (!clustStore) {
    cout << PHWHERE
         << "PANIC: " << sNodeName.Data() << " node is missing!"
         << endl;
    assert(clustStore);
  }
  return clustStore;

}  // end 'GetClusterStore(PHCompositeNode*, TString)'



ParticleFlowElementContainer* SCorrelatorJetTree::GetFlowStore(PHCompositeNode *topNode) {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::GetFlowStore(PHCompositeNode*) Grabbing particle flow container..." << endl;
  }

  // declare pf  objects
  ParticleFlowElementContainer *flowStore = findNode::getClass<ParticleFlowElementContainer>(topNode, "ParticleFlowElements");
  if (!flowStore) {
    cerr << PHWHERE
         << "PANIC: Couldn't grab particle flow container!"
         << endl;
    assert(flowStore);
  }
  return flowStore;

}  // end 'GetFlowStore(PHCompositeNode*)'

// end ------------------------------------------------------------------------
