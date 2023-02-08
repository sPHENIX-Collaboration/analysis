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

#pragma once

using namespace std;
using namespace findNode;



void SCorrelatorJetTree::initializeVariables() {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::initializeVariables() Initializing class members..." << endl;
  }

  // initialize class members as needed
  m_hm                   = nullptr;
  m_particleflow_mineta  = -1.1;
  m_particleflow_maxeta  = 1.1;
  m_track_minpt          = 0.;
  m_track_maxpt          = 9999.;
  m_track_mineta         = -1.1;
  m_track_maxeta         = 1.1;
  m_EMCal_cluster_minpt  = 0.;
  m_EMCal_cluster_maxpt  = 9999.;
  m_EMCal_cluster_mineta = -1.1;
  m_EMCal_cluster_maxeta = 1.1;
  m_HCal_cluster_minpt   = 0.;
  m_HCal_cluster_maxpt   = 9999.;
  m_HCal_cluster_mineta  = -1.1;
  m_HCal_cluster_maxeta  = 1.1;
  m_MC_particle_minpt    = 0.;
  m_MC_particle_maxpt    = 9999.;
  m_MC_particle_mineta   = -1.1;
  m_MC_particle_maxeta   = 1.1;
  m_add_particleflow     = true;
  m_add_tracks           = false;
  m_add_EMCal_clusters   = false;
  m_add_HCal_clusters    = false;
  m_jetr                 = 0.4;
  m_jetalgo              = antikt_algorithm;
  m_recomb_scheme        = pt_scheme;
  m_doQualityPlots       = true;
  m_save_dst             = false;
  m_recNumJets           = 0;
  m_recPartonID[0]       = -9999;
  m_recPartonID[1]       = -9999;
  m_recPartonMomX[0]     = -9999.;
  m_recPartonMomX[1]     = -9999.;
  m_recPartonMomY[0]     = -9999.;
  m_recPartonMomY[1]     = -9999.;
  m_recPartonMomZ[0]     = -9999.;
  m_recPartonMomZ[1]     = -9999.;
  m_truNumJets           = 0;
  m_truPartonID[0]       = -9999;
  m_truPartonID[1]       = -9999;
  m_truPartonMomX[0]     = -9999.;
  m_truPartonMomX[1]     = -9999.;
  m_truPartonMomY[0]     = -9999.;
  m_truPartonMomY[1]     = -9999.;
  m_truPartonMomZ[0]     = -9999.;
  m_truPartonMomZ[1]     = -9999.;
  m_recJetNCst.clear();
  m_recJetId.clear();
  m_recJetTruId.clear();
  m_recJetE.clear();
  m_recJetPt.clear();
  m_recJetEta.clear();
  m_recJetPhi.clear();
  m_recJetArea.clear();
  m_truJetNCst.clear();
  m_truJetId.clear();
  m_truJetTruId.clear();
  m_truJetE.clear();
  m_truJetPt.clear();
  m_truJetEta.clear();
  m_truJetPhi.clear();
  m_truJetArea.clear();
  m_recCstZ.clear();
  m_recCstDr.clear();
  m_recCstE.clear();
  m_recCstJt.clear();
  m_recCstEta.clear();
  m_recCstPhi.clear();
  m_truCstZ.clear();
  m_truCstDr.clear();
  m_truCstE.clear();
  m_truCstJt.clear();
  m_truCstEta.clear();
  m_truCstPhi.clear();
  m_outFile = new TFile();
  return;

}  // end 'initializeVariables()'



