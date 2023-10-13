// ----------------------------------------------------------------------------
// 'SEnergyCorrelator.sys.h'
// Derek Anderson
// 01.27.2023
//
// A module to implement Peter Komiske's EEC library
// in the sPHENIX software stack for the Cold QCD
// Energy-Energy Correlator analysis.
// ----------------------------------------------------------------------------

#pragma once

using namespace std;
using namespace fastjet;



namespace SColdQcdCorrelatorAnalysis {

  // system methods -----------------------------------------------------------

  void SEnergyCorrelator::InitializeMembers() {

    // print debug statement
    if (m_inDebugMode) PrintDebug(0);

    m_ptJetBins.clear();
    m_inFileNames.clear();
    m_eecLongSide.clear();
    m_subEvtsToUse.clear();
    m_jetCstVector.clear();
    m_outHistVarDrAxis.clear();
    m_outHistErrDrAxis.clear();
    m_outHistVarLnDrAxis.clear();
    m_outHistErrLnDrAxis.clear();
    return;

  }  // end 'InitializeMembers()'



  void SEnergyCorrelator::InitializeTree() {

    // print debug statement
    if (m_inDebugMode) PrintDebug(4);

    // check for tree
    if (!m_inChain) {
      PrintError(10);
      assert(m_inChain);
    }
    m_fCurrent = -1;
    m_inChain -> SetMakeClass(1);

    // set truth vs. reco branch addresses
    if (m_isInputTreeTruth) {
      m_inChain -> SetBranchAddress("Parton3_ID",   &m_partonID[0],   &m_brPartonID[0]);
      m_inChain -> SetBranchAddress("Parton4_ID",   &m_partonID[1],   &m_brPartonID[1]);
      m_inChain -> SetBranchAddress("Parton3_MomX", &m_partonMomX[0], &m_brPartonMomX[0]);
      m_inChain -> SetBranchAddress("Parton3_MomY", &m_partonMomY[0], &m_brPartonMomY[0]);
      m_inChain -> SetBranchAddress("Parton3_MomZ", &m_partonMomZ[0], &m_brPartonMomZ[0]);
      m_inChain -> SetBranchAddress("Parton4_MomX", &m_partonMomX[1], &m_brPartonMomX[1]);
      m_inChain -> SetBranchAddress("Parton4_MomY", &m_partonMomY[1], &m_brPartonMomY[1]);
      m_inChain -> SetBranchAddress("Parton4_MomZ", &m_partonMomZ[1], &m_brPartonMomZ[1]);
      m_inChain -> SetBranchAddress("EvtSumParEne", &m_evtSumPar,     &m_brEvtSumPar);
      m_inChain -> SetBranchAddress("CstID",        &m_cstID,         &m_brCstID);
      m_inChain -> SetBranchAddress("CstEmbedID",   &m_cstEmbedID,    &m_brCstEmbedID);
    } else {
      m_inChain -> SetBranchAddress("EvtNumTrks",    &m_evtNumTrks, &m_brEvtNumTrks);
      m_inChain -> SetBranchAddress("EvtSumECalEne", &m_evtSumECal, &m_brEvtSumECal);
      m_inChain -> SetBranchAddress("EvtSumHCalEne", &m_evtSumHCal, &m_brEvtSumHCal);
      m_inChain -> SetBranchAddress("CstMatchID",    &m_cstMatchID, &m_brCstMatchID);
    }

    // set generic branch addresses
    m_inChain -> SetBranchAddress("EvtVtxX",    &m_evtVtxX,    &m_brEvtVtxX);
    m_inChain -> SetBranchAddress("EvtVtxY",    &m_evtVtxY,    &m_brEvtVtxY);
    m_inChain -> SetBranchAddress("EvtVtxZ",    &m_evtVtxZ,    &m_brEvtVtxZ);
    m_inChain -> SetBranchAddress("EvtNumJets", &m_evtNumJets, &m_brEvtNumJets);
    m_inChain -> SetBranchAddress("JetNumCst",  &m_jetNumCst,  &m_brJetNumCst);
    m_inChain -> SetBranchAddress("JetID",      &m_jetID,      &m_brJetID);
    m_inChain -> SetBranchAddress("JetEnergy",  &m_jetEnergy,  &m_brJetEnergy);
    m_inChain -> SetBranchAddress("JetPt",      &m_jetPt,      &m_brJetPt);
    m_inChain -> SetBranchAddress("JetEta",     &m_jetEta,     &m_brJetEta);
    m_inChain -> SetBranchAddress("JetPhi",     &m_jetPhi,     &m_brJetPhi);
    m_inChain -> SetBranchAddress("JetArea",    &m_jetArea,    &m_brJetArea);
    m_inChain -> SetBranchAddress("CstZ",       &m_cstZ,       &m_brCstZ);
    m_inChain -> SetBranchAddress("CstDr",      &m_cstDr,      &m_brCstDr);
    m_inChain -> SetBranchAddress("CstEnergy",  &m_cstEnergy,  &m_brCstEnergy);
    m_inChain -> SetBranchAddress("CstJt",      &m_cstJt,      &m_brCstJt);
    m_inChain -> SetBranchAddress("CstEta",     &m_cstEta,     &m_brCstEta);
    m_inChain -> SetBranchAddress("CstPhi",     &m_cstPhi,     &m_brCstPhi);

    // announce tree setting
    if (m_inStandaloneMode) PrintMessage(2);
    return;

  }  // end 'InitializeTree()'



