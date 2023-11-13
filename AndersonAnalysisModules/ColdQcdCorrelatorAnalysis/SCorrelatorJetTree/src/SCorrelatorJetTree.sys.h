// ----------------------------------------------------------------------------
// 'SCorrelatorJetTree.system.h'
// Derek Anderson
// 01.18.2023
//
// A module to produce a tree of jets for the sPHENIX
// Cold QCD Energy-Energy Correlator analysis.
//
// Derived from code by Antonio Silva (thanks!!)
// ----------------------------------------------------------------------------

#pragma once

using namespace std;
using namespace findNode;



namespace SColdQcdCorrelatorAnalysis {

  // system methods -----------------------------------------------------------

  void SCorrelatorJetTree::InitVariables() {

    // print debug statement
    if (m_doDebug) {
      cout << "SCorrelatorJetTree::InitVariables() Initializing class members..." << endl;
    }

    // initialize parton and other variables
    m_partonID[0]  = -9999;
    m_partonID[1]  = -9999;
    m_partonMom[0] = CLHEP::Hep3Vector(-9999., -9999., -9999.);
    m_partonMom[1] = CLHEP::Hep3Vector(-9999., -9999., -9999.);
    m_vecEvtsToGrab.clear();
    m_mapCstToEmbedID.clear();

    // initialize truth (inclusive) tree address members
    m_trueVtx           = CLHEP::Hep3Vector(-9999., -9999., -9999.);
    m_trueNumJets       = 0;
    m_trueNumChrgPars   = -9999;
    m_trueSumPar        = -9999.;
    m_truePartonID[0]   = -9999;
    m_truePartonID[1]   = -9999;
    m_truePartonMomX[0] = -9999.;
    m_truePartonMomX[1] = -9999.;
    m_truePartonMomY[0] = -9999.;
    m_truePartonMomY[1] = -9999.;
    m_truePartonMomZ[0] = -9999.;
    m_truePartonMomZ[1] = -9999.;
    m_trueJets.clear();
    m_trueJetNCst.clear();
    m_trueJetID.clear();
    m_trueJetE.clear();
    m_trueJetPt.clear();
    m_trueJetEta.clear();
    m_trueJetPhi.clear();
    m_trueJetArea.clear();
    m_trueCstID.clear();
    m_trueCstEmbedID.clear();
    m_trueCstZ.clear();
    m_trueCstDr.clear();
    m_trueCstE.clear();
    m_trueCstPt.clear();
    m_trueCstEta.clear();
    m_trueCstPhi.clear();

    // initialize reco tree address members
    m_recoVtx     = CLHEP::Hep3Vector(-9999., -9999., -9999.);
    m_trueVtxX    = -9999.;
    m_trueVtxY    = -9999.;
    m_trueVtxZ    = -9999.;
    m_recoNumJets = 0;
    m_recoVtxX    = -9999.;
    m_recoVtxY    = -9999.;
    m_recoVtxZ    = -9999.;
    m_recoNumTrks = -9999;
    m_recoSumECal = -9999.;
    m_recoSumHCal = -9999.;
    m_recoJets.clear();
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
    m_recoCstPt.clear();
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
    const uint64_t nNumBins(500);
    const uint64_t nEneBins(200);
    const uint64_t nPhiBins(63);
    const uint64_t nEtaBins(40);
    const uint64_t nPtBins(200);
    const uint64_t nDcaBins(10000);
    const uint64_t nQualBins(200);
    const uint64_t nDeltaBins(1000);
    const uint64_t nAreaBins(1000);
    const double   rNumBins[CONST::NRange]   = {0.,    500.};
    const double   rPhiBins[CONST::NRange]   = {-3.15, 3.15};
    const double   rEtaBins[CONST::NRange]   = {-2.,   2.};
    const double   rEneBins[CONST::NRange]   = {0.,    100.};
    const double   rPtBins[CONST::NRange]    = {0.,    100.};
    const double   rDcaBins[CONST::NRange]   = {-5.,   5.};
    const double   rQualBins[CONST::NRange]  = {0.,    20.};
    const double   rDeltaBins[CONST::NRange] = {0.,    5.};
    const double   rAreaBins[CONST::NRange]  = {0.,    10.};

    m_outFile -> cd();
    // no. of objects in acceptance
    m_hNumObject[OBJECT::TRACK]  = new TH1D("hNumTrks",     "N_{accept} (tracks)",       nNumBins, rNumBins[0], rNumBins[1]);
    m_hNumObject[OBJECT::ECLUST] = new TH1D("hNumEClust",   "N_{accept} (EMCal clust.)", nNumBins, rNumBins[0], rNumBins[1]);
    m_hNumObject[OBJECT::HCLUST] = new TH1D("hNumHClust",   "N_{accept} (HCal clust.)",  nNumBins, rNumBins[0], rNumBins[1]);
    m_hNumObject[OBJECT::FLOW]   = new TH1D("hNumFlow",     "N_{accept} (flow)",         nNumBins, rNumBins[0], rNumBins[1]);
    m_hNumObject[OBJECT::PART]   = new TH1D("hNumPart",     "N_{accept} (par.s)",        nNumBins, rNumBins[0], rNumBins[1]);
    m_hNumObject[OBJECT::TJET]   = new TH1D("hNumTruthJet", "N_{jet} (truth)",           nNumBins, rNumBins[0], rNumBins[1]);
    m_hNumObject[OBJECT::RJET]   = new TH1D("hNumRecoJets", "N_{jet} (reco.)",           nNumBins, rNumBins[0], rNumBins[1]);
    m_hNumObject[OBJECT::TCST]   = new TH1D("hNumTruthCst", "N_{cst} (truth)",           nNumBins, rNumBins[0], rNumBins[1]);
    m_hNumObject[OBJECT::RCST]   = new TH1D("hNumRecoCst",  "N_{cst} (reco.)",           nNumBins, rNumBins[0], rNumBins[1]);
    // sum of cst. energies
    m_hSumCstEne[CST_TYPE::PART_CST]  = new TH1D("hSumPartEne",  "#SigmaE (cst. par.s)", nEneBins, rEneBins[0], rEneBins[1]);
    m_hSumCstEne[CST_TYPE::FLOW_CST]  = new TH1D("hSumFlowEne",  "#SigmaE (cst. flow)",  nEneBins, rEneBins[0], rEneBins[1]);
    m_hSumCstEne[CST_TYPE::TRACK_CST] = new TH1D("hSumTrackEne", "#SigmaE (cst. track)", nEneBins, rEneBins[0], rEneBins[1]);
    m_hSumCstEne[CST_TYPE::ECAL_CST]  = new TH1D("hSumECalEne",  "#SigmaE (cst. ecal)",  nEneBins, rEneBins[0], rEneBins[1]);
    m_hSumCstEne[CST_TYPE::HCAL_CST]  = new TH1D("hSumHCalEne",  "#SigmaE (cst. hcal)",  nEneBins, rEneBins[0], rEneBins[1]);
    // particle QA
    m_hObjectQA[OBJECT::PART][INFO::PT]      = new TH1D("hPartPt",      "p_{T} (par.s)",                  nPtBins,    rPtBins[0],    rPtBins[1]);
    m_hObjectQA[OBJECT::PART][INFO::ETA]     = new TH1D("hPartEta",     "#eta (par.s)",                   nEtaBins,   rEtaBins[0],   rEtaBins[1]);
    m_hObjectQA[OBJECT::PART][INFO::PHI]     = new TH1D("hPartPhi",     "#phi (par.s)",                   nPhiBins,   rPhiBins[0],   rPhiBins[1]);
    m_hObjectQA[OBJECT::PART][INFO::ENE]     = new TH1D("hPartEne",     "E (par.s)",                      nEneBins,   rEneBins[0],   rEneBins[1]);
    m_hObjectQA[OBJECT::PART][INFO::QUAL]    = new TH1D("hPartQuality", "Quality (par.s, N/A)",           nQualBins,  rQualBins[0],  rQualBins[1]);
    m_hObjectQA[OBJECT::PART][INFO::DCAXY]   = new TH1D("hPartDcaXY",   "DCA_{xy} (par.s, N/A)",          nDcaBins,   rDcaBins[0],   rDcaBins[1]);
    m_hObjectQA[OBJECT::PART][INFO::DCAZ]    = new TH1D("hPartDcaZ",    "DCA_{z} (par.s, N/A)",           nDcaBins,   rDcaBins[0],   rDcaBins[1]);
    m_hObjectQA[OBJECT::PART][INFO::DELTAPT] = new TH1D("hPartDeltaPt", "#deltap_{T}/p_{T} (par.s, N/A)", nDeltaBins, rDeltaBins[0], rDeltaBins[1]);
    m_hObjectQA[OBJECT::PART][INFO::NTPC]    = new TH1D("hPartNumTpc",  "N_{clust}^{tpc} (par.s, N/A)",   nNumBins,   rNumBins[0],   rNumBins[1]);
    // track QA
    m_hObjectQA[OBJECT::TRACK][INFO::PT]      = new TH1D("hTrackPt",      "p_{T} (tracks)",             nPtBins,    rPtBins[0],    rPtBins[1]);
    m_hObjectQA[OBJECT::TRACK][INFO::ETA]     = new TH1D("hTrackEta",     "#eta (tracks)",              nEtaBins,   rEtaBins[0],   rEtaBins[1]);
    m_hObjectQA[OBJECT::TRACK][INFO::PHI]     = new TH1D("hTrackPhi",     "#phi (tracks)",              nPhiBins,   rPhiBins[0],   rPhiBins[1]);
    m_hObjectQA[OBJECT::TRACK][INFO::ENE]     = new TH1D("hTrackEne",     "E (tracks)",                 nEneBins,   rEneBins[0],   rEneBins[1]);
    m_hObjectQA[OBJECT::TRACK][INFO::QUAL]    = new TH1D("hTrackQuality", "Quality (tracks)",           nQualBins,  rQualBins[0],  rQualBins[1]);
    m_hObjectQA[OBJECT::TRACK][INFO::DCAXY]   = new TH1D("hTrackDcaXY",   "DCA_{xy} (tracks)",          nDcaBins,   rDcaBins[0],   rDcaBins[1]);
    m_hObjectQA[OBJECT::TRACK][INFO::DCAZ]    = new TH1D("hTrackDcaZ",    "DCA_{z} (tracks)",           nDcaBins,   rDcaBins[0],   rDcaBins[1]);
    m_hObjectQA[OBJECT::TRACK][INFO::DELTAPT] = new TH1D("hTrackDeltaPt", "#deltap_{T}/p_{T} (tracks)", nDeltaBins, rDeltaBins[0], rDeltaBins[1]);
    m_hObjectQA[OBJECT::TRACK][INFO::NTPC]    = new TH1D("hTrackNumTpc",  "N_{clust}^{tpc} (tracks)",   nNumBins,   rNumBins[0],   rNumBins[1]);
    // particle flow QA
    m_hObjectQA[OBJECT::FLOW][INFO::PT]      = new TH1D("hFlowPt",      "p_{T} (flow)",                  nPtBins,    rPtBins[0],    rPtBins[1]);
    m_hObjectQA[OBJECT::FLOW][INFO::ETA]     = new TH1D("hFlowEta",     "#eta (flow)",                   nEtaBins,   rEtaBins[0],   rEtaBins[1]);
    m_hObjectQA[OBJECT::FLOW][INFO::PHI]     = new TH1D("hFlowPhi",     "#phi (flow)",                   nPhiBins,   rPhiBins[0],   rPhiBins[1]);
    m_hObjectQA[OBJECT::FLOW][INFO::ENE]     = new TH1D("hFlowEne",     "E (flow)",                      nEneBins,   rEneBins[0],   rEneBins[1]);
    m_hObjectQA[OBJECT::FLOW][INFO::QUAL]    = new TH1D("hFlowQuality", "Quality (flow, N/A)",           nQualBins,  rQualBins[0],  rQualBins[1]);
    m_hObjectQA[OBJECT::FLOW][INFO::DCAXY]   = new TH1D("hFlowDcaXY",   "DCA_{xy} (flow, N/A)",          nDcaBins,   rDcaBins[0],   rDcaBins[1]);
    m_hObjectQA[OBJECT::FLOW][INFO::DCAZ]    = new TH1D("hFlowDcaZ",    "DCA_{z} (flow, N/A)",           nDcaBins,   rDcaBins[0],   rDcaBins[1]);
    m_hObjectQA[OBJECT::FLOW][INFO::DELTAPT] = new TH1D("hFlowDeltaPt", "#deltap_{T}/p_{T} (flow, N/A)", nDeltaBins, rDeltaBins[0], rDeltaBins[1]);
    m_hObjectQA[OBJECT::FLOW][INFO::NTPC]    = new TH1D("hFlowNumTpc",  "N_{clust}^{tpc} (flow, N/A)",   nNumBins,   rNumBins[0],   rNumBins[1]);
    // emcal cluster QA
    m_hObjectQA[OBJECT::ECLUST][INFO::PT]      = new TH1D("hEClustPt",      "p_{T} (EMCal clust.)",           nPtBins,    rPtBins[0],    rPtBins[1]);
    m_hObjectQA[OBJECT::ECLUST][INFO::ETA]     = new TH1D("hEClustEta",     "#eta (EMCal clust.)",            nEtaBins,   rEtaBins[0],   rEtaBins[1]);
    m_hObjectQA[OBJECT::ECLUST][INFO::PHI]     = new TH1D("hEClustPhi",     "#phi (EMCal clust.)",            nPhiBins,   rPhiBins[0],   rPhiBins[1]);
    m_hObjectQA[OBJECT::ECLUST][INFO::ENE]     = new TH1D("hEClustEne",     "E (EMCal clust.)",               nEneBins,   rEneBins[0],   rEneBins[1]);
    m_hObjectQA[OBJECT::ECLUST][INFO::QUAL]    = new TH1D("hEClustQuality", "Quality (EMCal, N/A)",           nQualBins,  rQualBins[0],  rQualBins[1]);
    m_hObjectQA[OBJECT::ECLUST][INFO::DCAXY]   = new TH1D("hEClustDcaXY",   "DCA_{xy} (EMCal, N/A)",          nDcaBins,   rDcaBins[0],   rDcaBins[1]);
    m_hObjectQA[OBJECT::ECLUST][INFO::DCAZ]    = new TH1D("hEClustDcaZ",    "DCA_{z} (EMCal, N/A)",           nDcaBins,   rDcaBins[0],   rDcaBins[1]);
    m_hObjectQA[OBJECT::ECLUST][INFO::DELTAPT] = new TH1D("hEClustDeltaPt", "#deltap_{T}/p_{T} (EMCal, N/A)", nDeltaBins, rDeltaBins[0], rDeltaBins[1]);
    m_hObjectQA[OBJECT::ECLUST][INFO::NTPC]    = new TH1D("hEClustNumTpc",  "N_{clust}^{tpc} (EMCal, N/A)",   nNumBins,   rNumBins[0],   rNumBins[1]);
    // hcal cluster QA
    m_hObjectQA[OBJECT::HCLUST][INFO::PT]      = new TH1D("hHClustPt",      "p_{T} (HCal clust.)",           nPtBins,    rPtBins[0],    rPtBins[1]);
    m_hObjectQA[OBJECT::HCLUST][INFO::ETA]     = new TH1D("hHClustEta",     "#eta (HCal clust.)",            nEtaBins,   rEtaBins[0],   rEtaBins[1]);
    m_hObjectQA[OBJECT::HCLUST][INFO::PHI]     = new TH1D("hHClustPhi",     "#phi (HCal clust.)",            nPhiBins,   rPhiBins[0],   rPhiBins[1]);
    m_hObjectQA[OBJECT::HCLUST][INFO::ENE]     = new TH1D("hHClustEne",     "E (HCal clust.)",               nEneBins,   rEneBins[0],   rEneBins[1]);
    m_hObjectQA[OBJECT::HCLUST][INFO::QUAL]    = new TH1D("hHClustQuality", "Quality (HCal, N/A)",           nQualBins,  rQualBins[0],  rQualBins[1]);
    m_hObjectQA[OBJECT::HCLUST][INFO::DCAXY]   = new TH1D("hHClustDcaXY",   "DCA_{xy} (HCal, N/A)",          nDcaBins,   rDcaBins[0],   rDcaBins[1]);
    m_hObjectQA[OBJECT::HCLUST][INFO::DCAZ]    = new TH1D("hHClustDcaZ",    "DCA_{z} (HCal, N/A)",           nDcaBins,   rDcaBins[0],   rDcaBins[1]);
    m_hObjectQA[OBJECT::HCLUST][INFO::DELTAPT] = new TH1D("hHClustDeltaPt", "#deltap_{T}/p_{T} (HCal, N/A)", nDeltaBins, rDeltaBins[0], rDeltaBins[1]);
    m_hObjectQA[OBJECT::HCLUST][INFO::NTPC]    = new TH1D("hHClustNumTpc",  "N_{clust}^{tpc} (HCal, N/A)",   nNumBins,   rNumBins[0],   rNumBins[1]);
    // truth jet QA
    m_hObjectQA[OBJECT::TJET][INFO::PT]      = new TH1D("hTruthJetPt",      "p_{T} (truth jet)",                  nPtBins,    rPtBins[0],    rPtBins[1]);
    m_hObjectQA[OBJECT::TJET][INFO::ETA]     = new TH1D("hTruthJetEta",     "#eta (truth jet)",                   nEtaBins,   rEtaBins[0],   rEtaBins[1]);
    m_hObjectQA[OBJECT::TJET][INFO::PHI]     = new TH1D("hTruthJetPhi",     "#phi (truth jet)",                   nPhiBins,   rPhiBins[0],   rPhiBins[1]);
    m_hObjectQA[OBJECT::TJET][INFO::ENE]     = new TH1D("hTruthJetEne",     "E (truth jet)",                      nEneBins,   rEneBins[0],   rEneBins[1]);
    m_hObjectQA[OBJECT::TJET][INFO::QUAL]    = new TH1D("hTruthJetQuality", "Quality (truth jet, N/A)",           nQualBins,  rQualBins[0],  rQualBins[1]);
    m_hObjectQA[OBJECT::TJET][INFO::DCAXY]   = new TH1D("hTruthJetDcaXY",   "DCA_{xy} (truth jet, N/A)",          nDcaBins,   rDcaBins[0],   rDcaBins[1]);
    m_hObjectQA[OBJECT::TJET][INFO::DCAZ]    = new TH1D("hTruthJetDcaZ",    "DCA_{z} (truth jet, N/A)",           nDcaBins,   rDcaBins[0],   rDcaBins[1]);
    m_hObjectQA[OBJECT::TJET][INFO::DELTAPT] = new TH1D("hTruthJetDeltaPt", "#deltap_{T}/p_{T} (truth jet, N/A)", nDeltaBins, rDeltaBins[0], rDeltaBins[1]);
    m_hObjectQA[OBJECT::TJET][INFO::NTPC]    = new TH1D("hTruthJetNumTpc",  "N_{clust}^{tpc} (truth jet, N/A)",   nNumBins,   rNumBins[0],   rNumBins[1]);
    m_hJetArea[0]                            = new TH1D("hTruthJetArea",    "Area (truth jet)",                   nAreaBins,  rAreaBins[0],  rAreaBins[1]);
    m_hJetNumCst[0]                          = new TH1D("hTruthJetNCst",    "N_{cst} (truth jet)",                nNumBins,   rNumBins[0],   rNumBins[1]);
    // reco jet QA
    m_hObjectQA[OBJECT::RJET][INFO::PT]      = new TH1D("hRecoJetPt",      "p_{T} (reco. jet)",                  nPtBins,    rPtBins[0],    rPtBins[1]);
    m_hObjectQA[OBJECT::RJET][INFO::ETA]     = new TH1D("hRecoJetEta",     "#eta (reco. jet)",                   nEtaBins,   rEtaBins[0],   rEtaBins[1]);
    m_hObjectQA[OBJECT::RJET][INFO::PHI]     = new TH1D("hRecoJetPhi",     "#phi (reco. jet)",                   nPhiBins,   rPhiBins[0],   rPhiBins[1]);
    m_hObjectQA[OBJECT::RJET][INFO::ENE]     = new TH1D("hRecoJetEne",     "E (reco. jet)",                      nEneBins,   rEneBins[0],   rEneBins[1]);
    m_hObjectQA[OBJECT::RJET][INFO::QUAL]    = new TH1D("hRecoJetQuality", "Quality (reco. jet, N/A)",           nQualBins,  rQualBins[0],  rQualBins[1]);
    m_hObjectQA[OBJECT::RJET][INFO::DCAXY]   = new TH1D("hRecoJetDcaXY",   "DCA_{xy} (reco. jet, N/A)",          nDcaBins,   rDcaBins[0],   rDcaBins[1]);
    m_hObjectQA[OBJECT::RJET][INFO::DCAZ]    = new TH1D("hRecoJetDcaZ",    "DCA_{z} (reco. jet, N/A)",           nDcaBins,   rDcaBins[0],   rDcaBins[1]);
    m_hObjectQA[OBJECT::RJET][INFO::DELTAPT] = new TH1D("hRecoJetDeltaPt", "#deltap_{T}/p_{T} (reco. jet, N/A)", nDeltaBins, rDeltaBins[0], rDeltaBins[1]);
    m_hObjectQA[OBJECT::RJET][INFO::NTPC]    = new TH1D("hRecoJetNumTpc",  "N_{clust}^{tpc} (reco. jet, N/A)",   nNumBins,   rNumBins[0],   rNumBins[1]);
    m_hJetArea[1]                            = new TH1D("hRecoJetArea",    "Area (reco. jet)",                   nAreaBins,  rAreaBins[0],  rAreaBins[1]);
    m_hJetNumCst[1]                          = new TH1D("hRecoJetNCst",    "N_{cst} (reco. jet)",                nNumBins,   rNumBins[0],   rNumBins[1]);
    // truth cst. QA
    m_hObjectQA[OBJECT::TCST][INFO::PT]      = new TH1D("hTruthCstPt",      "p_{T} (truth cst.)",                  nPtBins,    rPtBins[0],    rPtBins[1]);
    m_hObjectQA[OBJECT::TCST][INFO::ETA]     = new TH1D("hTruthCstEta",     "#eta (truth cst.)",                   nEtaBins,   rEtaBins[0],   rEtaBins[1]);
    m_hObjectQA[OBJECT::TCST][INFO::PHI]     = new TH1D("hTruthCstPhi",     "#phi (truth cst.)",                   nPhiBins,   rPhiBins[0],   rPhiBins[1]);
    m_hObjectQA[OBJECT::TCST][INFO::ENE]     = new TH1D("hTruthCstEne",     "E (truth cst.)",                      nEneBins,   rEneBins[0],   rEneBins[1]);
    m_hObjectQA[OBJECT::TCST][INFO::QUAL]    = new TH1D("hTruthCstQuality", "Quality (truth cst., N/A)",           nQualBins,  rQualBins[0],  rQualBins[1]);
    m_hObjectQA[OBJECT::TCST][INFO::DCAXY]   = new TH1D("hTruthCstDcaXY",   "DCA_{xy} (truth cst., N/A)",          nDcaBins,   rDcaBins[0],   rDcaBins[1]);
    m_hObjectQA[OBJECT::TCST][INFO::DCAZ]    = new TH1D("hTruthCstDcaZ",    "DCA_{z} (truth cst., N/A)",           nDcaBins,   rDcaBins[0],   rDcaBins[1]);
    m_hObjectQA[OBJECT::TCST][INFO::DELTAPT] = new TH1D("hTruthCstDeltaPt", "#deltap_{T}/p_{T} (truth cst., N/A)", nDeltaBins, rDeltaBins[0], rDeltaBins[1]);
    m_hObjectQA[OBJECT::TCST][INFO::NTPC]    = new TH1D("hTruthCstNumTpc",  "N_{clust}^{tpc} (truth cst., N/A)",   nNumBins,   rNumBins[0],   rNumBins[1]);
    // reco cst. QA
    m_hObjectQA[OBJECT::RCST][INFO::PT]      = new TH1D("hRecoCstPt",      "p_{T} (reco. cst.)",                  nPtBins,    rPtBins[0],    rPtBins[1]);
    m_hObjectQA[OBJECT::RCST][INFO::ETA]     = new TH1D("hRecoCstEta",     "#eta (reco. cst.)",                   nEtaBins,   rEtaBins[0],   rEtaBins[1]);
    m_hObjectQA[OBJECT::RCST][INFO::PHI]     = new TH1D("hRecoCstPhi",     "#phi (reco. cst.)",                   nPhiBins,   rPhiBins[0],   rPhiBins[1]);
    m_hObjectQA[OBJECT::RCST][INFO::ENE]     = new TH1D("hRecoCstEne",     "E (reco. cst.)",                      nEneBins,   rEneBins[0],   rEneBins[1]);
    m_hObjectQA[OBJECT::RCST][INFO::QUAL]    = new TH1D("hRecoCstQuality", "Quality (reco. cst., N/A)",           nQualBins,  rQualBins[0],  rQualBins[1]);
    m_hObjectQA[OBJECT::RCST][INFO::DCAXY]   = new TH1D("hRecoCstDcaXY",   "DCA_{xy} (reco. cst., N/A)",          nDcaBins,   rDcaBins[0],   rDcaBins[1]);
    m_hObjectQA[OBJECT::RCST][INFO::DCAZ]    = new TH1D("hRecoCstDcaZ",    "DCA_{z} (reco. cst., N/A)",           nDcaBins,   rDcaBins[0],   rDcaBins[1]);
    m_hObjectQA[OBJECT::RCST][INFO::DELTAPT] = new TH1D("hRecoCstDeltaPt", "#deltap_{T}/p_{T} (reco. cst., N/A)", nDeltaBins, rDeltaBins[0], rDeltaBins[1]);
    m_hObjectQA[OBJECT::RCST][INFO::NTPC]    = new TH1D("hRecoCstNumTpc",  "N_{clust}^{tpc} (reco. cst., N/A)",   nNumBins,   rNumBins[0],   rNumBins[1]);
    // no. of cst.s
    m_hNumCstAccept[CST_TYPE::PART_CST][0]  = new TH1D("hNumPartCstTot", "N_{cst}^{par} total",      nNumBins, rNumBins[0], rNumBins[1]);
    m_hNumCstAccept[CST_TYPE::PART_CST][1]  = new TH1D("hNumPartCstAcc", "N_{cst}^{par} accepted",   nNumBins, rNumBins[0], rNumBins[1]);
    m_hNumCstAccept[CST_TYPE::TRACK_CST][0] = new TH1D("hNumTrkCstTot",  "N_{cst}^{trk} total",      nNumBins, rNumBins[0], rNumBins[1]);
    m_hNumCstAccept[CST_TYPE::TRACK_CST][1] = new TH1D("hNumTrkCstAcc",  "N_{cst}^{trk} accepted",   nNumBins, rNumBins[0], rNumBins[1]);
    m_hNumCstAccept[CST_TYPE::FLOW_CST][0]  = new TH1D("hNumFlowCstTot", "N_{cst}^{flow} total",     nNumBins, rNumBins[0], rNumBins[1]);
    m_hNumCstAccept[CST_TYPE::FLOW_CST][1]  = new TH1D("hNumFlowCstAcc", "N_{cst}^{flow} accepted",  nNumBins, rNumBins[0], rNumBins[1]);
    m_hNumCstAccept[CST_TYPE::ECAL_CST][0]  = new TH1D("hNumECalCstTot", "N_{cst}^{clust} total",    nNumBins, rNumBins[0], rNumBins[1]);
    m_hNumCstAccept[CST_TYPE::ECAL_CST][1]  = new TH1D("hNumECalCstAcc", "N_{cst}^{clust} accepted", nNumBins, rNumBins[0], rNumBins[1]);
    m_hNumCstAccept[CST_TYPE::HCAL_CST][0]  = new TH1D("hNumHCalCstTot", "N_{cst}^{clust} total",    nNumBins, rNumBins[0], rNumBins[1]);
    m_hNumCstAccept[CST_TYPE::HCAL_CST][1]  = new TH1D("hNumHCalCstAcc", "N_{cst}^{clust} accepted", nNumBins, rNumBins[0], rNumBins[1]);

    // set errors
    for (size_t iObj = OBJECT::TRACK; iObj < CONST::NObjType; iObj++) {
      m_hNumObject[iObj] -> Sumw2();
      for (size_t iInfo = INFO::PT; iInfo < CONST::NInfoQA; iInfo++) {
        m_hObjectQA[iObj][iInfo] -> Sumw2();
      }  // end info loop
    }  // end object loop
    for (size_t iCst = CST_TYPE::TRACK_CST; iCst < CONST::NCstType; iCst++) {
      m_hNumCstAccept[iCst][0] -> Sumw2();
      m_hNumCstAccept[iCst][1] -> Sumw2();
      m_hSumCstEne[iCst]       -> Sumw2();
    }  // end cst loop
    m_hJetArea[0]   -> Sumw2();
    m_hJetNumCst[0] -> Sumw2();
    m_hJetArea[1]   -> Sumw2();
    m_hJetNumCst[1] -> Sumw2();
    return;

  }  // end 'InitHists()'



