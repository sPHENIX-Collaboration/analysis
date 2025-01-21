/// ---------------------------------------------------------------------------
/*! \file    SEnergyCorrelator.sys.h
 *  \authors Derek Anderson, Alex Clarke
 *  \date    01.20.2023
 *
 *  A module to run ENC calculations in the sPHENIX
 *  software stack for the Cold QCD EEC analysis.
 */
/// ---------------------------------------------------------------------------

#pragma once

// make common namespaces implicit
using namespace std;
using namespace fastjet;



namespace SColdQcdCorrelatorAnalysis {

  // system methods ===========================================================

  // --------------------------------------------------------------------------
  //! Initialize class members
  // --------------------------------------------------------------------------
  void SEnergyCorrelator::InitializeMembers() {

    // print debug statement
    if (m_config.isDebugOn) PrintDebug(0);

    // clear calculation vectors
    m_jetCalcVec.clear();
    m_cstCalcVec.clear();
    m_eecLongSide.clear();    

    // clear output histograms
    m_outPackageHistVarDrAxis.clear();
    m_outPackageHistErrDrAxis.clear();
    m_outPackageHistVarLnDrAxis.clear();
    m_outPackageHistErrLnDrAxis.clear();
    m_outManualHistErrDrAxis.clear();
    m_outProjE3C.clear();
    m_outE3C.clear();
    m_outGlobalHistDPhiAxis.clear();
    m_outGlobalHistCosDFAxis.clear();
    return;

  }  // end 'InitializeMembers()'



  // --------------------------------------------------------------------------
  //! Initialize input tree
  // --------------------------------------------------------------------------
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

    // set input tree
    m_interface.SetChainAddresses(m_inChain, m_config.isInTreeTruth);

