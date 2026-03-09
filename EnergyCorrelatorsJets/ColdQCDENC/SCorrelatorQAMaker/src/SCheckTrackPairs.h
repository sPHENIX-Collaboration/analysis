// ----------------------------------------------------------------------------
// 'SCheckTrackPairs.h'
// Derek Anderson
// 11.14.2023
//
// SCorrelatorQAMaker plugin to iterate through
// all pairs of tracks in an event and fill
// tuples/histograms comparing them.
// ----------------------------------------------------------------------------

#ifndef SCORRELATORQAMAKER_SCHECKTRACKPAIRS_H
#define SCORRELATORQAMAKER_SCHECKTRACKPAIRS_H

// c++ utilities
#include <string>
#include <vector>
#include <utility>
// root utilities
#include <TF1.h>
#include <TNtuple.h>
#include <Math/Vector3D.h>
// f4a libraries
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllReturnCodes.h>
// phool libraries
#include <phool/phool.h>
#include <phool/getClass.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHCompositeNode.h>
// tracking libraries
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/TrackAnalysisUtils.h>
// analysis utilities
#include <scorrelatorutilities/Tools.h>
#include <scorrelatorutilities/Types.h>
#include <scorrelatorutilities/Constants.h>
#include <scorrelatorutilities/Interfaces.h>
// plugin definitions
#include "SBaseQAPlugin.h"
#include "SCheckTrackPairsConfig.h"

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // SCheckTrackPairs definition ----------------------------------------------

  class SCheckTrackPairs : public SubsysReco, public SBaseQAPlugin<SCheckTrackPairsConfig> {

    public:

      // ctor/dtor
      SCheckTrackPairs(const string& name = "CheckTrackPairs") : SubsysReco(name) {};
      ~SCheckTrackPairs() {};

      // F4A methods
      int Init(PHCompositeNode*)          override;
      int process_event(PHCompositeNode*) override;
      int End(PHCompositeNode*)           override;

    private:

      // internal methods
      void InitTuples();
      void SaveOutput();
      void ResetVectors();
      void DoDoubleTrackLoop(PHCompositeNode* topNode);
      bool IsGoodTrack(SvtxTrack* track, PHCompositeNode* topNode);

      // vector members
      vector<float>             m_vecTrackPairLeaves;
      vector<TrkrDefs::cluskey> m_vecClustKeysA;
      vector<TrkrDefs::cluskey> m_vecClustKeysB;

      // root members
      TNtuple* m_ntTrackPairs;

  };  // end SCheckTrackPairs

}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------
