#include "RawClusterPositionCorrectionFull.h"

#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawTowerGeomContainer.h>

#include <phparameter/PHParameters.h>

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/SubsysReco.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHObject.h>
#include <phool/getClass.h>

#include <CLHEP/Vector/ThreeVector.h>

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include <math.h>

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
  // there are 24 blocks in eta and each block is consists of 4 towers in 2x2 form which results in 24x2=48 towers in eta
  // we further subdivide each tower into 8x8 which results in 48x8=384 correction factors in eta
  for (int j = 0; j < bins_eta; j++)
  {
    // binvals_eta.push_back(j * tower_eta_mid /(bins_eta - 1.));
    binvals_eta.push_back(j * cluster_eta_max /(bins_eta - 1.));
  }

  // set bin boundaries phi
  // there are 4 blocks in phi per sector and each block is consists of 4 towers in 2x2 form which results in 4x2=8 bins in phi
  // we further subdivide each tower into 8x8 which results in 8x8=64 correction factors in phi
  for (int j = 0; j < bins_phi; j++)
  {
    binvals_phi.push_back(sector_phi_boundary_low + j * sector_phi_boundary_width / (bins_phi - 1.));
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

  // loop over the clusters
  RawClusterContainer::ConstRange begin_end = rawclusters->getClusters();
  RawClusterContainer::ConstIterator iter;

  for (iter = begin_end.first; iter != begin_end.second; ++iter)
  {
    RawCluster *cluster = iter->second;

    double clus_energy = cluster->get_energy();
    double phi = cluster->get_phi();
    double eta = abs(RawClusterUtility::GetPseudorapidity(*cluster, CLHEP::Hep3Vector(0,0,0)));
    // accounts for floating point error
    if(phi >= M_PI || phi < -M_PI) phi = -M_PI;
    if(eta >= cluster_eta_max) eta = cluster_eta_max;

    // map phi [-pi, pi) -> [-pi/32, pi/32)
    double fmodphi = phi + ceil((sector_phi_boundary_low - phi) / sector_phi_boundary_width) * sector_phi_boundary_width;

    // accounts for floating point error
    if(fmodphi < sector_phi_boundary_low || fmodphi >= sector_phi_boundary_high) fmodphi = sector_phi_boundary_low;

    int etabin = -99;
    int phibin = -99;

    for (int j = 0; j < bins_phi - 1; ++j) // TODO: Implement in binary search.
      if (fmodphi >= binvals_phi.at(j) && fmodphi < binvals_phi.at(j + 1))
        phibin = j;

    for (int j = 0; j < bins_eta - 1; ++j)
      if (eta >= binvals_eta.at(j) && eta < binvals_eta.at(j + 1))
        etabin = j;

    if(eta == cluster_eta_max) etabin = bins_eta-2;

    if ((phibin < 0 || etabin < 0) && Verbosity())
    {
      if (Verbosity())
        std::cout << "couldn't recalibrate cluster, something went wrong??" << std::endl;
    }

    double eclus_recalib_val = 1;
    double ecore_recalib_val = 1;
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
