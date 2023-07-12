// ----------------------------------------------------------------------------
// 'SEnergyCorrelator.sys.h'
// Derek Anderson
// 01.27.2023
//
// A module to implement Peter Komiske's
// EEC library in the sPHENIX software
// stack.
// ----------------------------------------------------------------------------

#pragma once

using namespace std;
using namespace fastjet;



// system methods -------------------------------------------------------------

void SEnergyCorrelator::InitializeMembers() {

  // print debug statement
  if (m_inDebugMode) PrintDebug(0);

  m_inFile            = 0x0;
  m_inTree            = 0x0;
  m_outFile           = 0x0;
  m_fCurrent          = 0;
  m_verbosity         = 0;
  m_inDebugMode       = false;
  m_inBatchMode       = false;
  m_inComplexMode     = false;
  m_inStandaloneMode  = false;
  m_isInputTreeTruth  = false;
  m_moduleName        = "";
  m_inFileName        = "";
  m_inNodeName        = "";
  m_inTreeName        = "";
  m_outFileName       = "";
  m_nPointCorr        = 0;
  m_nBinsDr           = 0;
  m_nBinsJetPt        = 0;
  m_drBinRange[0]     = 0.;
  m_drBinRange[1]     = 0.;
  m_ptJetRange[0]     = 0.;
  m_ptJetRange[1]     = 0.;
  m_etaJetRange[0]    = 0.;
  m_etaJetRange[1]    = 0.;
  m_momCstRange[0]    = 0.;
  m_momCstRange[1]    = 0.;
  m_drCstRange[0]     = 0.;
  m_drCstRange[1]     = 0.;
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
  m_jetCstVector.clear();
  m_outHistDrAxis.clear();
  m_outHistLnDrAxis.clear();
  return;

}  // end 'InitializeMembers()'



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

  // announce tree setting
  if (m_inStandaloneMode) PrintMessage(2);
  return;

}  // end 'InitializeTree()'



void SEnergyCorrelator::InitializeHists() {

  // print debug statement
  if (m_inDebugMode) PrintDebug(5);

  for (size_t iPtBin = 0; iPtBin < m_nBinsJetPt; iPtBin++) {
    TH1D *hInitialDrAxis   = 0x0;
    TH1D *hInitialLnDrAxis = 0x0;
    m_outHistDrAxis.push_back(hInitialDrAxis);
    m_outHistLnDrAxis.push_back(hInitialLnDrAxis);
  }

  // announce histogram initialization
  if (m_inStandaloneMode) PrintMessage(3);
  return;

}  // end 'InitializeHists()'



void SEnergyCorrelator::InitializeCorrs() {

  // print debug statement
  if (m_inDebugMode) PrintDebug(6);

  // initialize correlator for each jet pt bin
  for (size_t iPtBin = 0; iPtBin < m_nBinsJetPt; iPtBin++) {
    m_eecLongSide.push_back(new contrib::eec::EECLongestSide<contrib::eec::hist::axis::log>(m_nPointCorr, m_nBinsDr, {m_drBinRange[0], m_drBinRange[1]}));
  }

  // announce correlator initialization
  if (m_inStandaloneMode) PrintMessage(4);
  return;

}  // end 'InitializeCorrs()'



void SEnergyCorrelator::PrintMessage(const uint32_t code, const uint64_t nEvts, const uint64_t event) {

  // print debug statement
  if (m_inDebugMode && (m_verbosity > 5)) PrintDebug(22);

  switch (code) {
    case 0:
      cout << "\n  Running standalone correlator calculation...\n"
           << "    Set name & modes:\n"
           << "      module name      = " << m_moduleName.data() << "\n"
           << "      complex mode?    = " << m_inComplexMode     << "\n"
           << "      standalone mode? = " << m_inStandaloneMode  << "\n"
           << "      debug mode?      = " << m_inDebugMode       << "\n"
           << "      batch mode?      = " << m_inBatchMode
           << endl;
      break;
    case 1:
      cout << "    Opened files:\n"
           << "      input  = " << m_inFileName.data() << "\n"
           << "      output = " << m_outFileName.data()
           << endl;
      break;
    case 2:
      cout << "    Initialized input tree:\n"
           << "      tree name = " << m_inTreeName.data()
           << endl;
      break;
    case 3:
      cout << "    Initialized output histograms." << endl;
      break;
    case 4:
      cout << "    Initialized correlators." << endl;
      break;
    case 5:
      cout << "    Set correlator parameters:\n"
           << "      n-point = "       << m_nPointCorr    << ", number of dR bins = " << m_nBinsDr       << "\n"
           << "      dR bin range = (" << m_drBinRange[0] << ", "                     << m_drBinRange[1] << ")"
           << endl;
      break;
    case 6:
      cout << "    Set jet parameters:\n"
           << "      eta range = (" << m_etaJetRange[0] << ", " << m_etaJetRange[1] << ")\n"
           << "      pt range  = (" << m_ptJetRange[0]  << ", " << m_ptJetRange[1]  << ")\n"
           << "    Set pTjet bins:"
           << endl;
      for (uint32_t iPtBin = 0; iPtBin < m_nBinsJetPt; iPtBin++) {
        cout << "      bin[" << iPtBin << "] = (" << m_ptJetBins.at(iPtBin).first << ", " << m_ptJetBins.at(iPtBin).second << ")" << endl;
      }
      break;
    case 7:
      cout << "    Beginning event loop: " << nEvts << " events to process..." << endl;
      break;
    case 8:
      if (m_inBatchMode) {
        cout << "      processing event " << (event + 1) << "/" << nEvts << "..." << endl;
      } else {
        cout << "      processing event " << (event + 1) << "/" << nEvts << "...\r" << flush;
        if ((event + 1) == nEvts) cout << endl;
      }
      break; 
    case 9:
      cout << "    Analysis finished!" << endl;
      break;
    case 10:
      cout << "    Saved output histograms." << endl;
      break;
    case 11:
      cout << "  Finished correlator calculation!\n" << endl;
      break;
    case 12:
      cout << "    Set constituent parameters:\n"
           << "      momentum range = (" << m_momCstRange[0] << ", " << m_momCstRange[1] << ")\n"
           << "      dr range       = (" << m_drCstRange[0]  << ", " << m_drCstRange[1]  << ")"
           << endl;
      break;
    case 13:
      cout << "    Finished event loop!" << endl;
      break;
    case 14:
      cout << "    Extracted output histograms from correlators." << endl;
      break;
  }
  return;

}  // end 'PrintMessage(uint32_t)'


