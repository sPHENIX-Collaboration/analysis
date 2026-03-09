// ----------------------------------------------------------------------------
// 'SMakeTrackQATuple.h'
// Derek Anderson
// 12.29.2023
//
// SCorrelatorQAMaker plugin to produce QA tuples
// for tracks.
// ----------------------------------------------------------------------------

#ifndef SCORRELATORQAMAKER_SMAKETRACKQATUPLE_H
#define SCORRELATORQAMAKER_SMAKETRACKQATUPLE_H

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
#include <fun4all/Fun4AllHistoManager.h>
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
#include "SMakeTrackQATupleConfig.h"

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // SMakeTrackQATuple definition ----------------------------------------------

  class SMakeTrackQATuple : public SubsysReco, public SBaseQAPlugin<SMakeTrackQATupleConfig> {

    public:

      // ctor/dtor
      SMakeTrackQATuple(const string& name = "TrackQATuple") : SubsysReco(name) {};
      ~SMakeTrackQATuple() {};

      // F4A methods
      int Init(PHCompositeNode*)          override;
      int process_event(PHCompositeNode*) override;
      int End(PHCompositeNode*)           override;

    private:

      // internal methods
      void InitTuple();
      void SaveOutput();
      void DoTrackLoop(PHCompositeNode* topNode);
      bool IsGoodTrack(SvtxTrack* track, PHCompositeNode* topNode);

      // for tuple leaves
      vector<float> m_vecTrackLeaves;

      // root members
      TNtuple* m_ntTrackQA;

  };  // end SMakeTrackQATuple

}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end -----------------------------------------------------------------------
