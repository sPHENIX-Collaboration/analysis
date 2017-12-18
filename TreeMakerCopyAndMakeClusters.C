#include <TreeMaker.h>

#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>

// --- calorimeter towers
#include <g4cemc/RawTower.h>
#include <g4cemc/RawTowerContainer.h>
#include <g4cemc/RawTowerGeom.h>
#include <g4cemc/RawTowerGeomContainer.h>

// --- calorimeter clusters
#include <g4cemc/RawCluster.h>
#include <g4cemc/RawClusterv1.h>
#include <g4cemc/RawClusterContainer.h>

using std::cout;
using std::endl;



int TreeMaker::CopyAndMakeClusters(PHCompositeNode *topNode)
{

  // -----------------------------------------------------------------------------------------------------
  // ---
  // -----------------------------------------------------------------------------------------------------

  // --- calorimeter tower containers
  // RawTowerContainer* towersEM3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC");
  // RawTowerContainer* towersIH3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALIN");
  // RawTowerContainer* towersOH3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALOUT");

  // --- calorimeter geometry objects
  // RawTowerGeomContainer* geomEM = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
  // RawTowerGeomContainer* geomIH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
  // RawTowerGeomContainer* geomOH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");

  // --- clorimeter cluster containers
  RawClusterContainer* clustersEM3 = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");
  RawClusterContainer* clustersIH3 = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_HCALIN");
  RawClusterContainer* clustersOH3 = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_HCALOUT");

  if ( verbosity > 3 )
    {
      cout << "clustersEM3 " << clustersEM3 << endl;
      cout << "clustersIH3 " << clustersIH3 << endl;
      cout << "clustersOH3 " << clustersOH3 << endl;
    }

  // --- make new cluster object
  RawClusterContainer* new_clusters = new RawClusterContainer(); // no node on the tree yet

  // --- loop over cemc clusters
  RawClusterContainer::Range cemc_range = clustersEM3->getClusters();
  for ( RawClusterContainer::Iterator cemc_iter = cemc_range.first; cemc_iter != cemc_range.second; ++cemc_iter )
    {
      // --- get the current cluster
      RawCluster* old_cluster = cemc_iter->second;
      double energy = old_cluster->get_energy();
      double eta    = old_cluster->get_eta();
      double phi    = old_cluster->get_phi();
      double ecore  = old_cluster->get_ecore();
      double chi2   = old_cluster->get_chi2();
      double prob   = old_cluster->get_prob();
      if ( verbosity > 0 )
        {
          cout << "for old cluster:"  << endl;
          cout << "energy " << energy << endl;
          cout << "eta    " << eta    << endl;
          cout << "phi    " << phi    << endl;
          cout << "ecore  " << ecore  << endl;
          cout << "chi2   " << chi2   << endl;
          cout << "prob   " << prob   << endl;
        }
      // --- make the new cluster
      RawCluster* new_cluster = new RawClusterv1();
      // --- set the cluster variables
      new_cluster->set_id(old_cluster->get_id());
      new_cluster->set_energy(energy);
      new_cluster->set_eta(eta);
      new_cluster->set_phi(phi);
      new_cluster->set_ecore(ecore);
      new_cluster->set_chi2(chi2);
      new_cluster->set_prob(prob);
      // --- add the new cluster to the new cluster container
      new_clusters->AddCluster(new_cluster);
    }



  if ( verbosity > 0 )
    {
      cout << "process_event: exiting with # original clusters = " << clustersEM3->size() << endl;
      cout << "process_event: exiting with # new clusters = " << new_clusters->size() << endl;
    }

  return 0;

}
