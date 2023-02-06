// 'SEnergyCorrelator.io.h'
// Derek Anderson
// 01.27.2023
//
// A module to implement Peter Komiske's
// EEC library in the sPHENIX software
// stack.

#pragma once

using namespace std;
using namespace findNode;



void SEnergyCorrelator::GrabInputNode() {

  // print debug statement
  if (m_inDebugMode) {
    PrintMessage(3);
  }

  /* TODO grab input node here */
  const bool placeholder = true;
  if (placeholder) {
    PrintError(1);
    assert(!placeholder);
  }
  if (placeholder) {
    PrintError(2);
    assert(!placeholder);
  }
  return;

}  // end 'GrabInputNode()'



void SEnergyCorrelator::OpenInputFile() {

  // print debug statement
  if (m_inDebugMode) {
    PrintMessage(11);
  }

  // open file
  m_inFile = new TFile(m_inFileName.data(), "read");
  if (!m_inFile) {
    PrintError(6);
    assert(m_inFile);
  }

  // grab tree
  m_inFile -> GetObject(m_inTreeName.data(), m_inTree);
  if (!m_inTree) {
    PrintError(7);
  } 
  return;

}  // end 'OpenInputFile()'



void SEnergyCorrelator::SaveOutput() {

  // print debug statement
  if (m_inDebugMode) {
    PrintMessage(9);
  }

  /* TODO save output here */
  return;

}  // end 'SaveOutput()'

// end ------------------------------------------------------------------------
