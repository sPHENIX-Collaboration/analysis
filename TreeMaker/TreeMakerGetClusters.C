#include <TreeMaker.h>

#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>

// --- calorimeter towers
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>

// --- calorimeter clusters
#include <calobase/RawCluster.h>
#include <calobase/RawClusterv1.h>
#include <calobase/RawClusterContainer.h>

using std::cout;
using std::endl;



int TreeMaker::GetClusters(PHCompositeNode *topNode)
{

  // -----------------------------------------------------------------------------------------------------
  // ---
  // -----------------------------------------------------------------------------------------------------

  // --- calorimeter tower containers
  // RawTowerContainer* cemc_towers = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC");
  // RawTowerContainer* hcalin_towers = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALIN");
  // RawTowerContainer* hcalout_towers = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALOUT");

  // --- calorimeter geometry objects
  // RawTowerGeomContainer* cemc_geom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
  // RawTowerGeomContainer* hcalin_geom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
  // RawTowerGeomContainer* hcalout_geom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");

  // --- calorimeter cluster containers
  RawClusterContainer* cemc_clusters = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");
  RawClusterContainer* hcalin_clusters = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_HCALIN");
  RawClusterContainer* hcalout_clusters = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_HCALOUT");

  if ( verbosity > 3 )
    {
      cout << "cemc_clusters " << cemc_clusters << endl;
      cout << "hcalin_clusters " << hcalin_clusters << endl;
      cout << "hcalout_clusters " << hcalout_clusters << endl;
    }

  // --- these nodes are created in CreateNode and filled in CopyAndMakeClusters
  RawClusterContainer* new_cemc_clusters = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC_MOD");
  RawClusterContainer* new_hcalin_clusters = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_HCALIN_MOD");
  RawClusterContainer* new_hcalout_clusters = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_HCALOUT_MOD");

  if ( verbosity > 3 )
    {
      cout << "new_cemc_clusters " << new_cemc_clusters << endl;
      cout << "new_hcalin_clusters " << new_hcalin_clusters << endl;
      cout << "new_hcalout_clusters " << new_hcalout_clusters << endl;
    }

  // --- loop over cemc clusters
  double cemc_esum = 0;
  RawClusterContainer::Range cemc_range = cemc_clusters->getClusters();
  for ( RawClusterContainer::Iterator cemc_iter = cemc_range.first; cemc_iter != cemc_range.second; ++cemc_iter )
    {
      // --- get the current cluster
      RawCluster* cluster = cemc_iter->second;
      double energy = cluster->get_energy();
      cemc_esum += energy;
    }

  // --- loop over new_cemc clusters
  double new_cemc_esum = 0;
  RawClusterContainer::Range new_cemc_range = new_cemc_clusters->getClusters();
  for ( RawClusterContainer::Iterator new_cemc_iter = new_cemc_range.first; new_cemc_iter != new_cemc_range.second; ++new_cemc_iter )
    {
      // --- get the current cluster
      RawCluster* cluster = new_cemc_iter->second;
      double energy = cluster->get_energy();
      new_cemc_esum += energy;
    }

  if ( verbosity > 1 )
    {
      cout << "process_event: cemc_esum " << cemc_esum << endl;
      cout << "process_event: new_cemc_esum " << new_cemc_esum << endl;
    }



  return 0;

}
