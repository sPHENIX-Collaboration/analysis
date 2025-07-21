// ----------------------------------------------------------------------------
/*! \file    TriggerClusterMaker.h'
 *  \authors Derek Anderson
 *  \date    05.15.2024
 *
 *  A Fun4All module to construct trigger clusters,
 *  jet patches stored in RawCluster objects, for
 *  downstream analysis
 */
// ----------------------------------------------------------------------------

#ifndef TRIGGERCLUSTERMAKER_H
#define TRIGGERCLUSTERMAKER_H

// c++ utilities
#include <array>
#include <string>
#include <utility>
#include <vector>
// calo base
#include <calobase/RawClusterContainer.h>
// f4a libraries
#include <fun4all/SubsysReco.h>
// module utilities
#include "TriggerClusterMakerDefs.h"

// forward declarations
class LL1Out;
class PHCompositeNode;
class RawClusterv1;
class TowerInfoContainer;
class TriggerPrimitiveContainer;



// ----------------------------------------------------------------------------
//! Options for TriggerClusterMaker module
// ----------------------------------------------------------------------------
struct TriggerClusterMakerConfig {

  // general options
  bool debug = true;

  // output options
  std::string outNodeName = "TriggerClusters";

  // input trigger nodes
  std::vector<std::string> inLL1Nodes = {
    "LL1OUT_JET"
  };
  std::vector<std::string> inPrimNodes = {
    "TRIGGERPRIMITIVES_JET",
    "TRIGGERPRIMITIVES_EMCAL",
    "TRIGGERPRIMITIVES_EMCAL_LL1",
    "TRIGGERPRIMITIVES_HCAL_LL1",
    "TRIGGERPRIMITIVES_HCALIN",
    "TRIGGERPRIMITIVES_HCALOUT"
  };

  // input tower nodes
  std::string inEMCalTowerNode = "TOWERINFO_CALIB_CEMC";
  std::string inIHCalTowerNode = "TOWERINFO_CALIB_HCALIN";
  std::string inOHCalTowerNode = "TOWERINFO_CALIB_HCALOUT";

};



// ----------------------------------------------------------------------------
//! Makes Trigger Cluster
// ----------------------------------------------------------------------------
/*! This Fun4all modules ingests calorimeter triggers and
 *  trigger primitives to turn them into RawCluster objects,
 *  i.e. "Trigger Clusters", for downstream analysis. Output
 *  clusters can be placed on the node tree, or saved to
 *  a TTree in a specified output file.
 */
class TriggerClusterMaker : public SubsysReco {

  public:

    // ctor
    TriggerClusterMaker(const std::string& name = "TriggerClusterMaker");
    ~TriggerClusterMaker() override;

    // setters
    void SetConfig(const TriggerClusterMakerConfig& config) {m_config = config;}

    // getters
    TriggerClusterMakerConfig GetConfig() {return m_config;}

    // f4a methods
    int Init(PHCompositeNode* topNode)          override;
    int process_event(PHCompositeNode* topNode) override;
    int End(PHCompositeNode* topNode)           override;

  private:

    // private methods
    void       InitOutNode(PHCompositeNode* topNode);
    void       GrabTowerNodes(PHCompositeNode* topNode);
    void       GrabTriggerNodes(PHCompositeNode* topNode);
    void       ProcessLL1s(LL1Out* lloNode);
    void       ProcessPrimitives(TriggerPrimitiveContainer* primNode);
    void       AddPrimitiveToCluster(TriggerPrimitive* primitive, RawClusterv1* cluster);
    TowerInfo* GetTowerFromKey(const uint32_t key, const uint32_t det);

    // input nodes
    std::array<TowerInfoContainer*, 3>      m_inTowerNodes;
    std::vector<LL1Out*>                    m_inLL1Nodes;
    std::vector<TriggerPrimitiveContainer*> m_inPrimNodes;

    // output node
    RawClusterContainer* m_outClustNode = NULL;

    // module configuration
    TriggerClusterMakerConfig m_config;

};

#endif

// end ------------------------------------------------------------------------