  void SEnergyCorrelator::InitializeHists() {

    // print debug statement
    if (m_inDebugMode) PrintDebug(5);

    for (size_t iPtBin = 0; iPtBin < m_nBinsJetPt; iPtBin++) {
      TH1D* hInitialVarDrAxis   = NULL;
      TH1D* hInitialErrDrAxis   = NULL;
      TH1D* hInitialVarLnDrAxis = NULL;
      TH1D* hInitialErrLnDrAxis = NULL;
      m_outHistVarDrAxis.push_back(hInitialVarDrAxis);
      m_outHistVarLnDrAxis.push_back(hInitialVarLnDrAxis);
      m_outHistErrDrAxis.push_back(hInitialErrDrAxis);
      m_outHistErrLnDrAxis.push_back(hInitialErrLnDrAxis);
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
             << "      output = " << m_outFileName.data() << "\n"
             << "      inputs = {"
             << endl;
        for (const string& inFileName : m_inFileNames) {
          cout << "        " << inFileName.data() << endl;
        }
        cout << "      }" << endl;
        break;
      case 2:
        cout << "    Initialized input chain:\n"
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
             << "      apply constituent cuts? = " << m_applyCstCuts   << "\n"
             << "      momentum range = ("         << m_momCstRange[0] << ", " << m_momCstRange[1] << ")\n"
             << "      dr range       = ("         << m_drCstRange[0]  << ", " << m_drCstRange[1]  << ")"
             << endl;
        break;
      case 13:
        cout << "    Finished event loop!" << endl;
        break;
      case 14:
        cout << "    Extracted output histograms from correlators." << endl;
        break;
      case 15:
        cout << "    Set which sub-events to use:" << endl;
        switch (m_subEvtOpt) {
          case 1:
            cout << "      Option " << m_subEvtOpt << ": use only signal event" << endl;
            break;
          case 2:
            cout << "      Option " << m_subEvtOpt << ": use only background events" << endl;
            break;
          case 3:
            cout << "      Option " << m_subEvtOpt << ": use only primary background event" << endl;
            break;
          case 4:
            cout << "      Option " << m_subEvtOpt << ": use only pileup events" << endl;
            break;
          case 5:
            cout << "      Option " << m_subEvtOpt << ": use events only with these embedding IDs: ";
            for (size_t iEvtToUse = 0; iEvtToUse < m_subEvtsToUse.size(); iEvtToUse++) {
              cout << m_subEvtsToUse[iEvtToUse];
              if ((iEvtToUse + 1) < m_subEvtsToUse.size()) {
                cout << ", ";
              } else {
                cout << endl;
              }
            }  // end sub-event id loop
            break;
          default:
            cout << "     Option " << m_subEvtOpt << ": use everything (check what you entered)" << endl;
            break;
        }
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
      case 32:
        cout << "SEnergyCorrelator::SetSubEventsToUse(uint16_t, vector<int>) setting sub-events to use..." << endl;
        break;
      case 33:
        cout << "SEnergyCorrelator::CheckIfSubEvtGood(int) checking if sub-event is good..." << endl;
        break;
    }
    return;

  }  // end 'PrintDebug(uint32_t)'



  void SEnergyCorrelator::PrintError(const uint32_t code, const size_t nDrBinEdges, const size_t iDrBin, const string sInFileName) {

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
          cerr << "SEnergyCorrelator::OpenInputFiles() PANIC: couldn't create input TChain! Aborting" << endl;
        } else {
          cerr << "PANIC: couldn't create input TChain! Aborting!" << endl;
        }
        break;
      case 7:
        if (m_inComplexMode) {
          cerr << "SEnergyCorrelator::OpenInputFiles() PANIC: couldn't grab tree \"" << m_inTreeName << "\" from file \"" << sInFileName << "\"! Aborting!" << endl;
        } else {
          cerr << "PANIC: couldn't grab tree \"" << m_inTreeName << "\" from file \"" << sInFileName << "\"! Aborting!" << endl;
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
          cerr << "WARNING: dR bin #" << iDrBin << " with variance has a NAN as content or error..." << endl;
        }
        break;
      case 14:
        if (m_inStandaloneMode) {
          cerr << "WARNING: dR bin #" << iDrBin << " with statistical error has a NAN as content or error..." << endl;
        }
        break;
      case 15:
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
    if (!m_inChain) {
      entryStatus = 0;
    } else {
      entryStatus = m_inChain -> GetEntry(entry);
    }
    return entryStatus;

  }  // end 'GetEntry(uint64_t)'



  int64_t SEnergyCorrelator::LoadTree(const uint64_t entry) {

    // print debugging statemet
    if (m_inDebugMode && (m_verbosity > 5)) PrintDebug(17);

    // check for tree & load
    int     treeNumber(-1);
    int64_t treeStatus(-1);
    if (!m_inChain) {
      treeStatus = -5;
    } else {
      treeNumber = m_inChain -> GetTreeNumber();
      treeStatus = m_inChain -> LoadTree(entry);
    }

    // update current tree number if need be
    const bool isTreeStatusGood = (treeStatus >= 0);
    const bool isNotCurrentTree = (treeNumber != m_fCurrent);
    if (isTreeStatusGood && isNotCurrentTree) {
      m_fCurrent = m_inChain -> GetTreeNumber();
    }
    return treeStatus;

  }  // end 'LoadTree(uint64_t)'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------
