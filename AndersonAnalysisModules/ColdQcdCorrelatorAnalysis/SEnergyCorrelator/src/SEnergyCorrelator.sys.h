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

// make common namespaces implicit
using namespace std;
using namespace fastjet;



namespace SColdQcdCorrelatorAnalysis {

  // system methods -----------------------------------------------------------

  void SEnergyCorrelator::InitializeMembers() {

    // print debug statement
    if (m_config.isDebugOn) PrintDebug(0);

    // clear calculation vectors
    m_jetCstVector.clear();
    m_eecLongSide.clear();    

    // clear output histograms
    m_outPackageHistVarDrAxis.clear();
    m_outPackageHistErrDrAxis.clear();
    m_outPackageHistVarLnDrAxis.clear();
    m_outPackageHistErrLnDrAxis.clear();
    m_outManualHistErrDrAxis.clear();
    

    return;

  }  // end 'InitializeMembers()'



  void SEnergyCorrelator::InitializeTree() {

    // print debug statement
    if (m_config.isDebugOn) PrintDebug(4);

    // check for tree
    if (!m_inChain) {
      PrintError(10);
      assert(m_inChain);
    }
    m_fCurrent = -1;
    m_inChain -> SetMakeClass(1);

    // set truth vs. reco branch addresses
    if (m_config.isLegacyInput) {
      m_legacy.SetChainAddresses(m_inChain, m_config.isInTreeTruth);
    }

    // announce tree setting
    if (m_config.isStandalone) PrintMessage(2);
    return;

  }  // end 'InitializeTree()'



  void SEnergyCorrelator::InitializeHists() {

    // print debug statement
    if (m_config.isDebugOn) PrintDebug(5);

    vector<double> drBinEdges  = m_eecLongSide[0] -> bin_edges();
    size_t         nDrBinEdges = drBinEdges.size();
 
    double drBinEdgeArray[nDrBinEdges];
    for (size_t iDrEdge = 0; iDrEdge < nDrBinEdges; iDrEdge++) {
      drBinEdgeArray[iDrEdge] = drBinEdges.at(iDrEdge);
    }

    for (size_t iPtBin = 0; iPtBin < m_config.ptJetBins.size(); iPtBin++) {
      TH1D* hInitialVarDrAxis   = NULL;
      TH1D* hInitialErrDrAxis   = NULL;
      TH1D* hInitialVarLnDrAxis = NULL;
      TH1D* hInitialErrLnDrAxis = NULL;
      m_outPackageHistVarDrAxis.push_back(hInitialVarDrAxis);
      m_outPackageHistVarLnDrAxis.push_back(hInitialVarLnDrAxis);
      m_outPackageHistErrDrAxis.push_back(hInitialErrDrAxis);
      m_outPackageHistErrLnDrAxis.push_back(hInitialErrLnDrAxis);
      if(m_config.doManualCalc){      
	TString weightNameTH1("hManualCorrelatorErrorDrAxis_ptJet");
	weightNameTH1+=floor(m_config.ptJetBins[iPtBin].first);
	m_outManualHistErrDrAxis.push_back(new TH1D(weightNameTH1, "", m_config.nBinsDr, drBinEdgeArray));
	m_outManualHistErrDrAxis[iPtBin]->Sumw2();
      }
    }

    // announce histogram initialization
    if (m_config.isStandalone) PrintMessage(3);
    return;

  }  // end 'InitializeHists()'



  void SEnergyCorrelator::InitializeCorrs() {

    // print debug statement
    if (m_config.isDebugOn) PrintDebug(6);

    // initialize correlator for each jet pt bin
    for (size_t iPtBin = 0; iPtBin < m_config.ptJetBins.size(); iPtBin++) {
      m_eecLongSide.push_back(
        new contrib::eec::EECLongestSide<contrib::eec::hist::axis::log>(
          m_config.nPoints.at(0),  // TODO enable multiple n per calculation
          m_config.nBinsDr,
          {m_config.drBinRange.first, m_config.drBinRange.second}
        )
      );
    }

    // announce correlator initialization
    if (m_config.isStandalone) PrintMessage(4);
    return;

  }  // end 'InitializeCorrs()'



