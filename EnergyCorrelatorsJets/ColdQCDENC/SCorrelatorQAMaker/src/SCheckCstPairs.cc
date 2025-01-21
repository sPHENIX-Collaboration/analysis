// ----------------------------------------------------------------------------
// 'SCheckCstPairs.cc'
// Derek Anderson
// 03.10.2024
//
// SCorrelatorQAMaker plugin to iterate through all pairs of constituents in
// an event and fill tuples/histograms comparing them.
//
// This is similar to the `SCheckTrackPairs` plugin, which specifically looks
// at pairs of tracks off the node tree. This plugin compares constituents
// of any type off the correlator jet tree.
// ----------------------------------------------------------------------------

#define SCORRELATORQAMAKER_SCHECKCSTPAIRS_CC

// plugin definition
#include "SCheckCstPairs.h"

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // SCheckCstPairs public methods --------------------------------------------

  void SCheckCstPairs::Init() {

    // announce start
    cout << "\n Starting constituent pair check!" << endl;

    // initialize output
    InitOutput();

    // run internal routines
    InitInput();
    InitTree();
    InitHists();
    return;

  }  // end 'Init()'



  void SCheckCstPairs::Analyze() {

    DoDoubleCstLoop();
    return;

  }  // end 'Analyze()'



  void SCheckCstPairs::End() {

    // run internal routines
    SaveOutput();
    CloseInput();

     // close output
    CloseOutput();

    // announce end and exit
    cout << "  Finished constituent-pair check!\n" << endl;
    return;

  }  // end 'End()'



  // SCheckCstPairs internal methods ------------------------------------------

  void SCheckCstPairs::InitInput() {


    // open input files
    m_fInput = new TFile(m_config.inFileName.data(), "read");
    if (!m_fInput) {
      cerr << "PANIC: couldn't open input file:\n"
           << "       \"" << m_config.inFileName << "\"\n"
           << endl;
      assert(m_fInput);
    }
    cout << "    Opened input file:\n"
         << "      \"" << m_config.inFileName << "\""
         << endl;

    // grab input tree
    m_cInput = (TChain*) m_fInput -> Get(m_config.inChainName.data());
    if (!m_tInput) {
      cerr << "PANIC: couldn't grab input chain \"" << m_config.inChainName << "\"!\n" << endl;
      assert(m_cInput);
    }
    cout << "    Grabbed input chain \"" << m_config.inChainName << "\"." << endl;

    // exit routine
    return;

  }  // end 'InitInput()'



  void SCheckCstPairs::InitTree() {

    // set up input chain
    m_fCurrent = -1;
    m_cInput -> SetMakeClass(1);

    // set truth vs. reco branch addresses
    if (m_config.isInChainTruth) {
      m_cInput -> SetBranchAddress("Parton3_ID",   &m_partonID.first,    &m_brPartonID.first);
      m_cInput -> SetBranchAddress("Parton4_ID",   &m_partonID.second,   &m_brPartonID.second);
      m_cInput -> SetBranchAddress("Parton3_MomX", &m_partonMomX.first,  &m_brPartonMomX.first);
      m_cInput -> SetBranchAddress("Parton3_MomY", &m_partonMomY.first,  &m_brPartonMomY.first);
      m_cInput -> SetBranchAddress("Parton3_MomZ", &m_partonMomZ.first,  &m_brPartonMomZ.first);
      m_cInput -> SetBranchAddress("Parton4_MomX", &m_partonMomX.second, &m_brPartonMomX.second);
      m_cInput -> SetBranchAddress("Parton4_MomY", &m_partonMomY.second, &m_brPartonMomY.second);
      m_cInput -> SetBranchAddress("Parton4_MomZ", &m_partonMomZ.second, &m_brPartonMomZ.second);
      m_cInput -> SetBranchAddress("EvtSumParEne", &m_evtSumPar,         &m_brEvtSumPar);
      m_cInput -> SetBranchAddress("CstID",        &m_cstID,             &m_brCstID);
      m_cInput -> SetBranchAddress("CstEmbedID",   &m_cstEmbedID,        &m_brCstEmbedID);
    } else {
      m_cInput -> SetBranchAddress("EvtNumTrks",    &m_evtNumTrks, &m_brEvtNumTrks);
      m_cInput -> SetBranchAddress("EvtSumECalEne", &m_evtSumECal, &m_brEvtSumECal);
      m_cInput -> SetBranchAddress("EvtSumHCalEne", &m_evtSumHCal, &m_brEvtSumHCal);
      m_cInput -> SetBranchAddress("CstMatchID",    &m_cstMatchID, &m_brCstMatchID);
    }

    // set generic branch addresses
    m_cInput -> SetBranchAddress("EvtVtxX",    &m_evtVtxX,    &m_brEvtVtxX);
    m_cInput -> SetBranchAddress("EvtVtxY",    &m_evtVtxY,    &m_brEvtVtxY);
    m_cInput -> SetBranchAddress("EvtVtxZ",    &m_evtVtxZ,    &m_brEvtVtxZ);
    m_cInput -> SetBranchAddress("EvtNumJets", &m_evtNumJets, &m_brEvtNumJets);
    m_cInput -> SetBranchAddress("JetNumCst",  &m_jetNumCst,  &m_brJetNumCst);
    m_cInput -> SetBranchAddress("JetID",      &m_jetID,      &m_brJetID);
    m_cInput -> SetBranchAddress("JetEnergy",  &m_jetEnergy,  &m_brJetEnergy);
    m_cInput -> SetBranchAddress("JetPt",      &m_jetPt,      &m_brJetPt);
    m_cInput -> SetBranchAddress("JetEta",     &m_jetEta,     &m_brJetEta);
    m_cInput -> SetBranchAddress("JetPhi",     &m_jetPhi,     &m_brJetPhi);
    m_cInput -> SetBranchAddress("JetArea",    &m_jetArea,    &m_brJetArea);
    m_cInput -> SetBranchAddress("CstZ",       &m_cstZ,       &m_brCstZ);
    m_cInput -> SetBranchAddress("CstDr",      &m_cstDr,      &m_brCstDr);
    m_cInput -> SetBranchAddress("CstEnergy",  &m_cstEnergy,  &m_brCstEnergy);
    m_cInput -> SetBranchAddress("CstJt",      &m_cstPt,      &m_brCstPt);
    m_cInput -> SetBranchAddress("CstEta",     &m_cstEta,     &m_brCstEta);
    m_cInput -> SetBranchAddress("CstPhi",     &m_cstPhi,     &m_brCstPhi);
    cout << "    Initialized input chain." << endl;

    // exit routine
    return;

  }  // end 'InitInput()'



  void SCheckCstPairs::InitHists() {

    // FIXME remove dependence on EECLongestSide
    vector<double> drBinEdges  = m_eecLongSide[0] -> bin_edges();
    size_t         nDrBinEdges = drBinEdges.size();
 
    double drBinEdgeArray[nDrBinEdges];
    for (size_t iDrEdge = 0; iDrEdge < nDrBinEdges; iDrEdge++) {
      drBinEdgeArray[iDrEdge] = drBinEdges.at(iDrEdge);
    }
    hCstPtOneVsDr      = new TH2D("hCstPtOneVsDr",      "", m_nBinsDr, drBinEdgeArray, 200,  0.,   100.);
    hCstPtTwoVsDr      = new TH2D("hCstPtTwoVsDr",      "", m_nBinsDr, drBinEdgeArray, 200,  0.,   100.);
    hCstPtFracVsDr     = new TH2D("hCstPtFracVsDr",     "", m_nBinsDr, drBinEdgeArray, 500,  0.,   5.);
    hCstPhiOneVsDr     = new TH2D("hCstPhiOneVsDr",     "", m_nBinsDr, drBinEdgeArray, 360, -3.15, 3.15);;
    hCstPhiTwoVsDr     = new TH2D("hCstPhiTwoVsDr",     "", m_nBinsDr, drBinEdgeArray, 360, -3.15, 3.15);
    hCstEtaOneVsDr     = new TH2D("hCstEtaOneVsDr",     "", m_nBinsDr, drBinEdgeArray, 400, -2.,   2.);
    hCstEtaTwoVsDr     = new TH2D("hCstEtaTwoVsDr",     "", m_nBinsDr, drBinEdgeArray, 400, -2.,   2.);
    hDeltaPhiOneVsDr   = new TH2D("hDeltaPhiOneVsDr",   "", m_nBinsDr, drBinEdgeArray, 720, -6.30, 6.30);
    hDeltaPhiTwoVsDr   = new TH2D("hDeltaPhiTwoVsDr",   "", m_nBinsDr, drBinEdgeArray, 720, -6.30, 6.30);
    hDeltaEtaOneVsDr   = new TH2D("hDeltaEtaOneVsDr",   "", m_nBinsDr, drBinEdgeArray, 800, -4.,   4.);
    hDeltaEtaTwoVsDr   = new TH2D("hDeltaEtaTwoVsDr",   "", m_nBinsDr, drBinEdgeArray, 800, -4.,   4.);
    hJetPtFracOneVsDr  = new TH2D("hJetPtFracOneVsDr",  "", m_nBinsDr, drBinEdgeArray, 500, 0.,    5.);
    hJetPtFracTwoVsDr  = new TH2D("hJetPtFracTwoVsDr",  "", m_nBinsDr, drBinEdgeArray, 500, 0.,    5.);
    hCstPairWeightVsDr = new TH2D("hCstPairWeightVsDr", "", m_nBinsDr, drBinEdgeArray, 100,  0.,   1.);
    cout << "    Initialized histograms." << endl;

    // exit routine
    return;

  }  // end 'InitHists()'



  void SCheckCstPairs::SaveOutput() {

    m_outDir           -> cd();
    hCstPtOneVsDr      -> Write();
    hCstPtTwoVsDr      -> Write();
    hCstPtFracVsDr     -> Write();
    hCstPhiOneVsDr     -> Write();
    hCstPhiTwoVsDr     -> Write();
    hCstEtaOneVsDr     -> Write();
    hCstEtaTwoVsDr     -> Write();
    hDeltaPhiOneVsDr   -> Write();
    hDeltaPhiTwoVsDr   -> Write();
    hDeltaEtaOneVsDr   -> Write();
    hDeltaEtaTwoVsDr   -> Write();
    hJetPtFracOneVsDr  -> Write();
    hJetPtFracTwoVsDr  -> Write();
    hCstPairWeightVsDr -> Write();
    return;

  }  // end 'SaveOutput()'




  void SCheckCstPairs::CloseInput() {

    m_fInput -> cd();
    m_fInput -> Close();
    return;

  }  // end 'CloseInput()'



  void SCheckCstPairs::DoDoubleCstLoop() {

    // announce start of event loop
    const uint64_t nEvts = m_cInput -> GetEntriesFast();

    // event loop
    uint64_t nBytes = 0;
    for (uint64_t iEvt = 0; iEvt < nEvts; iEvt++) {

      const uint64_t entry = Interfaces::LoadTree(m_cInput, iEvt, m_fCurrent);
      if (entry < 0) break;

      const uint64_t bytes = Interfaces::GetEntry(m_cInput, iEvt);
      if (bytes < 0) {
        break;
      } else {
        nBytes += bytes;
      }

      // announce progress
      const int64_t iProg = iEvt + 1;
      if (iProg == nEvents) {
        cout << "      Processing entry " << iEvt << "/" << nEvts << "..." << endl;
      } else {
        cout << "      Processing entry " << iEvt << "/" << nEvts << "...\r" << flush;
      }

      // jet loop
      uint64_t nJets = (int) m_evtNumJets;
      for (uint64_t iJet = 0; iJet < nJets; iJet++) {

        // clear vector for correlator
        m_jetCstVector.clear();

        // get jet info
        const uint64_t nCsts  = m_jetNumCst -> at(iJet);
        const double   ptJet  = m_jetPt     -> at(iJet);
        const double   etaJet = m_jetEta    -> at(iJet);

        // select jet pt bin & apply jet cuts
        const uint32_t iPtJetBin = GetJetPtBin(ptJet);
        const bool     isGoodJet = ApplyJetCuts(ptJet, etaJet);
        if (!isGoodJet) continue;

        // constituent loop
        for (uint64_t iCst = 0; iCst < nCsts; iCst++) {

          // get cst info
          const double drCst  = (m_cstDr  -> at(iJet)).at(iCst);
          const double etaCst = (m_cstEta -> at(iJet)).at(iCst);
          const double phiCst = (m_cstPhi -> at(iJet)).at(iCst);
          const double ptCst  = (m_cstPt  -> at(iJet)).at(iCst);

          // for weird cst check
          for (uint64_t jCst = 0; jCst < nCsts; jCst++) {

            // skip over the same cst
            if (jCst == iCst) continue;

            // get cst info
            const double etaCstB = (m_cstEta -> at(iJet)).at(jCst);
            const double phiCstB = (m_cstPhi -> at(iJet)).at(jCst);
            const double ptCstB  = (m_cstPt  -> at(iJet)).at(jCst);

            // calculate separation and pt-weight
            const double dhCstAB  = (etaCst - etaCstB);
            const double dfCstAB  = (phiCst - phiCstB);
            const double drCstAB  = sqrt((dhCstAB * dhCstAB) + (dfCstAB * dfCstAB));
            const double ptFrac   = ptCst / ptCstB;
            const double ztJetA   = ptCst / ptJet;
            const double ztJetB   = ptCstB / ptJet;
            const double ptWeight = (ptCst * ptCstB) / (ptJet * ptJet);
            hCstPtOneVsDr      -> Fill(drCstAB, ptCst);
            hCstPtTwoVsDr      -> Fill(drCstAB, ptCstB);
            hCstPtFracVsDr     -> Fill(drCstAB, ptFrac);
            hCstPhiOneVsDr     -> Fill(drCstAB, phiCst);
            hCstPhiTwoVsDr     -> Fill(drCstAB, phiCstB);
            hCstEtaOneVsDr     -> Fill(drCstAB, etaCst);
            hCstEtaTwoVsDr     -> Fill(drCstAB, etaCstB);
            hDeltaPhiOneVsDr   -> Fill(drCstAB, dfCstAB);
            hDeltaPhiTwoVsDr   -> Fill(drCstAB, dfCstAB);
            hDeltaEtaOneVsDr   -> Fill(drCstAB, dhCstAB);
            hDeltaEtaTwoVsDr   -> Fill(drCstAB, dhCstAB);
            hJetPtFracOneVsDr  -> Fill(drCstAB, ztJetA);
            hJetPtFracTwoVsDr  -> Fill(drCstAB, ztJetB);
            hCstPairWeightVsDr -> Fill(drCstAB, ptWeight);
          }  // end 2nd cst loop
        }  // end 1st cst loop
      }  // end jet loop
    }  // end event loop
    cout << "    Event loop finished!" << endl;

    // exit routine
    return;

  }  // end 'DoDoubleCstLoop()'



  bool SCheckCstPairs::IsGoodJet() {

    /* TODO fill in */
    return true;

  }  // end 'IsGoodJet'



  bool SCheckCstPairs::IsGoodCst() {

    /* TODO fill in */
    return true;

  }  // end 'IsGoodCst()'

}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------
