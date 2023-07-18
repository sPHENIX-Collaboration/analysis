// ----------------------------------------------------------------------------
// 'SEnergyCorrelator.ana.h'
// Derek Anderson
// 02.14.2023
//
// A module to implement Peter Komiske's
// EEC library in the sPHENIX software
// stack.
// ----------------------------------------------------------------------------

#pragma once

using namespace std;
using namespace fastjet;



// analysis methods -----------------------------------------------------------

void SEnergyCorrelator::DoCorrelatorCalculation() {

  // print debug statement
  if (m_inDebugMode) PrintDebug(31);

  // announce start of event loop
  const uint64_t nEvts = m_inTree -> GetEntriesFast();
  PrintMessage(7, nEvts);

  // event loop
  uint64_t nBytes = 0;
  for (uint64_t iEvt = 0; iEvt < nEvts; iEvt++) {

    const uint64_t entry = LoadTree(iEvt);
    if (entry < 0) break;

    const uint64_t bytes = GetEntry(iEvt);
    if (bytes < 0) {
      break;
    } else {
      nBytes += bytes;
      PrintMessage(8, nEvts, iEvt);
    }

    // jet loop
    uint64_t nJets = (int) m_evtNumJets;
    for (uint64_t iJet = 0; iJet < nJets; iJet++) {

      // clear vector for correlator
      m_jetCstVector.clear();

      // get jet info
      const uint64_t nCsts   = m_jetNumCst -> at(iJet);
      const double   ptJet   = m_jetPt     -> at(iJet);
      const double   etaJet  = m_jetEta    -> at(iJet);
      const double   phiJet  = m_jetPhi    -> at(iJet);
      const double   pxJet   = ptJet * cos(phiJet);
      const double   pyJet   = ptJet * sin(phiJet);
      const double   pzJet   = ptJet * sinh(etaJet);
      const TVector3 pVecJet = TVector3(pxJet, pyJet, pzJet);

      // select jet pt bin & apply jet cuts
      const uint32_t  iPtJetBin = GetJetPtBin(ptJet);
      const bool      isGoodJet = ApplyJetCuts(ptJet, etaJet);
      if (!isGoodJet) continue;

      // constituent loop
      for (uint64_t iCst = 0; iCst < nCsts; iCst++) {

        // get cst 3-vector
        const double   zCst    = (m_cstZ  -> at(iJet)).at(iCst);
        const TVector3 pVecCst = zCst * pVecJet;

        // get cst info
        const double etaCst  = (m_cstEta -> at(iJet)).at(iCst);
        const double phiCst  = (m_cstPhi -> at(iJet)).at(iCst);
        const double jtCst   = (m_cstJt  -> at(iJet)).at(iCst);
        const double pCst    = pVecCst.Mag();
        const double pTotCst = sqrt((jtCst * jtCst) + (pCst * pCst));
        const double pxCst   = pTotCst * cosh(etaCst) * cos(phiCst);
        const double pyCst   = pTotCst * cosh(etaCst) * sin(phiCst);
        const double pzCst   = pTotCst * sinh(etaCst);

        // create pseudojet & add to list
        PseudoJet constituent(pxCst, pyCst, pzCst, pTotCst);
        constituent.set_user_index(iCst);
        m_jetCstVector.push_back(constituent);
      }  // end cst loop

      // run eec computation
      for (size_t iPtBin = 0; iPtBin < m_nBinsJetPt; iPtBin++) {
        const bool isInPtBin = ((ptJet >= m_ptJetBins[iPtBin].first) && (ptJet < m_ptJetBins[iPtBin].second));
        if (isInPtBin) {
          if (m_jetCstVector.size() > 0) {
            m_eecLongSide[iPtJetBin] -> compute(m_jetCstVector);
          }
        }
      }  // end pTjet bin loop
    }  // end jet loop
  }  // end event loop

  // announce end of event loop
  PrintMessage(13);
  return;

}  // end 'DoCorrelatorCalculation()'



