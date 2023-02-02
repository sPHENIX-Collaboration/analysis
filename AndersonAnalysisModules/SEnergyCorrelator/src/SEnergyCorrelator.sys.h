// 'SEnergyCorrelator.sys.h'
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
    PrintMessage(0);
  }

  m_inFile           = 0x0;
  m_inTree           = 0x0;
  m_outFile          = 0x0;
  m_verbosity        = 0;
  m_inDebugMode      = false;
  m_inComplexMode    = false;
  m_inStandaloneMode = false;
  m_inFileName       = "";
  m_inNodeName       = "";
  m_inTreeName       = "";
  m_outFileName      = "";
  return;

}  // end 'InitializeMembers()'



void SEnergyCorrelator::InitializeHists() {

  // print debug statement
  if (m_inDebugMode) {
    PrintMessage(5);
  }

  /* TODO output histograms wil be initialized here */
  return;

}  // end 'InitializeHists()'



void SEnergyCorrelator::InitializeCorrs() {

  // print debug statement
  if (m_inDebugMode) {
    PrintMessage(6);
  }

  /* TODO correlators will be initialized here */
  return;

}  // end 'InitializeCorrs()'



void SEnergyCorrelator::InitializeTree() {

  // print debug statement
  if (m_inDebugMode) {
    PrintMessage(4);
  }

  /* TODO input tree will be initialized here */
  return;

}  // end 'InitializeTree()'



void SEnergyCorrelator::PrintMessage(const uint32_t code) {

  // print debug statement
  if (m_inDebugMode && (m_verbosity > 5)) {
    cout << "SEnergyCorrelator::PrintMessage() printing a message..." << endl;
  }

  switch (code) {
    case 0:
      cout << "SEnergyCorrelator::InitializeMembers() initializing internal variables" << endl;
      break;
    case 1:
      cout << "SEnergyCorrelator::SEnergyCorrelator(string, bool, bool) calling ctor" << endl;
      break;
    case 2:
      cout << "SEnergyCorrelator::Init(PHCompositeNode*) initializing" << endl;
      break;
    case 3:
      cout << "SEnergyCorrelator::GrabInputNode() grabbing input node" << endl;
      break;
    case 4:
      cout << "SEnergyCorrelator::InitializeTree() initializing input tree" << endl;
      break;
    case 5:
      cout << "SEnergyCorrelator::InitializeHists() initializing histograms" << endl;
      break;
    case 6:
      cout << "SEnergyCorrelator::InitializeCorrs() initializing correlators" << endl;
      break;
    case 7:
      cout << "SEnergyCorrelator::process_event(PHCompositeNode*) processing event" << endl;
      break;
    case 8:
      cout << "SEnergyCorrelator::End(PHCompositeNode*) this is the end" << endl;
      break;
    case 9:
      cout << "SEnergyCorrelator::SaveOutput() saving output" << endl;
      break;
    case 10:
      cout << "SEnergyCorrelator::Init() initializing" << endl;
      break;
    case 11:
      cout << "SenergyCorrelator::OpenInputFile() opening input file" << endl;
      break;
    case 12:
      cout << "SEnergyCorrelator::Analyze() analyzing input" << endl;
      break;
    case 13:
      cout << "SEnergyCorrelator::End() this is the end" << endl;
      break;
    case 14:
      cout << "SEnergyCorrelator::~SEnergyCorrelator() calling dtor" << endl;
      break;
  }
  return;

}  // end 'PrintMessage()'



void SEnergyCorrelator::PrintError(const uint32_t code) {

  // print debug statement
  if (m_inDebugMode && (m_verbosity > 5)) {
    cout << "SEnergyCorrelator::PrintError() printing an error..." << endl;
  }

  switch (code) {
    case 0:
      cerr << "SEnergyCorrelator::Init(PHCompositeNode*) PANIC: calling complex method in standalone mode!\n"
           << "                                          Ending program execution!"
           << endl;
      break;
    case 1:
      cerr << "SEnergyCorrelator::GrabInputNode() PANIC: couldn't grab node \"" << m_inNodeName << "\"\n"
           << "                                   Ending program execution!"
           << endl;
      break;
    case 2:
      cerr << "SEnergyCorrelator::GrabInputNode() PANIC: couldn't grab tree \"" << m_inTreeName << "\" from node \"" << m_inNodeName << "\"\n"
           << "                                   Ending program execution!"
           << endl;
      break;
    case 3:
      cerr << "SEnergyCorrelator::process_event(PHCompositeNode*) PANIC: calling complex method in standalone mode!\n"
           << "                                                   Ending program execution!"
           << endl;
      break;
    case 4:
      cerr << "SEnergyCorrelator::End(PHCompositeNode*) PANIC: calling complex method in standalone mode!\n"
           << "                                         Ending program execution!"
           << endl;
      break;
    case 5:
      cerr << "SEnergyCorrelator::Init() PANIC: calling standalone method in complex mode!\n"
           << "                          Ending program execution!"
           << endl;
      break;
    case 6:
      cerr << "SEnergyCorrelator::OpenInputFile() PANIC: couldn't open file \"" << m_inFileName << "\"\n"
           << "                                   Ending program execution!"
           << endl;
      break;
    case 7:
      cerr << "SEnergyCorrelator::OpenInputFile() PANIC: couldn't grab tree \"" << m_inTreeName << "\" from file \"" << m_inFileName << "\"\n"
           << "                                   Ending program execution!"
           << endl;
      break;
    case 8:
      cerr << "SEnergyCorrelator::Analyze() PANIC: calling standalone method in complex mode!\n"
           << "                             Ending program execution!"
           << endl;
      break;
    case 9:
      cerr << "SEnergyCorrelator::End() PANIC: calling standalone method in complex mode!\n"
           << "                         Ending program execution!"
           << endl;
      break;
  }
  return;

}  // end 'PrintError()'

// end ------------------------------------------------------------------------
