// 'SEnergyCorrelator.sys.h'
// Derek Anderson
// 01.27.2023
//
// A module to implement Peter Komiske's
// EEC library in the sPHENIX software
// stack.

#pragma once

using namespace std;
using namespace fastjet;



void SEnergyCorrelator::InitializeMembers() {

  // print debug statement
  if (m_inDebugMode) PrintDebug(0);

  m_inFile            = 0x0;
  m_inTree            = 0x0;
  m_outFile           = 0x0;
  m_fCurrent          = 0;
  m_verbosity         = 0;
  m_inDebugMode       = false;
  m_inComplexMode     = false;
  m_inStandaloneMode  = false;
  m_isInputTreeTruth  = false;
  m_inFileName        = "";
  m_inNodeName        = "";
  m_inTreeName        = "";
  m_outFileName       = "";
  m_nPointCorr        = 0;
  m_nBinsDr           = 0;
  m_nBinsJetPt        = 0;
  m_drBinRange[0]     = 0.;
  m_drBinRange[1]     = 0.;
  m_truParton3_ID     = 0;
  m_truParton4_ID     = 0;
  m_truParton3_MomX   = 0.;
  m_truParton3_MomY   = 0.;
  m_truParton3_MomZ   = 0.;
  m_truParton4_MomX   = 0.;
  m_truParton4_MomY   = 0.;
  m_truParton4_MomZ   = 0.;
  m_recParton3_ID     = 0;
  m_recParton4_ID     = 0;
  m_recParton3_MomX   = 0.;
  m_recParton3_MomY   = 0.;
  m_recParton3_MomZ   = 0.;
  m_recParton4_MomX   = 0.;
  m_recParton4_MomY   = 0.;
  m_recParton4_MomZ   = 0.;
  m_evtNumJets        = 0;
  m_jetNumCst         = 0x0;
  m_jetID             = 0x0;
  m_jetTruthID        = 0x0;
  m_jetEnergy         = 0x0;
  m_jetPt             = 0x0;
  m_jetEta            = 0x0;
  m_jetPhi            = 0x0;
  m_jetArea           = 0x0;
  m_cstZ              = 0x0;
  m_cstDr             = 0x0;
  m_cstEnergy         = 0x0;
  m_cstJt             = 0x0;
  m_cstEta            = 0x0;
  m_cstPhi            = 0x0;
  m_brTruParton3_ID   = 0x0;
  m_brTruParton4_ID   = 0x0;
  m_brTruParton3_MomX = 0x0;
  m_brTruParton3_MomY = 0x0;
  m_brTruParton3_MomZ = 0x0;
  m_brTruParton4_MomX = 0x0;
  m_brTruParton4_MomY = 0x0;
  m_brTruParton4_MomZ = 0x0;
  m_brRecParton3_ID   = 0x0;
  m_brRecParton4_ID   = 0x0;
  m_brRecParton3_MomX = 0x0;
  m_brRecParton3_MomY = 0x0;
  m_brRecParton3_MomZ = 0x0;
  m_brRecParton4_MomX = 0x0;
  m_brRecParton4_MomY = 0x0;
  m_brRecParton4_MomZ = 0x0;
  m_brEvtNumJets      = 0x0;
  m_brJetNumCst       = 0x0;
  m_brJetID           = 0x0;
  m_brJetTruthID      = 0x0;
  m_brJetEnergy       = 0x0;
  m_brJetPt           = 0x0;
  m_brJetEta          = 0x0;
  m_brJetPhi          = 0x0;
  m_brJetArea         = 0x0;
  m_brCstZ            = 0x0;
  m_brCstDr           = 0x0;
  m_brCstEnergy       = 0x0;
  m_brCstJt           = 0x0;
  m_brCstEta          = 0x0;
  m_brCstPhi          = 0x0;
  m_ptJetBins.clear();
  m_eecLongSide.clear();
  return;

}  // end 'InitializeMembers()'



void SEnergyCorrelator::InitializeHists() {

  // print debug statement
  if (m_inDebugMode) PrintDebug(5);

  /* TODO output histograms wil be initialized here */
  return;

}  // end 'InitializeHists()'



void SEnergyCorrelator::InitializeCorrs() {

  // print debug statement
  if (m_inDebugMode) PrintDebug(6);

  // initialize correlator for each jet pt bin
  for (size_t iPtBin = 0; iPtBin < m_nBinsJetPt; iPtBin++) {
    m_eecLongSide.push_back(new contrib::eec::EECLongestSide<contrib::eec::hist::axis::log>(m_nPointCorr, m_nBinsDr, {m_drBinRange[0], m_drBinRange[1]}));
  }
  return;

}  // end 'InitializeCorrs()'