  void SCorrelatorJetTree::InitFuncs() {

    // print debug statement
    if (m_doDebug) {
      cout << "SCorrelatorJetTree::InitFuncs() Initializing functions for internal calculations..." << endl;
    }

    // pt range of functions
    const pair<float, float> ptRange = {0., 100.};

    // initialize functions
    m_fSigDcaXY = new TF1("fSigmaDcaXY", "[0]+[1]/x+[2]/(x*x)", ptRange.first, ptRange.second);
    m_fSigDcaZ  = new TF1("fSigmaDcaZ",  "[0]+[1]/x+[2]/(x*x)", ptRange.first, ptRange.second);
    for (uint8_t iParam = 0; iParam < CONST::NParam; iParam++) {
      m_fSigDcaXY -> SetParameter(iParam, m_parSigDcaXY[iParam]);
      m_fSigDcaZ  -> SetParameter(iParam, m_parSigDcaZ[iParam]);
    }
    return;

  }  // end 'InitFuncs()'



  void SCorrelatorJetTree::InitTuples() {

    // print debug statement
    if (m_doDebug) {
      cout << "SCorrelatorJetTree::InitTuples() Initializing output tuples..." << endl;
    }

    // track QA leaves
    const vector<string> vecTrkQALeaves = {
      "pt",
      "eta",
      "phi",
      "energy",
      "dcaxy",
      "dcaz",
      "deltapt",
      "quality",
      "nmvtxlayer",
      "ninttlayer",
      "ntpclayer",
      "vtxx",
      "vtxy",
      "vtxz"
    };

    // flatten leaf list
    string argTrkQALeaves("");
    for (size_t iLeaf = 0; iLeaf < vecTrkQALeaves.size(); iLeaf++) {
      argTrkQALeaves.append(vecTrkQALeaves[iLeaf]);
      if ((iLeaf + 1) != vecTrkQALeaves.size()) {
        argTrkQALeaves.append(":");
      }
    }
    m_ntTrkQA = new TNtuple("ntTrkQA", "Track QA", argTrkQALeaves.data());

    // leaves for weird track check
    const vector<string> vecWeirdTrkLeaves = {
      "trkid_a",
      "trkid_b",
      "pt_a",
      "pt_b",
      "eta_a",
      "eta_b",
      "phi_a",
      "phi_b",
      "ene_a",
      "ene_b",
      "dcaxy_a",
      "dcaxy_b",
      "dcaz_a",
      "dcaz_b",
      "vtxx_a",
      "vtxx_b",
      "vtxy_a",
      "vtxy_b",
      "vtxz_a",
      "vtxz_b",
      "quality_a",
      "quality_b",
      "deltapt_a",
      "deltapt_b",
      "nmvtxlayers_a",
      "nmvtxlayers_b",
      "ninttlayers_a",
      "ninttlayers_b",
      "ntpclayers_a",
      "ntpclayers_b",
      "nmvtxclusts_a",
      "nmvtxclusts_b",
      "ninttclusts_a",
      "ninttclusts_b",
      "ntpcclusts_a",
      "ntpcclusts_b",
      "nclustkey_a",
      "nclustkey_b",
      "nsameclustkey",
      "deltartrack"
    };

    string argWeirdTrkLeaves("");
    for (size_t iLeaf = 0; iLeaf < vecWeirdTrkLeaves.size(); iLeaf++) {
      argWeirdTrkLeaves.append(vecWeirdTrkLeaves[iLeaf]);
      if ((iLeaf + 1) != vecWeirdTrkLeaves.size()) {
        argWeirdTrkLeaves.append(":");
      }
    }
    m_ntWeirdTracks = new TNtuple("ntWeirdTracks", "Weird Tracks",   argWeirdTrkLeaves.data());
    return;

  }  // end 'InitTuples()'



