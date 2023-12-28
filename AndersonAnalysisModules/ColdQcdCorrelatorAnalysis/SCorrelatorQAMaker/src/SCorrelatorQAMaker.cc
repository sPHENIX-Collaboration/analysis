// ----------------------------------------------------------------------------
// 'SCorrelatorQAMaker.cc'
// Derek Anderson
// 10.04.2023
//
// A module to produce TNtuples and histograms for QA
// plots relevant to the sPHENIX Cold QCD Energy-Energy
// Correlator analysis.
// ----------------------------------------------------------------------------

#define SCORRELATORQAMAKER_CC

// user includes
#include "SCorrelatorQAMaker.h"

using namespace std;
using namespace findNode;



namespace SColdQcdCorrelatorAnalysis {

  // ctor/dtor ----------------------------------------------------------------

  SCorrelatorQAMaker::SCorrelatorQAMaker() {

    m_checkTrackPairs = new SCheckTrackPairs();

  }  // end ctor


  SCorrelatorQAMaker::~SCorrelatorQAMaker() {

    delete m_checkTrackPairs;

  }  // end dtor

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------