void SEnergyCorrelator::InitializeTree() {

  // print debug statement
  if (m_inDebugMode) PrintDebug(4);

  // check for tree
  if (!m_inTree) {
    PrintError(10);
    assert(m_inTree);
  }
  m_fCurrent = -1;
  m_inTree   -> SetMakeClass(1);

  // set truth vs. reco branch addresses
  if (m_isInputTreeTruth) {
    m_inTree -> SetBranchAddress("Parton3_ID",   &m_truParton3_ID,   &m_brTruParton3_ID);
    m_inTree -> SetBranchAddress("Parton4_ID",   &m_truParton4_ID,   &m_brTruParton4_ID);
    m_inTree -> SetBranchAddress("Parton3_MomX", &m_truParton3_MomX, &m_brTruParton3_MomX);
    m_inTree -> SetBranchAddress("Parton3_MomY", &m_truParton3_MomY, &m_brTruParton3_MomY);
    m_inTree -> SetBranchAddress("Parton3_MomZ", &m_truParton3_MomZ, &m_brTruParton3_MomZ);
    m_inTree -> SetBranchAddress("Parton4_MomX", &m_truParton4_MomX, &m_brTruParton4_MomX);
    m_inTree -> SetBranchAddress("Parton4_MomY", &m_truParton4_MomY, &m_brTruParton4_MomY);
    m_inTree -> SetBranchAddress("Parton4_MomZ", &m_truParton4_MomZ, &m_brTruParton4_MomZ);
  } else {
    m_inTree -> SetBranchAddress("Parton3_ID",   &m_recParton3_ID,   &m_brRecParton3_ID);
    m_inTree -> SetBranchAddress("Parton4_ID",   &m_recParton4_ID,   &m_brRecParton4_ID);
    m_inTree -> SetBranchAddress("Parton3_MomX", &m_recParton3_MomX, &m_brRecParton3_MomX);
    m_inTree -> SetBranchAddress("Parton3_MomY", &m_recParton3_MomY, &m_brRecParton3_MomY);
    m_inTree -> SetBranchAddress("Parton3_MomZ", &m_recParton3_MomZ, &m_brRecParton3_MomZ);
    m_inTree -> SetBranchAddress("Parton4_MomX", &m_recParton4_MomX, &m_brRecParton4_MomX);
    m_inTree -> SetBranchAddress("Parton4_MomY", &m_recParton4_MomY, &m_brRecParton4_MomY);
    m_inTree -> SetBranchAddress("Parton4_MomZ", &m_recParton4_MomZ, &m_brRecParton4_MomZ);
  }

  // set generic branch addresses
  m_inTree -> SetBranchAddress("EvtNumJets", &m_evtNumJets, &m_brEvtNumJets);
  m_inTree -> SetBranchAddress("JetNumCst",  &m_jetNumCst,  &m_brJetNumCst);
  m_inTree -> SetBranchAddress("JetID",      &m_jetID,      &m_brJetID);
  m_inTree -> SetBranchAddress("JetTruthID", &m_jetTruthID, &m_brJetTruthID);
  m_inTree -> SetBranchAddress("JetEnergy",  &m_jetEnergy,  &m_brJetEnergy);
  m_inTree -> SetBranchAddress("JetPt",      &m_jetPt,      &m_brJetPt);
  m_inTree -> SetBranchAddress("JetEta",     &m_jetEta,     &m_brJetEta);
  m_inTree -> SetBranchAddress("JetPhi",     &m_jetPhi,     &m_brJetPhi);
  m_inTree -> SetBranchAddress("JetArea",    &m_jetArea,    &m_brJetArea);
  m_inTree -> SetBranchAddress("CstZ",       &m_cstZ,       &m_brCstZ);
  m_inTree -> SetBranchAddress("CstDr",      &m_cstDr,      &m_brCstDr);
  m_inTree -> SetBranchAddress("CstEnergy",  &m_cstEnergy,  &m_brCstEnergy);
  m_inTree -> SetBranchAddress("CstJt",      &m_cstJt,      &m_brCstJt);
  m_inTree -> SetBranchAddress("CstEta",     &m_cstEta,     &m_brCstEta);
  m_inTree -> SetBranchAddress("CstPhi",     &m_cstPhi,     &m_brCstPhi);
  return;

}  // end 'InitializeTree()'



