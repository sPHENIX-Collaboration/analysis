#include "RawClusterPositionCorrectionFull.h"

#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeomContainer.h>

#include <phparameter/PHParameters.h>

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/SubsysReco.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHObject.h>
#include <phool/getClass.h>

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

RawClusterPositionCorrectionFull::RawClusterPositionCorrectionFull(const std::string &name)
  : SubsysReco(std::string("RawClusterPositionCorrectionFull_") + name)
  , _eclus_calib_params(std::string("eclus_params_") + name)
  , _ecore_calib_params(std::string("ecore_params_") + name)
  , _det_name(name)
  , bins_eta(385) // store bin edges
  , bins_phi(65) // store bin edges
{
  SetDefaultParameters(_eclus_calib_params);
  SetDefaultParameters(_ecore_calib_params);
}

int RawClusterPositionCorrectionFull::InitRun(PHCompositeNode *topNode)
{
  CreateNodeTree(topNode);

  if (Verbosity())
  {
    std::cout << "RawClusterPositionCorrectionFull is running for clusters in the EMCal with eclus parameters:" << std::endl;
    _eclus_calib_params.Print();

    std::cout << "RawClusterPositionCorrectionFull is running for clusters in the EMCal with ecore parameters:" << std::endl;
    _ecore_calib_params.Print();
  }
  // now get the actual number of bins in the calib file
  std::ostringstream paramname;
  paramname.str("");
  paramname << "number_of_bins_eta";

  // +1 because I use bins as the total number of bin boundaries
  // i.e. 384 bins corresponds to 385 bin boundaries
  bins_eta = _eclus_calib_params.get_int_param(paramname.str()) + 1;

  paramname.str("");
  paramname << "number_of_bins_phi";

  // +1 because I use bins as the total number of bin boundaries
  // i.e. 64 bins corresponds to 65 bin boundaries
  bins_phi = _eclus_calib_params.get_int_param(paramname.str()) + 1;

  // set bin boundaries eta
  // there are 24 blocks in eta and each block is consists of 4 towers in 2x2 form which results in 24x2=48 bins in eta
  for (int j = 0; j < bins_eta; j++)
  {
    binvals_eta.push_back(0. + j * 48. / (float) (bins_eta - 1));
  }

  // set bin boundaries phi
  // there are 4 blocks in phi and each block is consists of 4 towers in 2x2 form which results in 4x2=8 bins in phi
  for (int j = 0; j < bins_phi; j++)
  {
    binvals_phi.push_back(0. + j * 8. / (float) (bins_phi - 1));
  }

  for (int i = 0; i < bins_eta - 1; i++)
  {
    std::vector<double> dumvec;

    for (int j = 0; j < bins_phi - 1; j++)
    {
      std::ostringstream calib_const_name;
      calib_const_name.str("");
      calib_const_name << "recalib_const_eta" << i << "_phi" << j;
      dumvec.push_back(_eclus_calib_params.get_double_param(calib_const_name.str()));
    }
    eclus_calib_constants.push_back(dumvec);
  }

  for (int i = 0; i < bins_eta - 1; i++)
  {
    std::vector<double> dumvec;

    for (int j = 0; j < bins_phi - 1; j++)
    {
      std::ostringstream calib_const_name;
      calib_const_name.str("");
      calib_const_name << "recalib_const_eta" << i << "_phi" << j;
      dumvec.push_back(_ecore_calib_params.get_double_param(calib_const_name.str()));
    }
    ecore_calib_constants.push_back(dumvec);
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int RawClusterPositionCorrectionFull::process_event(PHCompositeNode *topNode)
{
  if (Verbosity())
  {
    std::cout << "Processing a NEW EVENT" << std::endl;
  }

  RawClusterContainer *rawclusters = findNode::getClass<RawClusterContainer>(topNode, "CLUSTER_" + _det_name);
  if (!rawclusters)
  {
    std::cout << "No " << _det_name << " Cluster Container found while in RawClusterPositionCorrectionFull, can't proceed!!!" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  RawTowerContainer *_towers = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_" + _det_name);
  if (!_towers)
  {
    std::cout << "No calibrated " << _det_name << " tower info found while in RawClusterPositionCorrectionFull, can't proceed!!!" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  std::string towergeomnodename = "TOWERGEOM_" + _det_name;
  RawTowerGeomContainer *towergeom = findNode::getClass<RawTowerGeomContainer>(topNode, towergeomnodename);
  if (!towergeom)
  {
    std::cout << PHWHERE << ": Could not find node " << towergeomnodename << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  const int nphibin = towergeom->get_phibins();

  // loop over the clusters
  RawClusterContainer::ConstRange begin_end = rawclusters->getClusters();
  RawClusterContainer::ConstIterator iter;

  for (iter = begin_end.first; iter != begin_end.second; ++iter)
  {
    // RawClusterDefs::keytype key = iter->first;
    RawCluster *cluster = iter->second;

    float clus_energy = cluster->get_energy();

    std::vector<float> toweretas;
    std::vector<float> towerphis;
    std::vector<float> towerenergies;

    // loop over the towers in the cluster
    RawCluster::TowerConstRange towers = cluster->get_towers();
    RawCluster::TowerConstIterator toweriter;

    for (toweriter = towers.first; toweriter != towers.second; ++toweriter)
    {
      RawTower *tower = _towers->getTower(toweriter->first);

      int towereta = tower->get_bineta();
      int towerphi = tower->get_binphi();
      double towerenergy = tower->get_energy();

      // put the etabin, phibin, and energy into the corresponding vectors
      toweretas.push_back(towereta);
      towerphis.push_back(towerphi);
      towerenergies.push_back(towerenergy);
    }

    int ntowers = toweretas.size();
    assert(ntowers >= 1);

    // loop over the towers to determine the energy
    // weighted eta and phi position of the cluster

    float etamult = 0;
    float etasum = 0;
    float phimult = 0;
    float phisum = 0;

    for (int j = 0; j < ntowers; j++)
    {
      float energymult = towerenergies.at(j) * toweretas.at(j);
      etamult += energymult;
      etasum += towerenergies.at(j);

      int phibin = towerphis.at(j);

      if (phibin - towerphis.at(0) < -nphibin / 2.0)
        phibin += nphibin;
      else if (phibin - towerphis.at(0) > +nphibin / 2.0)
        phibin -= nphibin;
      assert(std::abs(phibin - towerphis.at(0)) <= nphibin / 2.0);

      energymult = towerenergies.at(j) * phibin;
      phimult += energymult;
      phisum += towerenergies.at(j);
    }

    float avgphi = phimult / phisum;
    float avgeta = etamult / etasum;

    if (avgphi < 0) avgphi += nphibin;

    // this determines the position of the cluster in the 48x8 sector
    float fmodphi = fmod(avgphi, 8);
    float fmodeta = fmod(avgeta, 48);

    // determine the bin number
    // 8 is here since we divide the 48x8 sector into 64 bins in phi
    // 48 is here since we divide the 48x8 sector into 384 bins in eta

    int etabin = -99;
    int phibin = -99;
    for (int j = 0; j < bins_phi - 1; j++) // TODO: Implement in binary search.
      if (fmodphi >= binvals_phi.at(j) && fmodphi <= binvals_phi.at(j + 1))
        phibin = j;

    for (int j = 0; j < bins_eta - 1; j++)
      if (fmodeta >= binvals_eta.at(j) && fmodeta <= binvals_eta.at(j + 1))
        etabin = j;

    if ((phibin < 0 || etabin < 0) && Verbosity())
    {
      if (Verbosity())
        std::cout << "couldn't recalibrate cluster, something went wrong??" << std::endl;
    }

    float eclus_recalib_val = 1;
    float ecore_recalib_val = 1;
    if (phibin > -1 && etabin > -1)
    {
      eclus_recalib_val = eclus_calib_constants.at(etabin).at(phibin);
      ecore_recalib_val = ecore_calib_constants.at(etabin).at(phibin);
    }
    RawCluster *recalibcluster = dynamic_cast<RawCluster *>(cluster->CloneMe());
    assert(recalibcluster);
    recalibcluster->set_energy(clus_energy / eclus_recalib_val);
    recalibcluster->set_ecore(cluster->get_ecore() / ecore_recalib_val);
    _recalib_clusters->AddCluster(recalibcluster);

    if (Verbosity() && clus_energy > 1)
    {
      std::cout << "Input eclus cluster energy: " << clus_energy << std::endl;
      std::cout << "Recalib value: " << eclus_recalib_val << std::endl;
      std::cout << "Recalibrated eclus cluster energy: " << clus_energy / eclus_recalib_val << std::endl;
      std::cout << "Input ecore cluster energy: " << cluster->get_ecore() << std::endl;
      std::cout << "Recalib value: " << ecore_recalib_val << std::endl;
      std::cout << "Recalibrated eclus cluster energy: " << cluster->get_ecore() / ecore_recalib_val << std::endl;
    }
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

void RawClusterPositionCorrectionFull::CreateNodeTree(PHCompositeNode *topNode)
{
  PHNodeIterator iter(topNode);

  // Get the DST Node
  PHCompositeNode *dstNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "DST"));

  // Check that it is there
  if (!dstNode)
  {
    std::cout << "DST Node missing, quitting" << std::endl;
    throw std::runtime_error("failed to find DST node in RawClusterPositionCorrectionFull::CreateNodeTree");
  }

  // Get the _det_name subnode
  PHCompositeNode *cemcNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", _det_name));

  // Check that it is there
  if (!cemcNode)
  {
    cemcNode = new PHCompositeNode(_det_name);
    dstNode->addNode(cemcNode);
  }

  // Check to see if the cluster recalib node is on the nodetree
  _recalib_clusters = findNode::getClass<RawClusterContainer>(topNode, "CLUSTER_RECALIB_" + _det_name);

  // If not, make it and add it to the _det_name subnode
  if (!_recalib_clusters)
  {
    _recalib_clusters = new RawClusterContainer();
    PHIODataNode<PHObject> *clusterNode = new PHIODataNode<PHObject>(_recalib_clusters, "CLUSTER_POS_COR_" + _det_name, "PHObject");
    cemcNode->addNode(clusterNode);
  }

  // put the recalib parameters on the node tree
  PHCompositeNode *parNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "RUN"));
  assert(parNode);
  const std::string paramNodeName = std::string("eclus_Recalibration_" + _det_name);
  _eclus_calib_params.SaveToNodeTree(parNode, paramNodeName);
  const std::string paramNodeName2 = std::string("ecore_Recalibration_" + _det_name);
  _ecore_calib_params.SaveToNodeTree(parNode, paramNodeName2);
}
int RawClusterPositionCorrectionFull::End(PHCompositeNode * /*topNode*/)
{
  return Fun4AllReturnCodes::EVENT_OK;
}
void RawClusterPositionCorrectionFull::SetDefaultParameters(PHParameters &param)
{
  param.set_int_param("number_of_bins_eta", bins_eta);
  param.set_int_param("number_of_bins_phi", bins_phi);

  std::ostringstream param_name;
  for (int i = 0; i < bins_eta - 1; i++)
  {
    for (int j = 0; j < bins_phi - 1; j++)
    {
      param_name.str("");
      param_name << "recalib_const_eta" << i << "_phi" << j;

      // default to 1, i.e. no recalibration
      param.set_double_param(param_name.str(), 1.0);
    }
  }
}
