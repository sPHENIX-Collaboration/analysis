// 'SEnergyCorrelator.system.h'
// Derek Anderson
// 01.27.2023
//
// A module to implement Peter Komiske's
// EEC library in the sPHENIX software
// stack.

#pragma once

using namespace std;
using namespace findNode;



void SEnergyCorrelator::InitializeMembers() {

  // print debug statement
  if (m_inDebugMode) {
    cout << "SEnergyCorrelator::InitializeMembers() initializing internal variables..." << endl;
  }

  m_verbosity        = 0;
  m_inDebugMode      = false;
  m_inComplexMode    = false;
  m_inStandaloneMode = false;
  return;

}  // end 'InitializeMembers()'



void SEnergyCorrelator::InitializeHists() {

  // print debug statement
  if (m_inDebugMode) {
     cout << "SEnergyCorrelator::InitializeHists() initializing histograms..." << endl;
  }

  /* output histograms wil be initialized here */
  return;

}  // end 'InitializeHists()'



void SEnergyCorrelator::PrintMessage() {

  // print debug statement
  if (m_inDebugMode && (m_verbosity > 5)) {
    cout << "SEnergyCorrelator::PrintMessage() printing a message..." << endl;
  }
  return;

}  // end 'PrintMessage()'



void SEnergyCorrelator::PrintError() {

  // print debug statement
  if (m_inDebugMode && (m_verbosity > 5)) {
    cout << "SEnergyCorrelator::PrintError() printing an error..." << endl;
  }
  return;

}  // end 'PrintError()'

// end ------------------------------------------------------------------------
