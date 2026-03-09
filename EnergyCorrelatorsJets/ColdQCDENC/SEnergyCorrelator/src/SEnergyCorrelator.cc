/// ---------------------------------------------------------------------------
/*! \file    SEnergyCorrelator.cc
 *  \authors Derek Anderson, Alex Clarke
 *  \date    01.20.2023
 *
 *  A module to run ENC calculations in the sPHENIX
 *  software stack for the Cold QCD EEC analysis.
 */
/// ---------------------------------------------------------------------------

#define SENERGYCORRELATOR_CC

// analysis definition
#include "SEnergyCorrelator.h"
#include "SEnergyCorrelator.io.h"
#include "SEnergyCorrelator.sys.h"
#include "SEnergyCorrelator.ana.h"
#include "SEnergyCorrelatorConfig.h"

// make common namespaces implicit
using namespace std;
using namespace findNode;



namespace SColdQcdCorrelatorAnalysis {

  // ctor/dtor =================================================================

  // --------------------------------------------------------------------------
  //! Module ctor accepting config struct
  // --------------------------------------------------------------------------
  SEnergyCorrelator::SEnergyCorrelator(SEnergyCorrelatorConfig& config) :
    SubsysReco(config.moduleName)
  {

    // print debug statement
    if (config.isDebugOn) PrintDebug(44);

    // set config & initialize internal variables
    m_config = config;
    InitializeMembers();

    // announce start of calculation
    PrintMessage(0);

  }  // end ctor(SEnergyCorrelatorConfig&)



  // --------------------------------------------------------------------------
  //! Module dtor
  // --------------------------------------------------------------------------
  SEnergyCorrelator::~SEnergyCorrelator() {

    // print debug statement
    if (m_config.isDebugOn) PrintDebug(14);

    // delete pointers to files
    if (!m_inChain) {
      delete m_outFile;
    }

    // delete pointers to correlators/histograms
    for (size_t iPtBin = 0; iPtBin < m_config.ptJetBins.size(); iPtBin++) {
      delete m_eecLongSide.at(iPtBin);
      delete m_outPackageHistVarDrAxis.at(iPtBin);
      delete m_outPackageHistErrDrAxis.at(iPtBin);
      delete m_outPackageHistVarLnDrAxis.at(iPtBin);
      delete m_outPackageHistErrLnDrAxis.at(iPtBin);
      delete m_outManualHistErrDrAxis.at(iPtBin);
      delete m_outProjE3C.at(iPtBin);
      for(size_t jRLBin = 0; jRLBin < m_config.rlBins.size(); jRLBin++){
	delete m_outE3C[iPtBin].at(jRLBin);
      }
    }
    m_eecLongSide.clear();
    m_outPackageHistVarDrAxis.clear();
    m_outPackageHistErrDrAxis.clear();
    m_outPackageHistVarLnDrAxis.clear();
    m_outPackageHistErrLnDrAxis.clear();
    m_outManualHistErrDrAxis.clear();
    m_outE3C.clear();
    m_outProjE3C.clear();

  }  // end dtor



  // public methods ===========================================================

  // --------------------------------------------------------------------------
  //! Module initialization
  // --------------------------------------------------------------------------
  void SEnergyCorrelator::Init() {

    // print debug statement
    if (m_config.isDebugOn) PrintDebug(10);

    // open input/output files
    OpenInputFiles();
    OpenOutputFile();

    // announce files
    PrintMessage(1);

    // initialize smearing if need be
    if (m_config.isInTreeTruth && (m_config.doJetSmear || m_config.doCstSmear)) {
      InitializeSmearing();
    }

    // initialize input, output, & correlators
    InitializeTree();
    InitializeCorrs();
    InitializeHists();
    return;

  }  // end 'Init()'



  // --------------------------------------------------------------------------
  //! Run analysis
  // --------------------------------------------------------------------------
  void SEnergyCorrelator::Analyze() {

    // print debug statement
    if (m_config.isDebugOn) PrintDebug(12);

    // announce start of event loop
    const uint64_t nEvts = m_inChain -> GetEntriesFast();
    PrintMessage(7, nEvts);

    // event loop
    uint64_t nBytes = 0;
    for (uint64_t iEvt = 0; iEvt < nEvts; iEvt++) {

      const uint64_t entry = Interfaces::LoadTree(m_inChain, iEvt, m_fCurrent);
      if (entry < 0) break;

      const uint64_t bytes = Interfaces::GetEntry(m_inChain, iEvt);
      if (bytes < 0) {
        break;
      } else {
        nBytes += bytes;
        PrintMessage(8, nEvts, iEvt);
      }

      // fill container and run calculations
      m_interface.SetCorrelatorInput(
        m_input,
        m_config.isInTreeTruth,
        m_config.isEmbed
      );
      RunCalculations();

    }  // end event loop

    // announce end of event loop
    PrintMessage(13);

    // translate correlators into root hists
    ExtractHistsFromCorr();
    PrintMessage(9);
    return;

  }  // end 'Analyze()'



  // --------------------------------------------------------------------------
  //! Module wind-down
  // --------------------------------------------------------------------------
  void SEnergyCorrelator::End() {

    // print debug statement
    if (m_config.isDebugOn) PrintDebug(13);

    // save output, close files, and exit
    SaveOutput();
    CloseOutputFile();
    PrintMessage(11);
    return;

  }  // end 'End()'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------
