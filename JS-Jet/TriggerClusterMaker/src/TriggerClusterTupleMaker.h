// ----------------------------------------------------------------------------
/*! \file    TriggerClusterTupleMaker.h'
 *  \authors Derek Anderson
 *  \date    06.06.2024
 *
 *  A Fun4All module to dump trigger clusters
 *  into an NTuple
 */
// ----------------------------------------------------------------------------

#ifndef TRIGGERCLUSTERTUPLEMAKER_H
#define TRIGGERCLUSTERTUPLEMAKER_H

// c++ utilities
#include <string>
#include <vector>
#include <cassert>
// root libraries
#include <TFile.h>
#include <TNtuple.h>
// base calo libraries
#include <calobase/RawCluster.h>
#include <calobase/RawClusterv1.h>
#include <calobase/RawClusterContainer.h>



// ----------------------------------------------------------------------------
//! Options for TriggerClusterTupleMaker module
// ----------------------------------------------------------------------------
struct TriggerClusterTupleMakerConfig {

  // general options 
  bool debug = true;

  // output options
  std::string outFile  = "test.tuple.root";
  std::string outTuple = "TrgClustTuple";

  // input options
  std::string inNode = "TriggerClusters";

};



// ----------------------------------------------------------------------------
//! Creates an NTuple of Trigger Clusters
// ----------------------------------------------------------------------------
/*! This Fun4all modules ingests trigger clusters and
 *  dumps select information from them into ROOT
 *  NTuples.
 */
class TriggerClusterTupleMaker : public SubsysReco {

  public:

    // ctor
    TriggerClusterTupleMaker(const std::string& name = "TriggerClusterTupleMaker");
    ~TriggerClusterMaker() override;

    // setters
    void SetConfig(const TriggerClusterTupleMakerConfig& config) {m_config = config;}

    // getters
    TriggerClusterTupleMakerConfig GetConfig() {return m_config;}

    // f4a methods
    int Init(PHCompositeNode* topNode)          override;
    int process_event(PHCompositeNode* topNode) override;
    int End(PHCompositeNode* topNode)           override;

  private:

    // private methods
    void InitOutput();
    void GrabInputNode(PHCompositeNode* topNode);
    void SetVariables(RawClusterv1* cluster);
    void SaveOutput();
    void ResetVariables();

    // output variables
    std::vector<float> m_vecOutVars;

    // output members
    TFile*   m_outFile  = NULL;
    TNtuple* m_outTuple = NULL; 

    // input node
    RawClusterContainer* m_inTrgClusts = NULL;

    // module configuration
    TriggerClusterMakerConfig m_config;

};

#endif

// end ------------------------------------------------------------------------
