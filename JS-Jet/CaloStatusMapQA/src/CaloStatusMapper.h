///  --------------------------------------------------------------------------
/*! \file   CaloStatusMapper.h
 *  \author Derek Anderson
 *  \date   05.22.2024
 *
 *  A Fun4All QA module to plot no. of towers per event
 *  and vs. eta, phi as a function of status.
 */
///  --------------------------------------------------------------------------

#ifndef CLUSTERSTATUSMAPPER_H
#define CLUSTERSTATUSMAPPER_H

// c++ utilities
#include <string>
#include <vector>
// calo base
#include <calobase/TowerInfoContainerv2.h>
// f4a libraries
#include <fun4all/SubsysReco.h>
// module definitions
#include "CaloStatusMapperDefs.h"

// forward declarations
class PHCompositeNode;
class QAHistManagerHistDef;



// ----------------------------------------------------------------------------
//! Options for CaloStatusMapper module
// ----------------------------------------------------------------------------
struct CaloStatusMapperConfig {

  // system options
  bool debug = true;

  // input nodes
  std::vector<CaloStatusMapperDefs::NodeDef> inNodeNames = {
    {"TOWERINFO_CALIB_CEMC",    CaloStatusMapperDefs::Calo::EMC},
    {"TOWERINFO_CALIB_HCALIN",  CaloStatusMapperDefs::Calo::IHC},
    {"TOWERINFO_CALIB_HCALOUT", CaloStatusMapperDefs::Calo::OHC},
    {"TOWERINFO_CALIB_ZDC",     CaloStatusMapperDefs::Calo::ZDC},
    {"TOWERS_SEPD",             CaloStatusMapperDefs::Calo::SEPD}
  };

};



// ----------------------------------------------------------------------------
//! Map Status of Calo Towers
// ----------------------------------------------------------------------------
/*! This Fun4all modules ingests calorimeter towers, and then plots
 *  the no. of towers per event and vs. eta/phi for a given status.
 */
class CaloStatusMapper : public SubsysReco {

  public:

    // ctor
    CaloStatusMapper(const std::string& name = "CaloStatusMapper");
    ~CaloStatusMapper() override;

    // setters
    void SetConfig(const CaloStatusMapperConfig& config) {m_config = config;}

    // getters
    CaloStatusMapperConfig GetConfig() {return m_config;}

    // f4a methods
    int Init(PHCompositeNode* topNode)          override;
    int InitRun(PHCompositeNode* topNode)       override;  // FIXME might not need...
    int process_event(PHCompositeNode* topNode) override;
    int End(PHCompositeNode* topNode)           override;

  private:

    // private methods
    void InitHistManager();
    void BuildHistograms();
    void GrabNodes(PHCompositeNode* topNode);

    // output histograms
    CaloStatusMapperDefs::H1DVec m_vecHist1D;
    CaloStatusMapperDefs::H2DVec m_vecHist2D;

    // f4a members
    Fun4AllHistoManager* m_manager = NULL;

    // input nodes
    std::vector<TowerInfoContainer*> m_inNodes;

    // module configuration
    CaloStatusMapperConfig m_config;

};  // end CaloStatusMapper

#endif

// end ------------------------------------------------------------------------