void SCorrelatorJetTree::initializeHists() {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::initializeHists() Initializing QA histograms..." << endl;
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
  m_hSumCstEne[CST_TYPE::TRACK_CST]       = new TH1D("hSumTrackEne",   "#SigmaE (cst. track)",      nEneBins,  rEneBins[0],  rEneBins[1]);
  m_hSumCstEne[CST_TYPE::CALO_CST]        = new TH1D("hSumCaloEne",    "#SigmaE (cst. calo.)",      nEneBins,  rEneBins[0],  rEneBins[1]);
  m_hSumCstEne[CST_TYPE::FLOW_CST]        = new TH1D("hSumFlowEne",    "#SigmaE (cst. flow)",       nEneBins,  rEneBins[0],  rEneBins[1]);
  m_hSumCstEne[CST_TYPE::PART_CST]        = new TH1D("hSumPartEne",    "#SigmaE (cst. par.s)",      nEneBins,  rEneBins[0],  rEneBins[1]);
  // track QA
  m_hObjectQA[OBJECT::TRACK][INFO::PT]    = new TH1D("hTrackPt",       "p_{T} (tracks)",            nPtBins,   rPtBins[0],   rPtBins[1]);
  m_hObjectQA[OBJECT::TRACK][INFO::ETA]   = new TH1D("hTrackEta",      "#eta (tracks)",             nEtaBins,  rEtaBins[0],  rEtaBins[1]);
  m_hObjectQA[OBJECT::TRACK][INFO::PHI]   = new TH1D("hTrackPhi",      "#phi (tracks)",             nPhiBins,  rPhiBins[0],  rPhiBins[1]);
  m_hObjectQA[OBJECT::TRACK][INFO::ENE]   = new TH1D("hTrackEne",      "E (tracks)",                nEneBins,  rEneBins[0],  rEneBins[1]);
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
  // particle flow QA
  m_hObjectQA[OBJECT::FLOW][INFO::PT]     = new TH1D("hFlowPt",        "p_{T} (flow)",              nPtBins,   rPtBins[0],   rPtBins[1]);
  m_hObjectQA[OBJECT::FLOW][INFO::ETA]    = new TH1D("hFlowEta",       "#eta (flow)",               nEtaBins,  rEtaBins[0],  rEtaBins[1]);
  m_hObjectQA[OBJECT::FLOW][INFO::PHI]    = new TH1D("hFlowPhi",       "#phi (flow)",               nPhiBins,  rPhiBins[0],  rPhiBins[1]);
  m_hObjectQA[OBJECT::FLOW][INFO::ENE]    = new TH1D("hFlowEne",       "E (flow)",                  nEneBins,  rEneBins[0],  rEneBins[1]);
  // particle QA
  m_hObjectQA[OBJECT::PART][INFO::PT]     = new TH1D("hPartPt",        "p_{T} (par.s)",             nPtBins,   rPtBins[0],   rPtBins[1]);
  m_hObjectQA[OBJECT::PART][INFO::ETA]    = new TH1D("hPartEta",       "#eta (par.s)",              nEtaBins,  rEtaBins[0],  rEtaBins[1]);
  m_hObjectQA[OBJECT::PART][INFO::PHI]    = new TH1D("hPartPhi",       "#phi (par.s)",              nPhiBins,  rPhiBins[0],  rPhiBins[1]);
  m_hObjectQA[OBJECT::PART][INFO::ENE]    = new TH1D("hPartEne",       "E (par.s)",                 nEneBins,  rEneBins[0],  rEneBins[1]);
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
  m_hNumCstAccept[CST_TYPE::TRACK_CST][0] = new TH1D("hNumTrkCstTot",  "N_{cst}^{trk} total",       nNumBins,  rNumBins[0],  rNumBins[1]);
  m_hNumCstAccept[CST_TYPE::TRACK_CST][1] = new TH1D("hNumTrkCstAcc",  "N_{cst}^{trk} accepted",    nNumBins,  rNumBins[0],  rNumBins[1]);
  m_hNumCstAccept[CST_TYPE::CALO_CST][0]  = new TH1D("hNumCaloCstTot", "N_{cst}^{clust} total",     nNumBins,  rNumBins[0],  rNumBins[1]);
  m_hNumCstAccept[CST_TYPE::CALO_CST][1]  = new TH1D("hNumCaloCstAcc", "N_{cst}^{clust} accepted",  nNumBins,  rNumBins[0],  rNumBins[1]);
  m_hNumCstAccept[CST_TYPE::FLOW_CST][0]  = new TH1D("hNumFlowCstTot", "N_{cst}^{flow} total",      nNumBins,  rNumBins[0],  rNumBins[1]);
  m_hNumCstAccept[CST_TYPE::FLOW_CST][1]  = new TH1D("hNumFlowCstAcc", "N_{cst}^{flow} accepted",   nNumBins,  rNumBins[0],  rNumBins[1]);
  m_hNumCstAccept[CST_TYPE::PART_CST][0]  = new TH1D("hNumPartCstTot", "N_{cst}^{par} total",       nNumBins,  rNumBins[0],  rNumBins[1]);
  m_hNumCstAccept[CST_TYPE::PART_CST][1]  = new TH1D("hNumPartCstAcc", "N_{cst}^{par} accepted",    nNumBins,  rNumBins[0],  rNumBins[1]);

}  // end 'initializeHists()'