    // announce tree setting
    PrintMessage(2);
    return;

  }  // end 'InitializeTree()'



  // --------------------------------------------------------------------------
  //! Initialize output histograms
  // --------------------------------------------------------------------------
  /* FIXME move this to a dedicated histogram collection */
  void SEnergyCorrelator::InitializeHists() {

    // print debug statement
    if (m_config.isDebugOn) PrintDebug(5);

    // extract log binning
    //   - FIXME manually code and move to utilities
    vector<double> drBinEdges  = m_eecLongSide[0] -> bin_edges();
    size_t         nDrBinEdges = drBinEdges.size();
 
    // fill bin edges
    double drBinEdgeArray[nDrBinEdges];
    for (size_t iDrEdge = 0; iDrEdge < nDrBinEdges; iDrEdge++) {
      drBinEdgeArray[iDrEdge] = drBinEdges.at(iDrEdge);
    }

    // define bins
    //   - FIXME move to a bins database
    map<string, tuple<size_t, float, float>> bins = {
      {"xi",    make_tuple(100, 0.0,  1.0)},
      {"phi",   make_tuple(100, 0.0,  TMath::Pi()/2.0)},
      {"dphi",  make_tuple(314, 0.0,  TMath::Pi())},
      {"cosdf", make_tuple(24,  -1.0, 1.0)}
    };

    // loop over pt bins
    for (size_t iPtBin = 0; iPtBin < m_config.ptJetBins.size(); iPtBin++) {

      // reserve space for package histograms
      TH1D* hInitialVarDrAxis   = NULL;
      TH1D* hInitialErrDrAxis   = NULL;
      TH1D* hInitialVarLnDrAxis = NULL;
      TH1D* hInitialErrLnDrAxis = NULL;
      m_outPackageHistVarDrAxis.push_back( hInitialVarDrAxis );
      m_outPackageHistVarLnDrAxis.push_back( hInitialVarLnDrAxis );
      m_outPackageHistErrDrAxis.push_back( hInitialErrDrAxis );
      m_outPackageHistErrLnDrAxis.push_back (hInitialErrLnDrAxis );

      // if doing manual calculation, add relevent histograms
      if (!m_config.doManualCalc) continue;

      // create names
      string projName("hManualProjE3C_ptJet");
      string eecName("hManualCorrelatorErrorDrAxis_ptJet");
      string e3cBase("hManualE3C_ptJet");
      projName += to_string( floor(m_config.ptJetBins[iPtBin].first) );
      eecName  += to_string( floor(m_config.ptJetBins[iPtBin].first) );
      e3cBase  += to_string( floor(m_config.ptJetBins[iPtBin].first) );

      // create manual two-point histograms
      m_outManualHistErrDrAxis.push_back(
        new TH1D(
          eecName.data(),
          "",
          m_config.nBinsDr,
          drBinEdgeArray
        )
      );
      m_outManualHistErrDrAxis.back() -> Sumw2();

      // add manual 3-point if needed
      if (!m_config.doThreePoint) continue;

      // create manual projected 3-point histogram
      m_outProjE3C.push_back(new TH1D(projName.data(), "", m_config.nBinsDr, drBinEdgeArray));
      m_outProjE3C.back() -> Sumw2();

      // create shape-dependent 3-point histograms
      vector<TH2D*> E3C_tmp;
      for (size_t jRLBin = 0; jRLBin < m_config.rlBins.size(); jRLBin++) {
        const string e3cName = e3cBase + "_RLBin" + to_string(jRLBin);
        E3C_tmp.push_back(
          new TH2D(
            e3cName.data(),
            "",
            get<0>(bins["xi"]), get<1>(bins["xi"]), get<2>(bins["xi"]),
            get<0>(bins["phi"]), get<1>(bins["phi"]), get<2>(bins["phi"])
          )
        );
      }
      m_outE3C.push_back(E3C_tmp);

    }  // end pt bin loop

    // create global names
    const string dphiAxisBase("hTEECDPhiAxis_htEvt");
    const string cosdfAxisBase("hTEERCosDFAxis_htEvt");

    // loop over ht bins
    for (size_t iHtBin = 0; iHtBin < m_config.htEvtBins.size(); iHtBin++) {

      // construct names
      const string dphiAxisName  = dphiAxisBase + to_string(iHtBin);
      const string cosdfAxisName = cosdfAxisBase + to_string(iHtBin);

      // construct histograms
      m_outGlobalHistDPhiAxis.push_back(
        new TH1D(
          dphiAxisName.data(),
          "",
          get<0>(bins["dphi"]), get<1>(bins["dphi"]), get<2>(bins["dphi"])
        )
      );
      m_outGlobalHistCosDFAxis.push_back(
        new TH1D(
          cosdfAxisName.data(),
          "",
          get<0>(bins["cosdf"]), get<1>(bins["cosdf"]), get<2>(bins["cosdf"])
        )
      );
    }  // end ht bin loop

    // announce histogram initialization
    PrintMessage(3);
    return;

  }  // end 'InitializeHists()'



  // --------------------------------------------------------------------------
  //! Initialize ENC package
  // --------------------------------------------------------------------------
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
    PrintMessage(4);
    return;

  }  // end 'InitializeCorrs()'



  // --------------------------------------------------------------------------
  //! Initialize smearing functions and RNGs for smearing
  // --------------------------------------------------------------------------
  void SEnergyCorrelator::InitializeSmearing() {

    // print debug statement
    if (m_config.isDebugOn) PrintDebug(43);

    // grab date & time
    TDatime* time = new TDatime();

    // and seed RNG w/ date * time
    m_rando = new TRandom2(
      (time -> GetDate()) * (time -> GetTime())
    );

    // announce smearing initialization
    PrintMessage(16);
    return;

  }  // end 'InitializeSmearing()'



  // --------------------------------------------------------------------------
  //! Print messages
  // --------------------------------------------------------------------------
  void SEnergyCorrelator::PrintMessage(
    const uint32_t code,
    const uint64_t nEvts,
    const uint64_t event
  ) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 5)) PrintDebug(22);

    // select relevant message
    switch (code) {

      case 0:
        cout << "\n  Running standalone correlator calculation...\n"
             << "    Set name & modes:\n"
             << "      module name = " << m_config.moduleName.data() << "\n"
             << "      debug mode? = " << m_config.isDebugOn         << "\n"
             << "      batch mode? = " << m_config.isBatchOn
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

      case 16:
        cout << "    Initialized RNG (for smearing, if need be)." << endl;
        break;

      default:
        cerr << "WARNING: unknown message code " << code << "!" << endl;
        break;

    }  // end switch (code)
    return;

  }  // end 'PrintMessage(uint32_t)'



  // --------------------------------------------------------------------------
  //! Print debugging statement
  // --------------------------------------------------------------------------
  void SEnergyCorrelator::PrintDebug(const uint32_t code) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 7)) {
      cout << "SEnergyCorrelator::PrintDebug(uint32_t) printing a debugging statement..." << endl;
    }

    // select relevant message
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
        cout << "SEnergyCorrelator::RunCalculations() looping over jets and calculating correlators..." << endl;
        break;

      case 32:
        cout << "SEnergyCorrelator::SetSubEventsToUse(uint16_t, vector<int>) setting sub-events to use..." << endl;
        break;

      case 33:
        cout << "SEnergyCorrelator::CheckIfSubEvtGood(int) checking if sub-event is good..." << endl;
        break;

      case 34:
        cout << "SEnergyCorrelator::DoLocalCalcWithPackage() running local calculation with EEC package..." << endl;
        break;

      case 35:
        cout << "SEnergyCorrelator::SmearJetMomentum(ROOT::Math::PtEtaPhiEVector&) smearing jet momentum..." << endl;
        break;

      case 36:
        cout << "SEnergyCorrelator::SmearCstMomentum(ROOT::Math::PtEtaPhiEVector&) smearing constituent momentum..." << endl;
        break;

      case 37:
        cout << "SEnergyCorrelator::SurvivesEfficiency(double) checking if value survives efficiency..." << endl;
        break;

      case 38:
        cout << "SEnergyCorrelator::DoGlobalCalcManual(double) running global calculation..." << endl;
        break;

      case 39:
        cout << "SEnergyCorrelator::DoLocalCalcManual(ROOT::Math::PtEtaPhiEVector&) running local calculation manually..." << endl;
        break;

      case 40:
        cout << "SEnergyCorrelator::GetWeight(ROOT::Math::PtEtaPhiEVector&, int, optional<ROOT::Math::PtEtaPhiEVector>) getting weight..." << endl;
        break;

      case 41:
        cout << "SEnergyCorrelator::GetRM(tuple<double, double, double>&) determining RM..." << endl;
        break;

      case 42:
        cout << "SEnergyCorrelator::GetET(TVector3&, TVector3&) calculating et..." << endl;
        break;

      case 43:
        cout << "SEnergyCorrelator::InitializeSmearing() initializing smearing utilities..." << endl;
        break;

      case 44:
        cout << "SEnergyCorrelator::SEnergyCorrelator(SEnergyCorrelatorConfig&) calling ctor..." << endl;
        break;

      default:
        cerr << "WARNING: unknown debugging code " << code << "!" << endl;
        break;

    }  // end switch (code)
    return;

  }  // end 'PrintDebug(uint32_t)'



  // --------------------------------------------------------------------------
  //!  Print error message
  // --------------------------------------------------------------------------
  void SEnergyCorrelator::PrintError(
    const uint32_t code,
    const size_t nDrBinEdges,
    const size_t iDrBin,
    const string sInFileName
  ) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 5)) PrintDebug(23);

    // select relevant message
    switch (code) {

      case 6:
        cerr << "PANIC: couldn't create input TChain! Aborting!" << endl;
        break;

      case 7:
        cerr << "PANIC: couldn't grab tree \"" << m_config.inTreeName << "\" from file \"" << sInFileName << "\"! Aborting!" << endl;
        break;

      case 10:
        cerr << "PANIC: no TTree! Aborting!" << endl;
        break;

      case 11:
        cerr << "PANIC: couldn't open output file! Aborting!" << endl;
        break;

      case 12:
        cerr << "PANIC: number of dR bin edges is no good! Aborting!\n"
             << "       nDrBinEdges = " << nDrBinEdges << ", nDrBins = " << m_config.nBinsDr
             << endl;
        break;

      case 13:
        cerr << "WARNING: dR bin #" << iDrBin << " with variance has a NAN as content or error..." << endl;
        break;

      case 14:
        cerr << "WARNING: dR bin #" << iDrBin << " with statistical error has a NAN as content or error..." << endl;
        break;

      default:
        cerr << "WARNING: unknown error code " << code << "!" << endl;
        break;

    }  // end switch (code)
    return;

  }  // end 'PrintError(unint32_t)'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------
