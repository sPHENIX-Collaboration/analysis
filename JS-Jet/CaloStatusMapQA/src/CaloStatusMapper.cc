///  --------------------------------------------------------------------------
/*! \file   CaloStatusMapperLinkDef.h
 *  \author Derek Anderson
 *  \date   05.22.2024
 *
 *  A Fun4All QA module to plot no. of towers per event
 *  and vs. eta, phi as a function of status.
 */
///  --------------------------------------------------------------------------

#define CLUSTERSTATUSMAPPER_CC

// c++ utiilites
#include <algorithm>
#include <cassert>
#include <iostream>
// calo base
#include <calobase/TowerInfov2.h>
// f4a libraries
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllHistoManager.h>
// phool libraries
#include <phool/getClass.h>
#include <phool/phool.h>
#include <phool/PHCompositeNode.h>
// qa utilities
#include <qautils/QAHistManagerDef.h>
// root libraries
#include <TH1.h>
#include <TH2.h>

// module definition
#include "CaloStatusMapper.h"



// ctor/dtor ==================================================================

// ----------------------------------------------------------------------------
//! Module constructor
// ----------------------------------------------------------------------------
CaloStatusMapper::CaloStatusMapper(const std::string &name) : SubsysReco(name) {

  // print debug message
  if (m_config.debug && (Verbosity() > 0)) {
    std::cout << "CaloStatusMapper::CaloStatusMapper(const std::string &name) Calling ctor" << std::endl;
  }

  // make sure node vector is empty
  m_inNodes.clear();

}  // end ctor



// ----------------------------------------------------------------------------
//! Module destructor
// ----------------------------------------------------------------------------
CaloStatusMapper::~CaloStatusMapper() {

  // print debug message
  if (m_config.debug && (Verbosity() > 0)) {
    std::cout << "CaloStatusMapper::~CaloStatusMapper() Calling dtor" << std::endl;
  }

  /* nothing to do */

}  // end dtor



// fun4all methods ============================================================

