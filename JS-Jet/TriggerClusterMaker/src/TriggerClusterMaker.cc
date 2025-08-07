// ----------------------------------------------------------------------------
/*! \file    TriggerClusterMaker.cc'
 *  \authors Derek Anderson
 *  \date    05.15.2024
 *
 *  A Fun4All module to construct trigger clusters,
 *  jet patches stored in RawCluster objects, for
 *  downstream analysis
 */
// ----------------------------------------------------------------------------

#define TRIGGERCLUSTERMAKER_CC

// c++ utiilites
#include <algorithm>
#include <cassert>
#include <iostream>
// calo base
#include <calobase/RawClusterv1.h>
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoContainer.h>
// trigger libraries
#include <calotrigger/LL1Out.h>
#include <calotrigger/LL1Outv1.h>
#include <calotrigger/TriggerPrimitiveContainer.h>
#include <calotrigger/TriggerPrimitiveContainerv1.h>
// f4a libraries
#include <fun4all/Fun4AllReturnCodes.h>
// phool libraries
#include <phool/getClass.h>
#include <phool/phool.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHObject.h>

// module definition
#include "TriggerClusterMaker.h"



// ctor/dtor ==================================================================

// ----------------------------------------------------------------------------
//! Module constructor
// ----------------------------------------------------------------------------
TriggerClusterMaker::TriggerClusterMaker(const std::string &name) : SubsysReco(name) {

  // print debug message
  if (m_config.debug && (Verbosity() > 0)) {
    std::cout << "TriggerClusterMaker::TriggerClusterMaker(const std::string &name) Calling ctor" << std::endl;
  }

  // set tower nodes to null
  std::fill(m_inTowerNodes.begin(), m_inTowerNodes.end(), nullptr);

  // make sure vectors are clear
  m_inLL1Nodes.clear();
  m_inPrimNodes.clear();

}  // end ctor



// ----------------------------------------------------------------------------
//! Module destructor
// ----------------------------------------------------------------------------
TriggerClusterMaker::~TriggerClusterMaker() {

  // print debug message
  if (m_config.debug && (Verbosity() > 0)) {
    std::cout << "TriggerClusterMaker::~TriggerClusterMaker() Calling dtor" << std::endl;
  }

  /* nothing to do */

}  // end dtor



// fun4all methods ============================================================