void SEnergyCorrelator::PrintMessage(const uint32_t code) {

  // print debug statement
  if (m_inDebugMode && (m_verbosity > 5)) {
    cout << "SEnergyCorrelator::PrintMessage() printing a message..." << endl;
  }

  /* TODO add standalone running statements */
  switch (code) {
    default:
      cout << "testing testing..." << endl;
      break;
  }
  return;

}  // end 'PrintMessage(uint32_t)'


void SEnergyCorrelator::PrintDebug(const uint32_t code) {

  // print debug statement
  if (m_inDebugMode && (m_verbosity > 5)) {
    cout << "SEnergyCorrelator::PrintDebug() printing a debugging statement..." << endl;
  }

  switch (code) {
    case 0:
      cout << "SEnergyCorrelator::InitializeMembers() initializing internal variables..." << endl;
      break;
    case 1:
      cout << "SEnergyCorrelator::SEnergyCorrelator(string, bool, bool) calling ctor..." << endl;
      break;
    case 2:
      cout << "SEnergyCorrelator::Init(PHCompositeNode*) initializing..." << endl;
      break;
    case 3:
      cout << "SEnergyCorrelator::GrabInputNode() grabbing input node..." << endl;
      break;
    case 4:
      cout << "SEnergyCorrelator::InitializeTree() initializing input tree..." << endl;
      break;
    case 5:
      cout << "SEnergyCorrelator::InitializeHists() initializing histograms..." << endl;
      break;
    case 6:
      cout << "SEnergyCorrelator::InitializeCorrs() initializing correlators" << endl;
      break;
    case 7:
      cout << "SEnergyCorrelator::process_event(PHCompositeNode*) processing event..." << endl;
      break;
    case 8:
      cout << "SEnergyCorrelator::End(PHCompositeNode*) this is the end..." << endl;
      break;
    case 9:
      cout << "SEnergyCorrelator::SaveOutput() saving output..." << endl;
      break;
    case 10:
      cout << "SEnergyCorrelator::Init() initializing..." << endl;
      break;
    case 11:
      cout << "SenergyCorrelator::OpenInputFile() opening input file..." << endl;
      break;
    case 12:
      cout << "SEnergyCorrelator::Analyze() analyzing input..." << endl;
      break;
    case 13:
      cout << "SEnergyCorrelator::End() this is the end..." << endl;
      break;
    case 14:
      cout << "SEnergyCorrelator::~SEnergyCorrelator() calling dtor..." << endl;
      break;
    case 15:
      cout << "SEnergyCorrelator::OpenOutputFile() opening output file..." << endl;
      break;
    case 16:
      cout << "SEnergyCorrelator::GetEntry() getting tree entry..." << endl;
      break;
    case 17:
      cout << "SEnergyCorrelator::LoadTree() loading tree..." << endl;
      break;
  }
  return;

}  // end 'PrintDebug(uint32_t)'