// ----------------------------------------------------------------------------
//! Initialize module
// ----------------------------------------------------------------------------
int CaloStatusMapper::Init(PHCompositeNode* topNode) {

  if (m_config.debug) {
    std::cout << "CaloStatusMapper::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  }

  InitHistManager();
  BuildHistograms();
  return Fun4AllReturnCodes::EVENT_OK;

}  // end 'Init(PHCompositeNode*)'



// ----------------------------------------------------------------------------
//! Get ready for a new run
// ----------------------------------------------------------------------------
int CaloStatusMapper::InitRun(PHCompositeNode* topNode) {

  if (m_config.debug) {
    std::cout << "CaloStatusMapper::InitRun(PHCompositeNode *topNode) Preparing for new run" << std::endl;
  }

  /* TODO fill in */
  return Fun4AllReturnCodes::EVENT_OK;

}  // end 'Init(PHCompositeNode*)'



// ----------------------------------------------------------------------------
//! Grab inputs and fills histograms
// ----------------------------------------------------------------------------
int CaloStatusMapper::process_event(PHCompositeNode* topNode) {

  if (m_config.debug) {
    std::cout << "CaloStatusMapper::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;
  }

  // grab input nodes
  GrabNodes(topNode);

  // loop over input nodes
  for (size_t iNode = 0; iNode < m_inNodes.size(); ++iNode) {

    // loop over towers
    TowerInfoContainer* towers = m_inNodes[iNode];
    for (size_t iTower = 0; iTower < towers -> size(); ++iTower) {

       // grab eta, phi indices
       const int32_t key  = towers -> encode_key(iTower);
       const int32_t iEta = towers -> getTowerEtaBin(key);
       const int32_t iPhi = towers -> getTowerPhiBin(key);

       // get status
       const int status = CaloStatusMapperDefs::GetTowerStatus(
         towers -> get_tower_at_channel(iTower)
       );
       if (status == CaloStatusMapperDefs::Stat::Unknown) {
         std::cout << PHWHERE << ": Warning! Tower has an unknown status!\n"
                   << "  channel = " << iTower << ", key = " << key << "\n"
                   << "  node = " << m_config.inNodeNames[iNode].first
                   << std::endl; 
         continue;
       } 

       // fill histograms accordingly
       m_vecHist1D[iNode][status][CaloStatusMapperDefs::H1D::Num]      -> Fill(key);
       m_vecHist1D[iNode][status][CaloStatusMapperDefs::H1D::Eta]      -> Fill(iEta);
       m_vecHist1D[iNode][status][CaloStatusMapperDefs::H1D::Phi]      -> Fill(iPhi);
       m_vecHist2D[iNode][status][CaloStatusMapperDefs::H2D::EtaVsPhi] -> Fill(iPhi, iEta);

    }  // end tower loop
  }  // end node loop
  return Fun4AllReturnCodes::EVENT_OK;

}  // end 'process_event(PHCompositeNode*)'



// ----------------------------------------------------------------------------
//! Run final calculations
// ----------------------------------------------------------------------------
int CaloStatusMapper::End(PHCompositeNode *topNode) {

  if (m_config.debug) {
    std::cout << "CaloStatusMapper::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  }

  /* nothing to do */
  return Fun4AllReturnCodes::EVENT_OK;

}  // end 'End(PHCompositeNode*)'



// private methods ============================================================

// ----------------------------------------------------------------------------
//! Initialize histogram manager
// ----------------------------------------------------------------------------
void CaloStatusMapper::InitHistManager() {

  // print debug message
  if (m_config.debug && (Verbosity() > 0)) {
    std::cout << "CaloStatusMapper::InitHistManager() Initializing histogram manager" << std::endl;
  }

  m_manager = QAHistManagerDef::getHistoManager();
  if (!m_manager) {
    std::cerr << PHWHERE << ": PANIC! Couldn't grab histogram manager!" << std::endl;
    assert(m_manager);
  }
  return;

}  // end 'InitHistManager()'



// ----------------------------------------------------------------------------
//! Build histograms
// ----------------------------------------------------------------------------
void CaloStatusMapper::BuildHistograms() {

  // print debug message
  if (m_config.debug && (Verbosity() > 0)) {
    std::cout << "CaloStatusMapper::BuildHistograms() Creating histograms" << std::endl;
  }

  // grab relevant labels
  const auto& vecStatLabels = CaloStatusMapperDefs::StatLabels();
  const auto& vecH1DLabels  = CaloStatusMapperDefs::H1DLabels();
  const auto& vecH2DLabels  = CaloStatusMapperDefs::H2DLabels();

  // loop over input node names
  m_vecHist1D.resize( m_config.inNodeNames.size() );
  m_vecHist2D.resize( m_config.inNodeNames.size() );
  for (size_t iNode = 0; iNode < m_config.inNodeNames.size(); ++iNode) {

    // grab node name & type of calo
    const std::string nodeName = m_config.inNodeNames[iNode].first;
    const int         caloType = m_config.inNodeNames[iNode].second;

    // loop over status labels
    m_vecHist1D[iNode].resize( vecStatLabels.size() );
    m_vecHist2D[iNode].resize( vecStatLabels.size() );
    for (size_t iStatus = 0; iStatus < vecStatLabels.size(); ++iStatus) {

      // make 1d histograms
      for (size_t iHist1D = 0; iHist1D < vecH1DLabels.size(); ++iHist1D) {

        // construct name
        std::string histName = "h" + vecStatLabels[iStatus] + vecH1DLabels[iHist1D] + "_" + nodeName;

        // grab binning for histogram
        CaloStatusMapperDefs::BinDef binDef1D;
        switch (iHist1D) {

          case CaloStatusMapperDefs::H1D::Phi:
            binDef1D = CaloStatusMapperDefs::PhiBins().at(caloType);
            break;

          case CaloStatusMapperDefs::H1D::Eta:
            binDef1D = CaloStatusMapperDefs::EtaBins().at(caloType);
            break;

          case CaloStatusMapperDefs::H1D::Num:
            [[fallthrough]];

          default:
            binDef1D = CaloStatusMapperDefs::NumBins().at(caloType);
            break;
        }

        // create histogram
        m_vecHist1D[iNode][iStatus].push_back(
          new TH1D(
            histName.data(),
            "",  // TODO add axis labels
            std::get<0>(binDef1D),
            std::get<1>(binDef1D),
            std::get<2>(binDef1D)
          )
        );

        // if not null, register histogram with manager
        if (!m_vecHist1D[iNode][iStatus].back()) {
          std::cerr << PHWHERE << ": PANIC! Not able to make histogram " << histName
                    << "! Aborting!"
                    << std::endl;
          assert(m_vecHist1D[iNode][iStatus].back());
        } else {
          m_manager -> registerHisto( m_vecHist1D[iNode][iStatus].back() );
        }
      }  // end 1d histogram loop

      // make 2d histograms
      for (size_t iHist2D = 0; iHist2D < vecH2DLabels.size(); ++iHist2D) {

        // construct name
        std::string histName = "h" + vecStatLabels[iStatus] + vecH2DLabels[iHist2D] + "_" + nodeName;

        // grab binning for histogram
        CaloStatusMapperDefs::BinDef xBinDefs2D;
        CaloStatusMapperDefs::BinDef yBinDefs2D;
        switch (iHist2D) {

          case CaloStatusMapperDefs::H2D::EtaVsPhi:
            [[fallthrough]];

          default:
            xBinDefs2D = CaloStatusMapperDefs::PhiBins().at(caloType);
            yBinDefs2D = CaloStatusMapperDefs::EtaBins().at(caloType);
            break;
        }

        // create histogram
        m_vecHist2D[iNode][iStatus].push_back(
          new TH2D(
            histName.data(),
            "",  // TODO add axis labels
            std::get<0>(xBinDefs2D),
            std::get<1>(xBinDefs2D),
            std::get<2>(xBinDefs2D),
            std::get<0>(yBinDefs2D),
            std::get<1>(yBinDefs2D),
            std::get<2>(yBinDefs2D)
          )
        );

        // if not null, register histogram with manager
        if (!m_vecHist2D[iNode][iStatus].back()) {
          std::cerr << PHWHERE << ": PANIC! Not able to make histogram " << histName << "! Aborting!" << std::endl;
          assert(m_vecHist2D[iNode][iStatus].back());
        } else {
          m_manager -> registerHisto( m_vecHist2D[iNode][iStatus].back() );
        }
      }  // end 2d histogram loop
    }  // end status loop
  }  // end node loop
  return;

}  // end 'BuildHistograms()'



// ----------------------------------------------------------------------------
//! Grab input nodes
// ----------------------------------------------------------------------------
void CaloStatusMapper::GrabNodes(PHCompositeNode* topNode) {

  // print debug message
  if (m_config.debug && (Verbosity() > 0)) {
    std::cout << "CaloStatusMapper::GrabNodes(PHCompositeNode*) Grabbing input nodes" << std::endl;
  }

  // make sure node vector is empty
  m_inNodes.clear();

  // loop over nodes to grab
  for (auto inNodeName : m_config.inNodeNames) {
    m_inNodes.push_back(
      findNode::getClass<TowerInfoContainer>(topNode, inNodeName.first)
    );
    if (!m_inNodes.back()) {
      std::cerr << PHWHERE << ":" << " PANIC! Not able to grab node " << inNodeName.first << "! Aborting!" << std::endl;
      assert(m_inNodes.back());
    }
  }  // end input node name loop
  return;

}  // end 'GrabNodes(PHCompositeNode*)'

// end ------------------------------------------------------------------------