void SCorrelatorJetTree::initializeTrees() {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::initializeTrees() Initializing output trees..." << endl;
  }

  // initialize output trees
  m_recTree = new TTree("RecoJetTree", "A tree of reconstructed jets");
  m_recTree -> Branch("EvtNumJets",   &m_recNumJets,       "NumJets/I");
  m_recTree -> Branch("Parton3_ID",   &m_recPartonID[0],   "Parton3_ID/I");
  m_recTree -> Branch("Parton4_ID",   &m_recPartonID[1],   "Parton4_ID/I");
  m_recTree -> Branch("Parton3_MomX", &m_recPartonMomX[0], "Parton3_MomX/D");
  m_recTree -> Branch("Parton3_MomY", &m_recPartonMomY[0], "Parton3_MomY/D");
  m_recTree -> Branch("Parton3_MomZ", &m_recPartonMomZ[0], "Parton3_MomZ/D");
  m_recTree -> Branch("Parton4_MomX", &m_recPartonMomX[1], "Parton4_MomX/D");
  m_recTree -> Branch("Parton4_MomY", &m_recPartonMomY[1], "Parton4_MomY/D");
  m_recTree -> Branch("Parton4_MomZ", &m_recPartonMomZ[1], "Parton4_MomZ/D");
  m_recTree -> Branch("JetNumCst",    &m_recJetNCst);
  m_recTree -> Branch("JetID",        &m_recJetId);
  m_recTree -> Branch("JetTruthID",   &m_recJetTruId);
  m_recTree -> Branch("JetEnergy",    &m_recJetE);
  m_recTree -> Branch("JetPt",        &m_recJetPt);
  m_recTree -> Branch("JetEta",       &m_recJetEta);
  m_recTree -> Branch("JetPhi",       &m_recJetPhi);
  m_recTree -> Branch("JetArea",      &m_recJetArea);
  m_recTree -> Branch("CstZ",         &m_recCstZ);
  m_recTree -> Branch("CstDr",        &m_recCstDr);
  m_recTree -> Branch("CstEnergy",    &m_recCstE);
  m_recTree -> Branch("CstJt",        &m_recCstJt);
  m_recTree -> Branch("CstEta",       &m_recCstEta);
  m_recTree -> Branch("CstPhi",       &m_recCstPhi);

  m_truTree = new TTree("TruthJetTree", "A tree of truth jets");
  m_truTree -> Branch("EvtNumJets",   &m_truNumJets,       "NumJets/I");
  m_truTree -> Branch("Parton3_ID",   &m_truPartonID[0],   "Parton3_ID/I");
  m_truTree -> Branch("Parton4_ID",   &m_truPartonID[1],   "Parton4_ID/I");
  m_truTree -> Branch("Parton3_MomX", &m_truPartonMomX[0], "Parton3_MomX/D");
  m_truTree -> Branch("Parton3_MomY", &m_truPartonMomY[0], "Parton3_MomY/D");
  m_truTree -> Branch("Parton3_MomZ", &m_truPartonMomZ[0], "Parton3_MomZ/D");
  m_truTree -> Branch("Parton4_MomX", &m_truPartonMomX[1], "Parton4_MomX/D");
  m_truTree -> Branch("Parton4_MomY", &m_truPartonMomY[1], "Parton4_MomY/D");
  m_truTree -> Branch("Parton4_MomZ", &m_truPartonMomZ[1], "Parton4_MomZ/D");
  m_truTree -> Branch("JetNumCst",    &m_truJetNCst);
  m_truTree -> Branch("JetID",        &m_truJetId);
  m_truTree -> Branch("JetTruthID",   &m_truJetTruId);
  m_truTree -> Branch("JetEnergy",    &m_truJetE);
  m_truTree -> Branch("JetPt",        &m_truJetPt);
  m_truTree -> Branch("JetEta",       &m_truJetEta);
  m_truTree -> Branch("JetPhi",       &m_truJetPhi);
  m_truTree -> Branch("JetArea",      &m_truJetArea);
  m_truTree -> Branch("CstZ",         &m_truCstZ);
  m_truTree -> Branch("CstDr",        &m_truCstDr);
  m_truTree -> Branch("CstEnergy",    &m_truCstE);
  m_truTree -> Branch("CstJt",        &m_truCstJt);
  m_truTree -> Branch("CstEta",       &m_truCstEta);
  m_truTree -> Branch("CstPhi",       &m_truCstPhi);
  return;

}  // end 'initializeTrees()'