void SEnergyCorrelator::ExtractHistsFromCorr() {

  // print debug statement
  if (m_inDebugMode) PrintDebug(25);

  vector<double>                       drBinEdges;
  vector<double>                       lnDrBinEdges;
  pair<vector<double>, vector<double>> histContentAndError;
  for (size_t iPtBin = 0; iPtBin < m_nBinsJetPt; iPtBin++) {

    // create names
    TString sPtJetBin("_ptJet");
    sPtJetBin += floor(m_ptJetBins[iPtBin].first);

    TString sDrAxisName("hCorrelatorDrAxis");
    TString sLnDrAxisName("hCorrelatorLnDrAxis");
    sDrAxisName.Append(sPtJetBin.Data());
    sLnDrAxisName.Append(sPtJetBin.Data());

    // clear vectors
    drBinEdges.clear();
    lnDrBinEdges.clear();
    histContentAndError.first.clear();
    histContentAndError.second.clear();

    // grab bin edges, content, and error
    drBinEdges          = m_eecLongSide[iPtBin] -> bin_edges();
    histContentAndError = m_eecLongSide[iPtBin] -> get_hist_vars();
    //histContentAndError = m_eecLongSide[iPtBin] -> get_hist_errs();

    // create ln(dr) bin edges and arrays
    const size_t nDrBinEdges = drBinEdges.size();
    for (size_t iDrEdge = 0; iDrEdge < nDrBinEdges; iDrEdge++) {
      const double drEdge   = drBinEdges.at(iDrEdge);
      const double lnDrEdge = log(drEdge);
      lnDrBinEdges.push_back(lnDrEdge);
    }

    // bin edge arrays for histograms
    double drBinEdgeArray[nDrBinEdges];
    double lnDrBinEdgeArray[nDrBinEdges];
    for (size_t iDrEdge = 0; iDrEdge < nDrBinEdges; iDrEdge++) {
      drBinEdgeArray[iDrEdge]   = drBinEdges.at(iDrEdge);
      lnDrBinEdgeArray[iDrEdge] = lnDrBinEdges.at(iDrEdge);
    }

    // make sure number of bin edges is reasonable
    const bool isNumBinEdgesGood = ((nDrBinEdges - 1) == m_nBinsDr);
    if (!isNumBinEdgesGood) {
      PrintError(12, nDrBinEdges);
      assert(isNumBinEdgesGood);
    }

    // create output histograms
    m_outHistDrAxis[iPtBin]   = new TH1D(sDrAxisName.Data(),   "", m_nBinsDr, drBinEdgeArray);
    m_outHistLnDrAxis[iPtBin] = new TH1D(sLnDrAxisName.Data(), "", m_nBinsDr, lnDrBinEdgeArray);
    m_outHistDrAxis[iPtBin]   -> Sumw2();
    m_outHistLnDrAxis[iPtBin] -> Sumw2();

    // set bin content
    for (size_t iDrEdge = 0; iDrEdge < m_nBinsDr; iDrEdge++) {
      const size_t iDrBin     = iDrEdge + 1;
      const double binContent = histContentAndError.first.at(iDrEdge);
      const double binError   = histContentAndError.second.at(iDrEdge);

      // check if bin is good & set content/error
      const bool areBinValuesNans = (isnan(binContent) || isnan(binError));
      if (areBinValuesNans) {
        PrintError(13, 0, iDrBin);
      } else {
        m_outHistDrAxis[iPtBin]   -> SetBinContent(iDrBin, binContent);
        m_outHistLnDrAxis[iPtBin] -> SetBinContent(iDrBin, binContent);
        m_outHistDrAxis[iPtBin]   -> SetBinError(iDrBin, binError);
        m_outHistLnDrAxis[iPtBin] -> SetBinError(iDrBin, binError);
      }
    }  // end dr bin edge loop
  }  // end jet pt bin loop

  if (m_inStandaloneMode) PrintMessage(14);
  return;

}  // end 'ExtractHistsFromCorr()'



bool SEnergyCorrelator::ApplyJetCuts(const double ptJet, const double etaJet) {

  // print debug statement
  if (m_inDebugMode && (m_verbosity > 7)) PrintDebug(26);

  const bool isInPtRange  = ((ptJet >= m_ptJetRange[0])  && (ptJet < m_ptJetRange[1]));
  //const bool isInEtaRange = ((etaJet > m_etaJetRange[0]) && (etaJet < m_etaJetRange[1]));
  const bool isInEtaRange = (abs(etaJet) < m_etaJetRange[1]);
  const bool isGoodJet    = (isInPtRange && isInEtaRange);
  return isGoodJet;

}  // end 'ApplyJetCuts(double, double)'



bool SEnergyCorrelator::ApplyCstCuts(const double momCst, const double drCst) {

  // print debug statement
  if (m_inDebugMode && (m_verbosity > 7)) PrintDebug(27);

  const bool isInMomRange = ((momCst >= m_momCstRange[0]) && (momCst < m_momCstRange[1]));
  const bool isInDrRange  = ((drCst >= m_drCstRange[0])   && (drCst < m_drCstRange[1]));
  const bool isGoodCst    = (isInMomRange && isInDrRange);
  return isGoodCst;

}  // end 'ApplyCstCuts(double, double)'



uint32_t SEnergyCorrelator::GetJetPtBin(const double ptJet) {

  // print debug statement
  if (m_inDebugMode && (m_verbosity > 7)) PrintDebug(28);

  bool     isInPtBin(false);
  uint32_t iJetPtBin(0);
  for (size_t iPtBin = 0; iPtBin < m_nBinsJetPt; iPtBin++) {
    isInPtBin = ((ptJet >= m_ptJetBins[iPtBin].first) && (ptJet < m_ptJetBins[iPtBin].second));
    if (isInPtBin) {
      iJetPtBin = iPtBin;
      break; 
    }
  }
  return iJetPtBin;

}  // end 'GetJetPtBin(double)'

// end ------------------------------------------------------------------------
