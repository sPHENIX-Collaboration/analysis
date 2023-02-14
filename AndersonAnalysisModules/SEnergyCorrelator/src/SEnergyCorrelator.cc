// ----------------------------------------------------------------------------
// 'SEnergyCorrelator.cc'
// Derek Anderson
// 01.20.2023
//
// A module to implement Peter Komiske's
// EEC library in the sPHENIX software
// stack.
// ----------------------------------------------------------------------------

#define SENERGYCORRELATOR_CC

// user includes
#include "SEnergyCorrelator.h"
#include "SEnergyCorrelator.io.h"
#include "SEnergyCorrelator.sys.h"
#include "SEnergyCorrelator.ana.h"

using namespace std;
using namespace fastjet;



// ctor/dtor ------------------------------------------------------------------

//SEnergyCorrelator::SEnergyCorrelator(const string &name, const bool isComplex, const bool doDebug) : SubsysReco(name) {
SEnergyCorrelator::SEnergyCorrelator(const string &name, const bool isComplex, const bool doDebug, const bool inBatch) {

  // initialize internal variables
  InitializeMembers();

  // set standalone/complex mode
  if (isComplex) {
    m_inComplexMode    = true;
    m_inStandaloneMode = false; 
  } else {
    m_inComplexMode    = false;
    m_inStandaloneMode = true;
  }

  // set verbosity in complex mode
/* TODO add back in complex mode
  if (m_inComplexMode) {
    m_verbosity = Verbosity();
  }
*/

  // set debug/batch mode & print debug statement
  m_inDebugMode = doDebug;
  m_inBatchMode = inBatch;
  if (m_inDebugMode) PrintDebug(1);

  // set module name & announce start of calculation
  m_moduleName = name;
  if (m_inStandaloneMode) PrintMessage(0);

}  // end ctor(string, bool, bool)



SEnergyCorrelator::~SEnergyCorrelator() {

  // print debug statement
  if (m_inDebugMode) PrintDebug(14);

  // delete pointers to files
  if (!m_inTree) {
    delete m_inFile;
    delete m_outFile;
  }

  // delete pointers to correlators
  for (size_t iPtBin = 0; iPtBin < m_nBinsJetPt; iPtBin++) {
    delete m_eecLongSide.at(iPtBin);
  }
  m_eecLongSide.clear();
  m_ptJetBins.clear();

}  // end dtor



// F4A methods ----------------------------------------------------------------

/* TODO F4A methods will go here */



// standalone-only methods ----------------------------------------------------

void SEnergyCorrelator::Init() {

  // print debug statement
  if (m_inDebugMode) PrintDebug(10);

  // make sure standalone mode is on & open files
  if (m_inComplexMode) {
    PrintError(5);
    assert(m_inStandaloneMode);
  } else {
    OpenInputFile();
  }
  OpenOutputFile();

  // announce files
  PrintMessage(1);

  // initialize input, output, & correlators
  InitializeTree();
  InitializeHists();
  InitializeCorrs();
  return;

}  // end 'StandaloneInit()'



void SEnergyCorrelator::Analyze() {

  // print debug statement
  if (m_inDebugMode) PrintDebug(12);

  // make sure standalone mode is on
  if (m_inComplexMode) {
    PrintError(8);
    assert(m_inStandaloneMode);
  }

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
      const double   pTotJet = sqrt((pxJet * pxJet) + (pyJet * pyJet) + (pzJet * pzJet));

      // select jet pt bin & apply jet cuts
      const uint32_t  iPtJetBin = GetJetPtBin(ptJet);
      const bool      isGoodJet = ApplyJetCuts(ptJet, etaJet);
      if (!isGoodJet) continue;

      // constituent loop
      for (uint64_t iCst = 0; iCst < nCsts; iCst++) {

        // get cst info
        const double zCst    = (m_cstZ   -> at(iJet)).at(iCst);
        const double drCst   = (m_cstDr  -> at(iJet)).at(iCst);
        const double etaCst  = (m_cstEta -> at(iJet)).at(iCst);
        const double phiCst  = (m_cstEta -> at(iJet)).at(iCst);
        const double pTotCst = zCst * pTotJet;
        const double pxCst   = pTotCst * cosh(etaCst) * cos(phiCst);
        const double pyCst   = pTotCst * cosh(etaCst) * sin(phiCst);
        const double pzCst   = pTotCst * sinh(etaCst);

        // apply cst cuts
        const bool isGoodCst = ApplyCstCuts(pTotCst, drCst);
        if (!isGoodCst) continue;

        // create pseudojet & add to list
        PseudoJet constituent(pxCst, pyCst, pzCst, pTotCst);
        constituent.set_user_index(iCst);
        m_jetCstVector.push_back(constituent);

      }  // end cst loop

      // run eec computation
      m_eecLongSide[iPtJetBin] -> compute(m_jetCstVector);

    }  // end jet loop
  }  // end event loop
  PrintMessage(13);

  // translate correlators into root hists
  ExtractHistsFromCorr();
  PrintMessage(9);
  return;

}  // end 'StandaloneAnalyze()'



void SEnergyCorrelator::End() {

  // print debug statement
  if (m_inDebugMode) PrintDebug(13);

  // make sure standalone mode is on & save output
  if (m_inComplexMode) {
    PrintError(9);
    assert(m_inStandaloneMode);
  } else {
    SaveOutput();
  }

  // announce end
  PrintMessage(11);
  return;

}  // end 'StandaloneEnd()'

// end ------------------------------------------------------------------------
