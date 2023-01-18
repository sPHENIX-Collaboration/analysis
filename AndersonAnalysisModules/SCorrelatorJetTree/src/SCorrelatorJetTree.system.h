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
  m_qualy_plots          = false;
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
  m_outFile = new TFile();
  return;

}  // end 'initializeVariables()'



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