  void SEnergyCorrelator::PrintMessage(const uint32_t code, const uint64_t nEvts, const uint64_t event) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 5)) PrintDebug(22);

    switch (code) {
      case 0:
        cout << "\n  Running standalone correlator calculation...\n"
             << "    Set name & modes:\n"
             << "      module name      = " << m_config.moduleName.data() << "\n"
             << "      complex mode?    = " << !m_config.isStandalone     << "\n"
             << "      standalone mode? = " << m_config.isStandalone      << "\n"
             << "      debug mode?      = " << m_config.isDebugOn         << "\n"
             << "      batch mode?      = " << m_config.isBatchOn
             << endl;
        break;
      case 1:
        cout << "    Opened files:\n"
             << "      output = " << m_config.outFileName.data() << "\n"
             << "      inputs = {"
             << endl;
        for (const string& inFileName : m_config.inFileNames) {
          cout << "        " << inFileName.data() << endl;
        }
        cout << "      }" << endl;
        break;
      case 2:
        cout << "    Initialized input chain:\n"
             << "      tree name = " << m_config.inTreeName.data()
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
             << "      n-point = "       << m_config.nPoints.at(0)     << ", number of dR bins = " << m_config.nBinsDr           << "\n"
             << "      dR bin range = (" << m_config.drBinRange.first  << ", "                     << m_config.drBinRange.second << ")"
             << endl;
        break;
      case 6:
        cout << "    Set jet parameters:\n"
             << "      eta range = (" << m_config.jetAccept.first.GetEta() << ", " << m_config.jetAccept.second.GetEta() << ")\n"
             << "      pt range  = (" << m_config.jetAccept.first.GetPT()  << ", " << m_config.jetAccept.second.GetPT()  << ")\n"
             << "    Set pTjet bins:"
             << endl;
        for (uint32_t iPtBin = 0; iPtBin < m_config.ptJetBins.size(); iPtBin++) {
          cout << "      bin[" << iPtBin << "] = (" << m_config.ptJetBins.at(iPtBin).first << ", " << m_config.ptJetBins.at(iPtBin).second << ")" << endl;
        }
        break;
      case 7:
        cout << "    Beginning event loop: " << nEvts << " events to process..." << endl;
        break;
      case 8:
        if (m_config.isBatchOn) {
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
             << "      apply constituent cuts? = " << m_config.applyCstCuts   << "\n"
             << "      momentum range = ("         << m_config.cstAccept.first.GetPT() << ", " << m_config.cstAccept.second.GetPT() << ")\n"
             << "      dr range       = ("         << m_config.cstAccept.first.GetDR() << ", " << m_config.cstAccept.second.GetDR() << ")"
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
        switch (m_config.subEvtOpt) {
          case 1:
            cout << "      Option " << m_config.subEvtOpt << ": use only signal event" << endl;
            break;
          case 2:
            cout << "      Option " << m_config.subEvtOpt << ": use only background events" << endl;
            break;
          case 3:
            cout << "      Option " << m_config.subEvtOpt << ": use only primary background event" << endl;
            break;
          case 4:
            cout << "      Option " << m_config.subEvtOpt << ": use only pileup events" << endl;
            break;
          case 5:
            cout << "      Option " << m_config.subEvtOpt << ": use events only with these embedding IDs: ";
            for (size_t iEvtToUse = 0; iEvtToUse < m_config.subEvtsToUse.size(); iEvtToUse++) {
              cout << m_config.subEvtsToUse[iEvtToUse];
              if ((iEvtToUse + 1) < m_config.subEvtsToUse.size()) {
                cout << ", ";
              } else {
                cout << endl;
              }
            }  // end sub-event id loop
            break;
          default:
            cout << "     Option " << m_config.subEvtOpt << ": use everything (check what you entered)" << endl;
            break;
        }
        break;
    }
    return;

  }  // end 'PrintMessage(uint32_t)'


  void SEnergyCorrelator::PrintDebug(const uint32_t code) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 7)) {
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
        cout << "SEnergyCorrelator::SetCorrelatorParameters(uint32_t, uint64_t, pair<double, double>) setting correlator parameters..." << endl;
        break;
      case 20:
        cout << "SEnergyCorrelator::SetJetParameters(vector<pair<double, double>>, pair<double, double>) setting jet parameters..." << endl;
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
        cout << "SEnergyCorrelator::SetConstituentParameters(pair<double, double>, pair<double, double>) setting constituent parameters..." << endl;
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
        cout << "SEnergyCorrelator::DoLocalCalculation() looping over jets and calculating correlators..." << endl;
        break;
      case 32:
        cout << "SEnergyCorrelator::SetSubEventsToUse(uint16_t, vector<int>) setting sub-events to use..." << endl;
        break;
      case 33:
        cout << "SEnergyCorrelator::CheckIfSubEvtGood(int) checking if sub-event is good..." << endl;
        break;
      case 34:
        cout << "SEnergyCorrelator::DoLocalCalcWitPackagen() running calculation with EEC package..." << endl;
        break;
    }
    return;

  }  // end 'PrintDebug(uint32_t)'



  void SEnergyCorrelator::PrintError(const uint32_t code, const size_t nDrBinEdges, const size_t iDrBin, const string sInFileName) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 5)) PrintDebug(23);

    switch (code) {
      case 0:
        if (!m_config.isStandalone) {
          cerr << "SEnergyCorrelator::Init(PHCompositeNode*) PANIC: calling complex method in standalone mode! Aborting!" << endl;
        } else {
          cerr << "PANIC: calling complex method in standalone mode! Aborting!" << endl;
        }
        break;
      case 1:
        if (!m_config.isStandalone) {
          cerr << "SEnergyCorrelator::GrabInputNode() PANIC: couldn't grab node \"" << m_config.inNodeName << "\"! Aborting!" << endl;
        } else {
          cerr << "PANIC: couldn't grab node \"" << m_config.inNodeName << "\"! Aborting!" << endl;
        }
        break;
      case 2:
        if (!m_config.isStandalone) {
          cerr << "SEnergyCorrelator::GrabInputNode() PANIC: couldn't grab tree \"" << m_config.inTreeName << "\" from node \"" << m_config.inNodeName << "\"! Aborting!" << endl;
        } else {
          cerr << "PANIC: couldn't grab tree \"" << m_config.inTreeName << "\" from node \"" << m_config.inNodeName << "\"! Aborting!" << endl;
        }
        break;
      case 3:
        if (!m_config.isStandalone) {
          cerr << "SEnergyCorrelator::process_event(PHCompositeNode*) PANIC: calling complex method in standalone mode! Aborting!" << endl;
        } else {
          cerr << "PANIC: calling complex method in standalone mode! Aborting!" << endl;
        }
        break;
      case 4:
        if (!m_config.isStandalone) {
          cerr << "SEnergyCorrelator::End(PHCompositeNode*) PANIC: calling complex method in standalone mode! Aborting!" << endl;
        } else {
          cerr << "PANIC: calling complex method in standalone mode! Aborting!" << endl;
        }
        break;
      case 5:
        if (!m_config.isStandalone) {
          cerr << "SEnergyCorrelator::Init() PANIC: calling standalone method in complex mode! Aborting!" << endl;
        } else {
          cerr << "PANIC: calling standalone method in complex mode! Aborting!" << endl;
        }
        break;
      case 6:
        if (!m_config.isStandalone) {
          cerr << "SEnergyCorrelator::OpenInputFiles() PANIC: couldn't create input TChain! Aborting" << endl;
        } else {
          cerr << "PANIC: couldn't create input TChain! Aborting!" << endl;
        }
        break;
      case 7:
        if (!m_config.isStandalone) {
          cerr << "SEnergyCorrelator::OpenInputFiles() PANIC: couldn't grab tree \"" << m_config.inTreeName << "\" from file \"" << sInFileName << "\"! Aborting!" << endl;
        } else {
          cerr << "PANIC: couldn't grab tree \"" << m_config.inTreeName << "\" from file \"" << sInFileName << "\"! Aborting!" << endl;
        }
        break;
      case 8:
        if (!m_config.isStandalone) {
          cerr << "SEnergyCorrelator::Analyze() PANIC: calling standalone method in complex mode! Aborting!" << endl;
        } else {
          cerr << "PANIC: calling standalone method in complex mode! Aborting!" << endl;
        }
        break;
      case 9:
        if (!m_config.isStandalone) {
          cerr << "SEnergyCorrelator::End() PANIC: calling standalone method in complex mode! Aborting!" << endl;
        } else {
          cerr << "PANIC: calling standalone method in complex mode! Aborting!" << endl;
        }
        break;
      case 10:
        if (!m_config.isStandalone) {
          cerr << "SEnergyCorrelator::InitializeTree() PANIC: no TTree! Aborting!" << endl;
        } else {
          cerr << "PANIC: no TTree! Aborting!" << endl;
        }
        break;
      case 11:
        if (!m_config.isStandalone) {
          cerr << "SEnergyCorrelator::OpenOutputFile() PANIC: couldn't open output file! Aborting!" << endl;
        } else {
          cerr << "PANIC: couldn't open output file! Aborting!" << endl;
        }
        break;
      case 12:
        if (!m_config.isStandalone) {
          cerr << "SEnergyCorrelator::ExtraHistsFromCorr() PANIC: number of dR bin edges is no good! Aborting!" << endl;
        } else {
          cerr << "PANIC: number of dR bin edges is no good! Aborting!\n"
               << "       nDrBinEdges = " << nDrBinEdges << ", nDrBins = " << m_config.nBinsDr
               << endl;
        }
        break;
      case 13:
        if (m_config.isStandalone) {
          cerr << "WARNING: dR bin #" << iDrBin << " with variance has a NAN as content or error..." << endl;
        }
        break;
      case 14:
        if (m_config.isStandalone) {
          cerr << "WARNING: dR bin #" << iDrBin << " with statistical error has a NAN as content or error..." << endl;
        }
        break;
      case 15:
        if (!m_config.isStandalone) {
          cerr << "SEnergyCorrelatorFile::End() PANIC: calling standalone method in complex mode! Aborting!" << endl;
        } else {
          cerr << "PANIC: calling standalone method in complex mode! Aborting!" << endl;
        }
        break;
    }
    return;

  }  // end 'PrintError(unint32_t)'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------
