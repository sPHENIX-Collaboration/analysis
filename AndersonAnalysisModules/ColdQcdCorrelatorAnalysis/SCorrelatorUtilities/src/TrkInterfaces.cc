// ----------------------------------------------------------------------------
// 'TrkInterfaces.h'
// Derek Anderson
// 03.05.2024
//
// Track-related interfaces.
// ----------------------------------------------------------------------------

#define SCORRELATORUTILITIES_TRKINTERFACES_CC

// namespace definition
#include "TrkInterfaces.h"

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  TF1* Interfaces::GetSigmaDcaTF1(
    const string name,
    const vector<float> params,
    const pair<float, float> range
  ) {

    TF1* func = new TF1(
      name.data(),
      Const::SigmaDcaFunc().data(),
      range.first,
      range.second
    );
    func -> SetParameter(0, params.at(0));
    func -> SetParameter(1, params.at(1));
    func -> SetParameter(2, params.at(2));
    return func; 

  }  // end 'GetSigmaDcaTF1(string, vector<float>, pair<float, float>)'



  SvtxTrackMap* Interfaces::GetTrackMap(PHCompositeNode* topNode) {

    // grab track map
    SvtxTrackMap* mapTrks = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
    if (!mapTrks) {
      cerr << PHWHERE
           << "PANIC: SvtxTrackMap node is missing!"
           << endl;
      assert(mapTrks);
    }
    return mapTrks;

  }  // end 'GetTrackMap(PHCompositeNode*)'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------
