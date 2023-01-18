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
  , _reco_e_threshold(1.0) // min energy for an accepted cluster
  , bins_eta(384) // store calib location
  , bins_phi(64) // store calib location
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

  bins_eta = _eclus_calib_params.get_int_param(paramname.str());

  paramname.str("");
  paramname << "number_of_bins_phi";

  bins_phi = _eclus_calib_params.get_int_param(paramname.str());

  for (int i = 0; i < bins_eta; i++)
  {
    std::vector<double> dumvec;

    for (int j = 0; j < bins_phi; j++)
    {
      std::ostringstream calib_const_name;
      calib_const_name.str("");
      calib_const_name << "recalib_const_eta" << i << "_phi" << j;
      dumvec.push_back(_eclus_calib_params.get_double_param(calib_const_name.str()));
    }
    eclus_calib_constants.push_back(dumvec);
  }

  for (int i = 0; i < bins_eta; i++)
  {
    std::vector<double> dumvec;

    for (int j = 0; j < bins_phi; j++)
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

  for (RawClusterContainer::ConstIterator iter = begin_end.first; iter != begin_end.second; ++iter)
  {
    RawCluster *cluster = iter->second;

    double clus_energy      = cluster->get_energy();

    // ensure that we have at least a 1 GeV cluster
    if(clus_energy < _reco_e_threshold) continue;

    double x                = 0;
    double y                = 0;
    double avg_tower_eta    = 0;
    double tower_energy_sum = 0;

    // loop over the towers in the cluster
    RawCluster::TowerConstRange towers = cluster->get_towers();

    for (RawCluster::TowerConstIterator toweriter = towers.first; toweriter != towers.second; ++toweriter)
    {
      RawTower* tower = _towers->getTower(toweriter->first);

      double towerenergy = tower->get_energy();

      // do phi calculations
      int towerphi = tower->get_binphi();
      double alpha = towerphi*tower_length_rad + towerphi/8 * sector_boundary_phi_length_rad;

      x += cos(alpha)*towerenergy;
      y += sin(alpha)*towerenergy;

      // do eta calculations
      int towereta = tower->get_bineta();
      tower_energy_sum += towerenergy;

      // to account for the presence of the sector boundary, we must shift the towerid by the sector boundary width if the
      // tower ids are on the right sector.
      if(towereta >= 48) avg_tower_eta += (towereta+sector_boundary_eta_length)*towerenergy;
      else avg_tower_eta += towereta*towerenergy;
    }

    int etabin = -1;
    int phibin = -1;

    // Begin: Find etabin //
    // calculate the weighted averaqe of towerid in eta
    avg_tower_eta /= tower_energy_sum;

    // check if avg_tower_eta is in the sector boundary
    // sector boundary is from 47.5 to 47.5 + sector_boundary_eta_length
    if(avg_tower_eta >= 47.5 && avg_tower_eta < 47.5 + sector_boundary_eta_length) {
       etabin = 0;
    }
    // check if avg_tower_eta is in the right half
    // towerid: 47.5 -> calib: 0 and towerid: 95.5 -> calib: 383
    if(avg_tower_eta >= 47.5 + sector_boundary_eta_length) {
      avg_tower_eta -= sector_boundary_eta_length;
      for(int i = 0; i < bins_eta; ++i){
        if(avg_tower_eta >= 47.5 + i/8.0 && avg_tower_eta < 47.5 + (i+1)/8.0) {
          etabin = i;
          break;
        }
      }
    }
    // else avg_tower_eta must be in the left half
    // towerid: -0.5 -> calib: 383 and towerid: 47.5 -> calib: 0
    else {
      for(int i = 0; i < bins_eta; ++i){
        if(avg_tower_eta > 47.5 - (i+1)/8.0 && avg_tower_eta <= 47.5 - i/8.0 ) {
          etabin = i;
          break;
        }
      }
    }
    // End: Find etabin //

    // Begin: Find phibin //
    double alpha = atan2(y,x); // returns angle in [-pi,pi]
    if(alpha < -0.5*tower_length_rad) alpha += 2*M_PI;

    // search for alpha within each sector and obtain the correct recalibration factor
    for(int i = 0; i < 32; ++i) {
      double tl = 8*i-0.5; // y0
      double th = 8*i+7.5; // y1
      double sl = tl*tower_length_rad + i*sector_boundary_phi_length_rad; // sector low edge, x0
      double sh = th*tower_length_rad + i*sector_boundary_phi_length_rad; // sector high edge, x1

      if(alpha >= sl && alpha < sh) {
        // use linear mapping to find the towerid which corresponds to alpha
        double towerphi = (th-tl)/(sh-sl)*(alpha-sl)+tl; // y = m(x-x0) + y0 where m = (y1-y0)/(x1-x0)
        // find the calibration constant location
        for(int j = 0; j < bins_phi; ++j) {
          if(towerphi >= tl + j/8.0 && towerphi < tl + (j+1)/8.0) {
           phibin = j;
           break;
          }
        }
        break;
      }
    }

    // calibration: phi
    // alpha not found in sector
    // must be in the sector boundary
    if(phibin == -1) {
      for(int i = 0; i < 32; ++i) {
        double sbl = (8*i+7.5)*tower_length_rad + i*sector_boundary_phi_length_rad; // sector boundary low edge
        double sbm = sbl + sector_boundary_phi_length_rad/2; // sector boundary middle
        double sbh = sbl + sector_boundary_phi_length_rad; // sector boundary high edge
        if(alpha >= sbl && alpha < sbm) {
          phibin = bins_phi-1;
          break;
        }
        if(alpha >= sbm && alpha < sbh) {
          phibin = 0;
          break;
        }
      }
    }
    // End: Find phibin //

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