void SEnergyCorrelator::PrintError(const uint32_t code) {

  // print debug statement
  if (m_inDebugMode && (m_verbosity > 5)) {
    cout << "SEnergyCorrelator::PrintError() printing an error..." << endl;
  }

  switch (code) {
    case 0:
      if (m_inComplexMode) {
        cerr << "SEnergyCorrelator::Init(PHCompositeNode*) PANIC: calling complex method in standalone mode! Aborting!" << endl;
      } else {
        cerr << "PANIC: calling complex method in standalone mode! Aborting!" << endl;
      }
      break;
    case 1:
      if (m_inComplexMode) {
        cerr << "SEnergyCorrelator::GrabInputNode() PANIC: couldn't grab node \"" << m_inNodeName << "\"! Aborting!" << endl;
      } else {
        cerr << "PANIC: couldn't grab node \"" << m_inNodeName << "\"! Aborting!" << endl;
      }
      break;
    case 2:
      if (m_inComplexMode) {
        cerr << "SEnergyCorrelator::GrabInputNode() PANIC: couldn't grab tree \"" << m_inTreeName << "\" from node \"" << m_inNodeName << "\"! Aborting!" << endl;
      } else {
        cerr << "PANIC: couldn't grab tree \"" << m_inTreeName << "\" from node \"" << m_inNodeName << "\"! Aborting!" << endl;
      }
      break;
    case 3:
      if (m_inComplexMode) {
        cerr << "SEnergyCorrelator::process_event(PHCompositeNode*) PANIC: calling complex method in standalone mode! Aborting!" << endl;
      } else {
        cerr << "PANIC: calling complex method in standalone mode! Aborting!" << endl;
      }
      break;
    case 4:
      if (m_inComplexMode) {
        cerr << "SEnergyCorrelator::End(PHCompositeNode*) PANIC: calling complex method in standalone mode! Aborting!" << endl;
      } else {
        cerr << "PANIC: calling complex method in standalone mode! Aborting!" << endl;
      }
      break;
    case 5:
      if (m_inComplexMode) {
        cerr << "SEnergyCorrelator::Init() PANIC: calling standalone method in complex mode! Aborting!" << endl;
      } else {
        cerr << "PANIC: calling standalone method in complex mode! Aborting!" << endl;
      }
      break;
    case 6:
      if (m_inComplexMode) {
        cerr << "SEnergyCorrelator::OpenInputFile() PANIC: couldn't open file \"" << m_inFileName << "\"! Aborting!" << endl;
      } else {
        cerr << "PANIC: couldn't open file \"" << m_inFileName << "\"! Aborting!" << endl;
      }
      break;
    case 7:
      if (m_inComplexMode) {
        cerr << "SEnergyCorrelator::OpenInputFile() PANIC: couldn't grab tree \"" << m_inTreeName << "\" from file \"" << m_inFileName << "\"! Aborting!" << endl;
      } else {
        cerr << "PANIC: couldn't grab tree \"" << m_inTreeName << "\" from file \"" << m_inFileName << "\"! Aborting!" << endl;
      }
      break;
    case 8:
      if (m_inComplexMode) {
        cerr << "SEnergyCorrelator::Analyze() PANIC: calling standalone method in complex mode! Aborting!" << endl;
      } else {
        cerr << "PANIC: calling standalone method in complex mode! Aborting!" << endl;
      }
      break;
    case 9:
      if (m_inComplexMode) {
        cerr << "SEnergyCorrelator::End() PANIC: calling standalone method in complex mode! Aborting!" << endl;
      } else {
        cerr << "PANIC: calling standalone method in complex mode! Aborting!" << endl;
      }
      break;
    case 10:
      if (m_inComplexMode) {
        cerr << "SEnergyCorrelator::InitializeTree() PANIC: no TTree! Aborting!" << endl;
      } else {
        cerr << "PANIC: no TTree! Aborting!" << endl;
      }
      break;
    case 11:
      if (m_inComplexMode) {
        cerr << "SEnergyCorrelator::OpenOutputFile() PANIC: couldn't open output file! Aborting!" << endl;
      } else {
        cerr << "PANIC: couldn't open output file! Aborting!" << endl;
      }
      break;
  }
  return;

}  // end 'PrintError(unint32_t)'



bool SEnergyCorrelator::CheckCriticalParameters() {

  // print debugging statement
  /* TODO add statement */

  /* TODO checking goes here */
  return true;

}  // end 'CheckCriticalParameters()'



int64_t SEnergyCorrelator::GetEntry(const uint64_t entry) {

  // print debugging statemet
  if (m_inDebugMode && (m_verbosity > 5)) PrintDebug(16);

  int64_t entryStatus(-1);
  if (!m_inTree) {
    entryStatus = 0;
  } else {
    entryStatus = m_inTree -> GetEntry(entry);
  }
  return entryStatus;

}  // end 'GetEntry(uint64_t)'



int64_t SEnergyCorrelator::LoadTree(const uint64_t entry) {

  // print debugging statemet
  if (m_inDebugMode && (m_verbosity > 5)) PrintDebug(17);

  // check for tree & load
  int     treeNumber(-1);
  int64_t treeStatus(-1);
  if (!m_inTree) {
    treeStatus = -5;
  } else {
    treeNumber = m_inTree -> GetTreeNumber();
    treeStatus = m_inTree -> LoadTree(entry);
  }

  // update current tree number if need be
  const bool isTreeStatusGood = (treeStatus >= 0);
  const bool isNotCurrentTree = (treeNumber != m_fCurrent);
  if (isTreeStatusGood && isNotCurrentTree) {
    m_fCurrent = m_inTree -> GetTreeNumber();
  }
  return treeStatus;

}  // end 'LoadTree(uint64_t)'

// end ------------------------------------------------------------------------