int SCorrelatorJetTree::createJetNode(PHCompositeNode* topNode) {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::createJetNode(PHCompositeNode *topNode) Creating jet node..." << endl;
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
  string jetNodeName;
  string jetNodeNameMC;
  if (m_jetcontainer_name.empty()) {
    baseName = "JetTree";
  } else {
    baseName = m_jetcontainer_name;
  }

  // cant have forward slashes in DST or else you make a subdirectory on save!!!
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
  jetNodeName   = baseName + "_Jet_Container";
  jetNodeNameMC = baseName + "_MC_Jet_Container";

  // construct jet maps
  m_jetMap = new JetMapv1();
  if (m_ismc && m_save_truth_dst) {
    m_truth_jetMap = new JetMapv1();
  }

  // add jet node
  PHIODataNode<PHObject>* jetNode = new PHIODataNode<PHObject>(m_jetMap, jetNodeName.c_str(), "PHObject");
  lowerNode -> addNode(jetNode);
  cout << jetNodeName << " node added" << endl;

  // save truth DST if needed
  if(m_ismc && m_save_truth_dst) {
    PHIODataNode<PHObject> *jetNodeMC = new PHIODataNode<PHObject>(m_truth_jetMap, jetNodeNameMC.c_str(), "PHObject");
    lowerNode -> addNode(jetNodeMC);
    cout << jetNodeNameMC << " node added" << endl;
  }
  return Fun4AllReturnCodes::EVENT_OK;

}  // end 'createJetNode(PHCompositeNode*)'



void  SCorrelatorJetTree::saveOutput() {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::saveOutput() Saving output trees and histograms..." << endl;
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
        case CST_TYPE::CALO_CST:
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
  m_recTree -> Write();
  m_truTree -> Write();
  return;

}  // end 'saveOutput()'



void SCorrelatorJetTree::resetTreeVariables() {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::resetTreeVariables() Resetting tree variables..." << endl;
  }
  m_recNumJets       = 0;
  m_recPartonID[0]   = -9999;
  m_recPartonID[1]   = -9999;
  m_recPartonMomX[0] = -9999.;
  m_recPartonMomX[1] = -9999.;
  m_recPartonMomY[0] = -9999.;
  m_recPartonMomY[1] = -9999.;
  m_recPartonMomZ[0] = -9999.;
  m_recPartonMomZ[1] = -9999.;
  m_truNumJets       = 0;
  m_truPartonID[0]   = -9999;
  m_truPartonID[1]   = -9999;
  m_truPartonMomX[0] = -9999.;
  m_truPartonMomX[1] = -9999.;
  m_truPartonMomY[0] = -9999.;
  m_truPartonMomY[1] = -9999.;
  m_truPartonMomZ[0] = -9999.;
  m_truPartonMomZ[1] = -9999.;
  m_recJetNCst.clear();
  m_recJetId.clear();
  m_recJetTruId.clear();
  m_recJetE.clear();
  m_recJetPt.clear();
  m_recJetEta.clear();
  m_recJetPhi.clear();
  m_truJetNCst.clear();
  m_truJetId.clear();
  m_truJetTruId.clear();
  m_truJetE.clear();
  m_truJetPt.clear();
  m_truJetEta.clear();
  m_truJetPhi.clear();
  m_recCstZ.clear();
  m_recCstDr.clear();
  m_recCstE.clear();
  m_recCstJt.clear();
  m_recCstEta.clear();
  m_recCstPhi.clear();
  m_truCstZ.clear();
  m_truCstDr.clear();
  m_truCstE.clear();
  m_truCstJt.clear();
  m_truCstEta.clear();
  m_truCstPhi.clear();
  return;

}  // end 'resetTreeVariables()

// end ------------------------------------------------------------------------