// ----------------------------------------------------------------------------
//! Initialize module
// ----------------------------------------------------------------------------
int TriggerClusterMaker::Init(PHCompositeNode* topNode) {

  if (m_config.debug) {
    std::cout << "TriggerClusterMaker::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  }

  // initialize outputs
  InitOutNode(topNode);
  return Fun4AllReturnCodes::EVENT_OK;

}  // end 'Init(PHCompositeNode*)'



// ----------------------------------------------------------------------------
//! Grab inputs and build trigger clusters
// ----------------------------------------------------------------------------
int TriggerClusterMaker::process_event(PHCompositeNode* topNode) {

  if (m_config.debug) {
    std::cout << "TriggerClusterMaker::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;
  }

  // grab input nodes
  GrabTowerNodes(topNode);
  GrabTriggerNodes(topNode);

  // loop over LL1 nodes
  for (auto inLL1Node : m_inLL1Nodes) {
    ProcessLL1s(inLL1Node);
  }  // end LL1 node loop

  // loop over trigger primitive nodes
  for (auto inPrimNode : m_inPrimNodes) {
    ProcessPrimitives(inPrimNode);
  }  // end trigger primitive node loop

  // end event
  return Fun4AllReturnCodes::EVENT_OK;

}  // end 'process_event(PHCompositeNode*)'



// ----------------------------------------------------------------------------
//! Run final calculations
// ----------------------------------------------------------------------------
int TriggerClusterMaker::End(PHCompositeNode *topNode) {

  if (m_config.debug) {
    std::cout << "TriggerClusterMaker::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  }

  /* nothing to do */

  return Fun4AllReturnCodes::EVENT_OK;

}  // end 'End(PHCompositeNode*)'



// private methods ============================================================

// ----------------------------------------------------------------------------
//! Create output node on node tree
// ----------------------------------------------------------------------------
void TriggerClusterMaker::InitOutNode(PHCompositeNode* topNode) {

  // print debug message
  if (m_config.debug && (Verbosity() > 0)) {
    std::cout << "TriggerClusterMaker::InitOutNode(PHCompositeNode*) Creating output node" << std::endl;
  }

  // find dst node
  //   - if missing, abort
  PHNodeIterator   itNode(topNode);
  PHCompositeNode* dstNode = static_cast<PHCompositeNode*>(itNode.findFirst("PHCompositeNode", "DST"));
  if (!dstNode) {
    std::cerr << PHWHERE << ": PANIC! DST node missing! Aborting!" << std::endl;
    assert(dstNode);
  }

  // find or add LL1 node
  PHNodeIterator   itTrg(dstNode);
  PHCompositeNode* trgNode = static_cast<PHCompositeNode*>(itNode.findFirst("PHCompositeNode", "LL1"));
  if (!trgNode) {
    PHCompositeNode* trgNodeToAdd = new PHCompositeNode("LL1");
    dstNode -> addNode(trgNodeToAdd);
    trgNode =  trgNodeToAdd;
  }

  // create container for clusters
  m_outClustNode = new RawClusterContainer();

  // and add node to tree
  PHIODataNode<PHObject>* clustNode = new PHIODataNode<PHObject>(m_outClustNode, m_config.outNodeName, "PHObject");
  if (!clustNode) {
    std::cerr << PHWHERE << ": PANIC! Couldn't create cluster node! Aborting!" << std::endl;
    assert(clustNode);
  } else {
    trgNode -> addNode(clustNode);
  }
  return;

}  // end 'InitOutNode(PHCompositeNode*)'



// ----------------------------------------------------------------------------
//! Grab input tower nodes
// ----------------------------------------------------------------------------
void TriggerClusterMaker::GrabTowerNodes(PHCompositeNode* topNode) {

  // print debug message
  if (m_config.debug && (Verbosity() > 0)) {
    std::cout << "TriggerClusterMaker::GrabTowerNodes(PHCompositeNode*) Grabbing input tower nodes" << std::endl;
  }

  // get emcal tower info node
  m_inTowerNodes[TriggerClusterMakerDefs::Cal::EM] = findNode::getClass<TowerInfoContainer>(topNode, m_config.inEMCalTowerNode);
  if (!m_inTowerNodes[TriggerClusterMakerDefs::Cal::EM]) {
    std::cerr << PHWHERE << ": PANIC! Couldn't grab EMCal towers from node '" << m_config.inEMCalTowerNode << "'!" << std::endl;
    assert(m_inTowerNodes[TriggerClusterMakerDefs::Cal::EM]); 
  }

  // get inner hcal tower info node
  m_inTowerNodes[TriggerClusterMakerDefs::Cal::IH] = findNode::getClass<TowerInfoContainer>(topNode, m_config.inIHCalTowerNode);
  if (!m_inTowerNodes[TriggerClusterMakerDefs::Cal::EM]) {
    std::cerr << PHWHERE << ": PANIC! Couldn't grab IHCal towers from node '" << m_config.inIHCalTowerNode << "'!" << std::endl;
    assert(m_inTowerNodes[TriggerClusterMakerDefs::Cal::IH]); 
  }

  // get outer hcal tower info node
  m_inTowerNodes[TriggerClusterMakerDefs::Cal::OH] = findNode::getClass<TowerInfoContainer>(topNode, m_config.inOHCalTowerNode);
  if (!m_inTowerNodes[TriggerClusterMakerDefs::Cal::EM]) {
    std::cerr << PHWHERE << ": PANIC! Couldn't grab OHCal towers from node '" << m_config.inOHCalTowerNode << "'!" << std::endl;
    assert(m_inTowerNodes[TriggerClusterMakerDefs::Cal::OH]); 
  }
  return;

}  // end 'GrabTowerNodes(PHCompositeNode*)'



// ----------------------------------------------------------------------------
//! Grab input trigger nodes
// ----------------------------------------------------------------------------
void TriggerClusterMaker::GrabTriggerNodes(PHCompositeNode* topNode) {

  // print debug message
  if (m_config.debug && (Verbosity() > 0)) {
    std::cout << "TriggerClusterMaker::GrabTriggerNodes(PHCompositeNode*) Grabbing input trigger nodes" << std::endl;
  }

  // get LL1 nodes
  for (const std::string& inLL1Node : m_config.inLL1Nodes) {
    m_inLL1Nodes.push_back(
      findNode::getClass<LL1Out>(topNode, inLL1Node)
    );
    if (!m_inLL1Nodes.back()) {
      std::cerr << PHWHERE << ": PANIC! Couldn't grab LL1Out node '" << inLL1Node << "'!" << std::endl;
      assert(m_inLL1Nodes.back());
    }
  }

  // get trigger primitive nodes
  for (const std::string& inPrimNode : m_config.inPrimNodes) {
    m_inPrimNodes.push_back(
      findNode::getClass<TriggerPrimitiveContainer>(topNode, inPrimNode)
    );
    if (!m_inPrimNodes.back()) {
      std::cerr << PHWHERE << ": PANIC! Couldn't grab TriggerPrimitive node '" << inPrimNode << "'!" << std::endl;
      assert(m_inPrimNodes.back());
    }
  }
  return;

}  // end 'GrabTriggerNodes(PHCompositeNode*)'



// ----------------------------------------------------------------------------
//! Process a node of LL1s
// ----------------------------------------------------------------------------
void TriggerClusterMaker::ProcessLL1s(LL1Out* lloNode) {

  // loop over trigger words
  LL1Outv1::Range lloWordRange = lloNode -> getTriggerWords();
  for (
    LL1Outv1::Iter itTrgWord = lloWordRange.first;
    itTrgWord != lloWordRange.second;
    ++itTrgWord
  ) {

    // FIXME figure out how to get trigger
    //   - primitives from LL1Out
    //auto word = (*itTrgWord).second;
    //MakeCluster(word);

  }  // end trigger word loop
  return;

}  // end 'ProcessLL1s(LL1Out*)'



// ----------------------------------------------------------------------------
//! Process a node of trigger primitives
// ----------------------------------------------------------------------------
void TriggerClusterMaker::ProcessPrimitives(TriggerPrimitiveContainer* primNode) {

    // loop over primitives
  TriggerPrimitiveContainerv1::Range trgPrimStoreRange = primNode -> getTriggerPrimitives();
  for (
    TriggerPrimitiveContainerv1::Iter itTrgPrim = trgPrimStoreRange.first;
    itTrgPrim != trgPrimStoreRange.second;
    ++itTrgPrim
  ) {

    // grab trigger primitve and decompose into clusters
    TriggerPrimitive* primitive = (*itTrgPrim).second;
    if (!primitive) continue;

    // create new cluster and add primitive to it
    RawClusterv1* cluster = new RawClusterv1();
    AddPrimitiveToCluster(primitive, cluster);

    // put cluster in output node
    m_outClustNode -> AddCluster(cluster);

  }  // end trigger primitive loop
  return;

}  // end 'ProcessPrimitives(TriggerPrimitiveContainer*)'



// ----------------------------------------------------------------------------
//! Add primitive to a given cluster
// ----------------------------------------------------------------------------
void TriggerClusterMaker::AddPrimitiveToCluster(TriggerPrimitive* primitive, RawClusterv1* cluster) {

  // print debug message
  if (m_config.debug && (Verbosity() > 1)) {
    std::cout << "TriggerClusterMaker::MakeClustersFromPrimitive(TriggerPrimitive*) Making clusters from TriggerPrimitive object" << std::endl;
  }

  // loop over sums
  TriggerPrimitivev1::Range trgPrimSumRange = primitive -> getSums();
  for (
    TriggerPrimitive::Iter itPrimSum = trgPrimSumRange.first;
    itPrimSum != trgPrimSumRange.second;
    ++itPrimSum
  ) {

    // loop over summands
    auto sum = (*itPrimSum).second;
    for (
      auto itSum = sum -> begin();
      itSum != sum -> end();
      ++itSum
    ) {

      // get sum key and detector ID
      auto sumKey = (*itPrimSum).first;
      auto detID  = TriggerDefs::getDetectorId_from_TriggerSumKey(sumKey);
      std::cout << "    CHECK-1 sum key = " << sumKey << ", detector ID = " << detID << std::endl;

      // get eta, phi bin of sum
      const uint32_t iEtaStart = TriggerClusterMakerDefs::GetBin(
        (*itPrimSum).first,
         TriggerClusterMakerDefs::Axis::Eta,
         TriggerClusterMakerDefs::Type::Prim
      );
      const uint32_t iPhiStart = TriggerClusterMakerDefs::GetBin(
        (*itPrimSum).first,
         TriggerClusterMakerDefs::Axis::Phi,
         TriggerClusterMakerDefs::Type::Prim
      );

      // then iterate through towers in sum
      /* TODO */

      // grab tower key
      const uint32_t towKey = TriggerClusterMakerDefs::GetKeyFromEtaPhiIndex(iEtaStart, iPhiStart, detID);

      // and finally grab tower
      TowerInfo* tower = GetTowerFromKey(towKey, detID);
      if (!tower) continue;
      std::cout << "    CHECK0 (eta, phi) = (" << iEtaStart << ", " << iPhiStart << ")\n"
                << "           key = " << towKey << ", tower = " << tower
                << std::endl;

      // and add to custer
      cluster -> addTower(towKey, tower -> get_energy());

    }  // end tower loop

  }  // end primitive sum loop
  return;

}  // end 'MakeClustersFromPrimitive(TriggerPrimitive*, RawClusterv1*)'



// ----------------------------------------------------------------------------
//! Grab tower from an input node based on key
// ----------------------------------------------------------------------------
TowerInfo* TriggerClusterMaker::GetTowerFromKey(const uint32_t key, const uint32_t det) {

  // print debug message
  if (m_config.debug && (Verbosity() > 2)) {
    std::cout << "TriggerClusterMaker::GetTowerFromKey(uint32_t) Grabbing tower based on key..." << std::endl;
  }

  TowerInfo* tower;
  switch (det) {

    // get emcal tower index
    case TriggerDefs::DetectorId::emcalDId:
      tower = m_inTowerNodes[TriggerClusterMakerDefs::Cal::EM] -> get_tower_at_key(key);
      break;

    // get inner hcal tower index
    case TriggerDefs::DetectorId::hcalinDId:
      tower = m_inTowerNodes[TriggerClusterMakerDefs::Cal::IH] -> get_tower_at_key(key);
      break;

    case TriggerDefs::DetectorId::hcaloutDId:
      tower = m_inTowerNodes[TriggerClusterMakerDefs::Cal::OH] -> get_tower_at_key(key);
      break;

    // otherwise return null
    default:
      tower = nullptr;
      break;
  }
  return tower;

}  // end 'GetTowerFromKey(uint32_t, uint32_t)'

// end ------------------------------------------------------------------------
