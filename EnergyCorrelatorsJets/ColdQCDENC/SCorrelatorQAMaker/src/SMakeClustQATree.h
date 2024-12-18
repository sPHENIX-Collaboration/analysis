// ----------------------------------------------------------------------------
// 'SMakeClustQATree.h'
// Derek Anderson
// 01.21.2024
//
// SCorrelatorQAMaker plugin to produce the QA tree
// for calorimeter clusters.
// ----------------------------------------------------------------------------

#ifndef SCORRELATORQAMAKER_SMAKECLUSTQATREE_H
#define SCORRELATORQAMAKER_SMAKECLUSTQATREE_H

// c++ utilities
#include <string>
#include <vector>
#include <utility>
// root utilities
#include <TF1.h>
#include <TTree.h>
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
// calo includes
#include <calobase/RawCluster.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calotrigger/CaloTriggerInfo.h>
// analysis utilities
#include <scorrelatorutilities/Tools.h>
#include <scorrelatorutilities/Types.h>
#include <scorrelatorutilities/Constants.h>
#include <scorrelatorutilities/Interfaces.h>
// plugin definitions
#include "SBaseQAPlugin.h"
#include "SMakeClustQATreeConfig.h"
#include "SMakeClustQATreeOutput.h"

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // SMakeClustQATree definition ----------------------------------------------

  class SMakeClustQATree : public SubsysReco, public SBaseQAPlugin<SMakeClustQATreeConfig> {

    public:

      // ctor/dtor
      SMakeClustQATree(const string& name = "ClustQATree") : SubsysReco(name) {};
      ~SMakeClustQATree() {};

      // F4A methods
      int Init(PHCompositeNode*)          override;
      int process_event(PHCompositeNode*) override;
      int End(PHCompositeNode*)           override;

    private:

      // internal methods
      void InitTree();
      void SaveOutput();
      void DoClustLoop(PHCompositeNode* topNode, const string node);
      bool IsGoodCluster(const RawCluster* cluster);

      // output
      SMakeClustQATreeOutput m_output;

      // root members
      TTree* m_tClustQA;

  };  // end SMakeClustQATree

}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------