void SEnergyCorrelator::PrintDebug(const uint32_t code) {

  // print debug statement
  if (m_inDebugMode && (m_verbosity > 7)) {
    cout << "SEnergyCorrelator::PrintDebug(uint32_t) printing a debugging statement..." << endl;
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
      cout << "SEnergyCorrelator::GetEntry(uint64_t) getting tree entry..." << endl;
      break;
    case 17:
      cout << "SEnergyCorrelator::LoadTree(uint64_t) loading tree..." << endl;
      break;
    case 18:
      cout << "SEnergyCorrelator::SetInputTree(string, bool) setting input tree name..." << endl;
      break;
    case 19:
      cout << "SEnergyCorrelator::SetCorrelatorParameters(uint32_t, uint64_t, double, double) setting correlator parameters..." << endl;
      break;
    case 20:
      cout << "SEnergyCorrelator::SetJetParameters(vector<pair<double, double>>, double, double) setting jet parameters..." << endl;
      break;
    case 21:
      cout << "SEnergyCorrelators:CheckCriticalParameters() checking critical parameters..." << endl;
      break;
    case 22:
      cout << "SEnergyCorrelator::PrintMessage(uint32_t, uint64_t, uint64_t) printing a message..." << endl;
      break;
    case 23:
      cout << "SEnergyCorrelator::PrintError(uint32_t) printing an error..." << endl;
      break;
    case 24:
      cout << "SEnergyCorrelator::SetConstituentParameters(double, double, double, double) setting constituent parameters..." << endl;
      break;
    case 25:
      cout << "SEnergyCorrelator::ExtractHistsFromCorr() extracting output histograms..." << endl;
      break;
    case 26:
      cout << "SEnergyCorrelator::ApplyJetCuts(double, double) applying jet cuts..." << endl;
      break;
    case 27:
      cout << "SEnergyCorrelator::ApplyCstCuts(double, double) applying constituent cuts..." << endl;
      break;
    case 28:
      cout << "SEnergyCorrelator::GetJetPtBin(double) getting jet pT bin..." << endl;
      break;
    case 29:
      cout << "SEnergyCorrelator::CloseInputFile() closing input file..." << endl;
      break;
    case 30:
      cout << "SEnergyCorrelator::CloseOutputFile() closing output file..." << endl;
      break;
    case 31:
      cout << "SEnergyCorrelator::DoCorrelatorCalculation() looping over events and calculating correlators..." << endl;
      break;
  }
  return;

}  // end 'PrintDebug(uint32_t)'



void SEnergyCorrelator::PrintError(const uint32_t code, const size_t nDrBinEdges, const size_t iDrBin) {

  // print debug statement
  if (m_inDebugMode && (m_verbosity > 5)) PrintDebug(23);

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
    case 12:
      if (m_inComplexMode) {
        cerr << "SEnergyCorrelator::ExtraHistsFromCorr() PANIC: number of dR bin edges is no good! Aborting!" << endl;
      } else {
        cerr << "PANIC: number of dR bin edges is no good! Aborting!\n"
             << "       nDrBinEdges = " << nDrBinEdges << ", nDrBins = " << m_nBinsDr
             << endl;
      }
      break;
    case 13:
      if (m_inStandaloneMode) {
        cerr << "WARNING: dR bin #" << iDrBin << " has a NAN as content or error..." << endl;
      }
      break;
    case 14:
      if (m_inComplexMode) {
        cerr << "SEnergyCorrelatorFile::End() PANIC: calling standalone method in complex mode! Aborting!" << endl;
      } else {
        cerr << "PANIC: calling standalone method in complex mode! Aborting!" << endl;
      }
      break;
  }
  return;

}  // end 'PrintError(unint32_t)'



bool SEnergyCorrelator::CheckCriticalParameters() {

  // print debugging statement
  if (m_inDebugMode) PrintDebug(21); 

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