  void SCorrelatorJetTree::InitTrees() {

    // print debug statement
    if (m_doDebug) {
      cout << "SCorrelatorJetTree::InitTrees() Initializing output trees..." << endl;
    }

    // initialize true (inclusive) jet tree
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
    m_trueTree -> Branch("CstEmbedID",     &m_trueCstEmbedID);
    m_trueTree -> Branch("CstZ",           &m_trueCstZ);
    m_trueTree -> Branch("CstDr",          &m_trueCstDr);
    m_trueTree -> Branch("CstEnergy",      &m_trueCstE);
    m_trueTree -> Branch("CstPt",          &m_trueCstPt);
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
    m_recoTree -> Branch("CstPt",         &m_recoCstPt);
    m_recoTree -> Branch("CstEta",        &m_recoCstEta);
    m_recoTree -> Branch("CstPhi",        &m_recoCstPhi);
    return;

  }  // end 'InitTrees()'



  void SCorrelatorJetTree::InitEvals(PHCompositeNode* topNode) {

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
    m_trueCstEmbedID.clear();
    m_trueCstZ.clear();
    m_trueCstDr.clear();
    m_trueCstE.clear();
    m_trueCstPt.clear();
    m_trueCstEta.clear();
    m_trueCstPhi.clear();

    // declare vectors to storing constituents
    vector<int>    vecTruCstID;
    vector<int>    vecTruCstEmbedID;
    vector<double> vecTruCstZ;
    vector<double> vecTruCstDr;
    vector<double> vecTruCstE;
    vector<double> vecTruCstPt;
    vector<double> vecTruCstEta;
    vector<double> vecTruCstPhi;
    vecTruCstID.clear();
    vecTruCstZ.clear();
    vecTruCstDr.clear();
    vecTruCstE.clear();
    vecTruCstPt.clear();
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
      vecTruCstEmbedID.clear();
      vecTruCstZ.clear();
      vecTruCstDr.clear();
      vecTruCstE.clear();
      vecTruCstPt.clear();
      vecTruCstEta.clear();
      vecTruCstPhi.clear();

      // loop over constituents
      vector<fastjet::PseudoJet> trueCsts = m_trueJets[iTruJet].constituents();
      for (unsigned int iTruCst = 0; iTruCst < trueCsts.size(); ++iTruCst) {

        // get constituent info
        const double cstPhi = trueCsts[iTruCst].phi_std();
        const double cstEta = trueCsts[iTruCst].pseudorapidity();
        const double cstE   = trueCsts[iTruCst].E();
        const double cstPt  = trueCsts[iTruCst].perp();
        const double cstPx  = trueCsts[iTruCst].px();
        const double cstPy  = trueCsts[iTruCst].py();
        const double cstPz  = trueCsts[iTruCst].pz();
        const double cstP   = ((cstPx * cstPx) + (cstPy * cstPy) + (cstPz * cstPz));
        const double cstZ   = cstP / jetP;
        const double cstDf  = cstPhi - jetPhi;
        const double cstDh  = cstEta - jetEta;
        const double cstDr  = sqrt((cstDf * cstDf) + (cstDh * cstDh));

        // get barcode and embedding ID
        const int cstID   = trueCsts[iTruCst].user_index();
        const int embedID = m_mapCstToEmbedID[cstID];

        // add csts to vectors
        vecTruCstID.push_back(abs(cstID));
        vecTruCstEmbedID.push_back(embedID);
        vecTruCstZ.push_back(cstZ);
        vecTruCstDr.push_back(cstDr);
        vecTruCstE.push_back(cstE);
        vecTruCstPt.push_back(cstPt);
        vecTruCstEta.push_back(cstEta);
        vecTruCstPhi.push_back(cstPhi);

        // fill QA histograms and increment counters
        m_hObjectQA[OBJECT::TCST][INFO::PT]  -> Fill(cstPt);
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
      m_trueCstEmbedID.push_back(vecTruCstEmbedID);
      m_trueCstZ.push_back(vecTruCstZ);
      m_trueCstDr.push_back(vecTruCstDr);
      m_trueCstE.push_back(vecTruCstE);
      m_trueCstPt.push_back(vecTruCstPt);
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
    m_recoCstPt.clear();
    m_recoCstEta.clear();
    m_recoCstPhi.clear();

    // declare vectors for storing constituents
    vector<int>    vecRecCstMatchID;
    vector<double> vecRecCstZ;
    vector<double> vecRecCstDr;
    vector<double> vecRecCstE;
    vector<double> vecRecCstPt;
    vector<double> vecRecCstEta;
    vector<double> vecRecCstPhi;
    vecRecCstMatchID.clear();
    vecRecCstZ.clear();
    vecRecCstDr.clear();
    vecRecCstE.clear();
    vecRecCstPt.clear();
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
      vecRecCstPt.clear();
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
        const double cstPt      = recoCsts[iCst].perp();
        const double cstPx      = recoCsts[iCst].px();
        const double cstPy      = recoCsts[iCst].py();
        const double cstPz      = recoCsts[iCst].pz();
        const double cstP       = ((cstPx * cstPx) + (cstPy * cstPy) + (cstPz * cstPz));
        const double cstZ       = cstP / jetP;
        const double cstDf      = cstPhi - jetPhi;
        const double cstDh      = cstEta - jetEta;
        const double cstDr      = sqrt((cstDf * cstDf) + (cstDh * cstDh));

        // add csts to vectors
        vecRecCstMatchID.push_back(cstMatchID);
        vecRecCstZ.push_back(cstZ);
        vecRecCstDr.push_back(cstDr);
        vecRecCstE.push_back(cstE);
        vecRecCstPt.push_back(cstPt);
        vecRecCstEta.push_back(cstEta);
        vecRecCstPhi.push_back(cstPhi);

        // fill QA histograms and increment counters
        m_hObjectQA[OBJECT::RCST][INFO::PT]  -> Fill(cstPt);
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
      m_recoCstPt.push_back(vecRecCstPt);
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
    const string sQuality[CONST::NDirectory + 1] = {"Tracks", "CaloClusters", "ParticleFlow", "Particles", "TruthJets", "RecoJets", "QA"};
    TDirectory*  dQuality[CONST::NDirectory + 1];
    if (m_doQualityPlots) {

      // create QA directories
      dQuality[CONST::NDirectory] = (TDirectory*) m_outFile -> mkdir(sQuality[CONST::NDirectory].data());
      for (size_t iDirect = 0; iDirect < CONST::NDirectory; iDirect++) {
        dQuality[iDirect] = (TDirectory*) dQuality[CONST::NDirectory] -> mkdir(sQuality[iDirect].data());
      }

      // save object-specific QA hists
      for (size_t iObj = OBJECT::TRACK; iObj < CONST::NObjType; iObj++) {
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
        for (size_t iInfo = INFO::PT; iInfo < CONST::NInfoQA; iInfo++) {
          m_hObjectQA[iObj][iInfo] -> Write();
        }
      }  // end object loop

      // save cst-specific histograms
      for (size_t iCst = CST_TYPE::TRACK_CST; iCst < CONST::NCstType; iCst++) {
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

    // save QA tuples
    dQuality[0] -> cd();
    m_ntTrkQA   -> Write();
    if (m_checkWeirdTrks) {
      m_ntWeirdTracks -> Write();
    }

    // save output trees
    m_outFile  -> cd();
    m_recoTree -> Write();
    if (m_isMC) {
      m_trueTree -> Write();
    }
    return;

  }  // end 'SaveOutput()'



  void SCorrelatorJetTree::ResetVariables() {

    // print debug statement
    if (m_doDebug) {
      cout << "SCorrelatorJetTree::ResetTreeVariables() Resetting tree variables..." << endl;
    }

    // reset fastjet members
    m_trueJetDef = NULL;
    m_recoJetDef = NULL;
    m_trueClust  = NULL;
    m_recoClust  = NULL;

    // reset parton and other variables
    m_partonID[0]  = -9999;
    m_partonID[1]  = -9999;
    m_partonMom[0] = CLHEP::Hep3Vector(-9999., -9999., -9999.);
    m_partonMom[1] = CLHEP::Hep3Vector(-9999., -9999., -9999.);
    m_vecEvtsToGrab.clear();
    m_mapCstToEmbedID.clear();

    // reset truth (inclusive) tree variables
    m_trueVtx           = CLHEP::Hep3Vector(-9999., -9999., -9999.);
    m_trueNumJets       = 0;
    m_trueNumChrgPars   = -9999;
    m_trueSumPar        = -9999.;
    m_truePartonID[0]   = -9999;
    m_truePartonID[1]   = -9999;
    m_truePartonMomX[0] = -9999.;
    m_truePartonMomX[1] = -9999.;
    m_truePartonMomY[0] = -9999.;
    m_truePartonMomY[1] = -9999.;
    m_truePartonMomZ[0] = -9999.;
    m_truePartonMomZ[1] = -9999.;
    m_trueJets.clear();
    m_trueJetNCst.clear();
    m_trueJetID.clear();
    m_trueJetE.clear();
    m_trueJetPt.clear();
    m_trueJetEta.clear();    
    m_trueJetPhi.clear();
    m_trueJetArea.clear();
    m_trueCstID.clear();
    m_trueCstEmbedID.clear();
    m_trueCstZ.clear();
    m_trueCstDr.clear();
    m_trueCstE.clear();
    m_trueCstPt.clear();
    m_trueCstEta.clear();
    m_trueCstPhi.clear();

    // reset reco tree variables
    m_recoVtx     = CLHEP::Hep3Vector(-9999., -9999., -9999.);
    m_recoNumJets = 0;
    m_recoNumTrks = -9999;
    m_recoSumECal = -9999.;
    m_recoSumHCal = -9999.;
    m_recoJets.clear();
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
    m_recoCstPt.clear();
    m_recoCstEta.clear();
    m_recoCstPhi.clear();
    return;

  }  // end 'ResetTreeVariables()



  void SCorrelatorJetTree::DetermineEvtsToGrab(PHCompositeNode* topNode) {

    // print debug statement
    if (m_doDebug) {
      cout << "SCorrelatorJetTree::DetermineEvtsToGrab() Determining which subevents to grab..." << endl;
    }

    // make sure vector is clear
    m_vecEvtsToGrab.clear();

    // if not embedding, grab signal event
    // otherwise add all subevents
    if (!m_isEmbed) {
      m_vecEvtsToGrab.push_back(1);
    } else {
      PHHepMCGenEventMap* mapMcEvts = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
      for (PHHepMCGenEventMap::ConstIter itEvt = mapMcEvts -> begin(); itEvt != mapMcEvts -> end(); ++itEvt) {
        m_vecEvtsToGrab.push_back(itEvt -> second -> get_embedding_id());
      }
    }
    return;

  }  // end 'DetermineEvtsToGrab()'



  int SCorrelatorJetTree::CreateJetNode(PHCompositeNode* topNode) {

    // print debug statement
    if (m_doDebug) {
      cout << "SCorrelatorJetTree::CreateJetNode(PHCompositeNode*) Creating jet node..." << endl;
    }

    // create iterator & DST node
    PHNodeIterator   iter(topNode);
    PHCompositeNode* lowerNode = dynamic_cast<PHCompositeNode*>(iter.findFirst("PHCompositeNode", "DST"));
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
    if  (m_isMC && m_saveDST) {
     m_trueJetMap = new JetMapv1();
    }

    // add jet node
    if (m_saveDST) {
      PHIODataNode<PHObject>* recoJetNode = new PHIODataNode<PHObject>(m_recoJetMap, recoNodeName.c_str(), "PHObject");
      lowerNode -> addNode(recoJetNode);
      cout << recoNodeName << " node added" << endl;
    }

    // save truth DSTs if needed
    if(m_isMC && m_saveDST) {
      PHIODataNode<PHObject> *trueJetNode = new PHIODataNode<PHObject>(m_trueJetMap, trueNodeName.c_str(), "PHObject");
      lowerNode -> addNode(trueJetNode);
      cout << trueNodeName << " node added" << endl;
    }
    return Fun4AllReturnCodes::EVENT_OK;

  }  // end 'CreateJetNode(PHCompositeNode*)'



  int SCorrelatorJetTree::GetEmbedID(PHCompositeNode* topNode, const int iEvtToGrab) {

    // print debug statement
    if (m_doDebug) {
      cout << "SCorrelatorJetTree::GetEmbedID(PHCompositeNode*) Grabbing embedding ID from MC subevent #" << iEvtToGrab << "..." << endl;
    }

    // grab mc event map
    PHHepMCGenEventMap* mapMcEvts = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
    if (!mapMcEvts) {
      cerr << PHWHERE
           << "PANIC: HEPMC event map node is missing!"
           << endl;
      assert(mapMcEvts);
    }

    // grab mc event & return embedding id
    PHHepMCGenEvent* mcEvtStart = mapMcEvts -> get(iEvtToGrab);
    if (!mcEvtStart) {
      cerr << PHWHERE
           << "PANIC: Couldn't grab start of mc events!"
           << endl;
      assert(mcEvtStart);
    }
    return mcEvtStart -> get_embedding_id();

  }  // end 'GetEmbedID(PHCompositeNode*, int)'



  SvtxTrackMap* SCorrelatorJetTree::GetTrackMap(PHCompositeNode* topNode) {

    // print debug statement
    if (m_doDebug) {
      cout << "SCorrelatorJetTree::GetTrackMap(PHCompositeNode*) Grabbing track map..." << endl;
    }

    // grab track map
    SvtxTrackMap* mapTrks = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
    if (!mapTrks) {
      cerr << PHWHERE
           << "PANIC: SvtxTrackMap node is missing!"
           << endl;
      assert(mapTrks);
    }
    return mapTrks;

  }  // end 'GetTrackMap(PHCompositeNode*)'



  GlobalVertex* SCorrelatorJetTree::GetGlobalVertex(PHCompositeNode* topNode, const int iVtxToGrab) {

    // print debug statement
    if (m_doDebug) {
      cout << "SCorrelatorJetTree::GetGlobalVertex(PHCompositeNode*) Getting global vertex..." << endl;
    }

    // get vertex map
    GlobalVertexMap* mapVtx = GetVertexMap(topNode);

    // get specified vertex
    GlobalVertex* vtx = NULL;
    if (iVtxToGrab < 0) {
      vtx = mapVtx -> begin() -> second;
    } else {
      vtx = mapVtx -> get(iVtxToGrab);
    }

    // check if good
    if (!vtx) {
      cerr << PHWHERE
           << "PANIC: no vertex!"
           << endl;
      assert(vtx);
    }
    return vtx;

  }  // end 'GetGlobalVertex(PHCompositeNode*, int)'



  GlobalVertexMap* SCorrelatorJetTree::GetVertexMap(PHCompositeNode* topNode) {

    // print debug statement
    if (m_doDebug) {
      cout << "SCorrelatorJetTree::GetVertexMap(PHCompositeNode*) Getting global vertex map..." << endl;
    }

    // get vertex map
    GlobalVertexMap* mapVtx = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");

    // check if good
    const bool isVtxMapGood = (mapVtx && !(mapVtx -> empty()));
    if (!isVtxMapGood) {
      cerr << PHWHERE
           << "PANIC: GlobalVertexMap node is missing or empty!\n"
           << "       Please turn on the do_global flag in the main macro in order to reconstruct the global vertex!"
           << endl;
      assert(isVtxMapGood);
    }
    return mapVtx;

  }  // end 'GetVertexMap(PHCompositeNode*)'
  


  HepMC::GenEvent* SCorrelatorJetTree::GetMcEvent(PHCompositeNode* topNode, const int iEvtToGrab) {

    // print debug statement
    if (m_doDebug) {
      cout << "SCorrelatorJetTree::GetMcEvent(PHCompositeNode*, int) Grabbing mc subevent #" << iEvtToGrab << "..." << endl;
    }

    // grab mc event map
    PHHepMCGenEventMap* mapMcEvts = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
    if (!mapMcEvts) {
      cerr << PHWHERE
           << "PANIC: HEPMC event map node is missing!"
           << endl;
      assert(mapMcEvts);
    }

    // grab mc event & check if good
    PHHepMCGenEvent* mcEvtStart = mapMcEvts -> get(iEvtToGrab);
    if (!mcEvtStart) {
      cerr << PHWHERE
           << "PANIC: Couldn't grab start of mc events!"
           << endl;
      assert(mcEvtStart);
    }

    HepMC::GenEvent* mcEvt = mcEvtStart -> getEvent();
    if (!mcEvt) {
      cerr << PHWHERE
           << "PANIC: Couldn't grab HepMC event!"
           << endl;
      assert(mcEvt);
    }
    return mcEvt;

  }  // end 'GetMcEvent(PHCompositeNode*, int)'



  RawClusterContainer* SCorrelatorJetTree::GetClusterStore(PHCompositeNode* topNode, const TString sNodeName) {

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



  ParticleFlowElementContainer* SCorrelatorJetTree::GetFlowStore(PHCompositeNode* topNode) {

    // print debug statement
    if (m_doDebug) {
      cout << "SCorrelatorJetTree::GetFlowStore(PHCompositeNode*) Grabbing particle flow container..." << endl;
    }

    // declare pf  objects
    ParticleFlowElementContainer* flowStore = findNode::getClass<ParticleFlowElementContainer>(topNode, "ParticleFlowElements");
    if (!flowStore) {
      cerr << PHWHERE
           << "PANIC: Couldn't grab particle flow container!"
           << endl;
      assert(flowStore);
    }
    return flowStore;

  }  // end 'GetFlowStore(PHCompositeNode*)'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------
