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



  // --------
  // --- CEMC
  // --------

  // --- make new cluster object
  RawClusterContainer* new_cemc_clusters = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC_MOD"); // this node is created in createnode
  if ( verbosity > 0 )
    {
      cout << "Regular clusters node: " << clustersEM3 << endl;
      cout << "Modified clusters node: " << new_cemc_clusters << endl;
    }
  if ( !clustersEM3 || !new_cemc_clusters )
    {
      cout << "One or more invalid pointers, exiting event" << endl;
      return 0;
    }

  // --- print sizes of old and new objects for diagnostic purposes
  if ( verbosity > 0 )
    {
      cout << "process_event: entering with # original clusters = " << clustersEM3->size() << endl;
      cout << "process_event: entering with # new clusters = " << new_cemc_clusters->size() << endl;
    }

  // --- loop over cemc clusters
  RawClusterContainer::Range cemc_range = clustersEM3->getClusters();
  for ( RawClusterContainer::Iterator cemc_iter = cemc_range.first; cemc_iter != cemc_range.second; ++cemc_iter )
    {
      // --- get the current cluster
      RawCluster* old_cluster = cemc_iter->second;
      double energy = old_cluster->get_energy();
      //double eta    = old_cluster->get_eta(); // no longer a member of RawCluster class. why?
      double r      = old_cluster->get_r();
      double z      = old_cluster->get_z();
      double phi    = old_cluster->get_phi();
      double ecore  = old_cluster->get_ecore();
      double chi2   = old_cluster->get_chi2();
      double prob   = old_cluster->get_prob();
      if ( verbosity > 10 )
        {
          cout << "for old cluster:"  << endl;
          cout << "energy " << energy << endl;
          //cout << "eta    " << eta    << endl;
          cout << "r      " << r      << endl;
          cout << "z      " << z      << endl;
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
      //new_cluster->set_eta(eta);
      new_cluster->set_r(r);
      new_cluster->set_z(z);
      new_cluster->set_phi(phi);
      new_cluster->set_ecore(ecore);
      new_cluster->set_chi2(chi2);
      new_cluster->set_prob(prob);
      // --- add the new cluster to the new cluster container
      new_cemc_clusters->AddCluster(new_cluster);
    }



  // --------
  // --- HCALIN
  // --------

  // --- make new cluster object
  RawClusterContainer* new_hcalin_clusters = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_HCALIN_MOD"); // this node is created in createnode
  if ( verbosity > 0 )
    {
      cout << "Regular clusters node: " << clustersIH3 << endl;
      cout << "Modified clusters node: " << new_hcalin_clusters << endl;
    }
  if ( !clustersIH3 || !new_hcalin_clusters )
    {
      cout << "One or more invalid pointers, exiting event" << endl;
      return 0;
    }

  // --- print sizes of old and new objects for diagnostic purposes
  if ( verbosity > 0 )
    {
      cout << "process_event: entering with # original clusters = " << clustersIH3->size() << endl;
      cout << "process_event: entering with # new clusters = " << new_hcalin_clusters->size() << endl;
    }

  // --- loop over hcalin clusters
  RawClusterContainer::Range hcalin_range = clustersIH3->getClusters();
  for ( RawClusterContainer::Iterator hcalin_iter = hcalin_range.first; hcalin_iter != hcalin_range.second; ++hcalin_iter )
    {
      // --- get the current cluster
      RawCluster* old_cluster = hcalin_iter->second;
      double energy = old_cluster->get_energy();
      //double eta    = old_cluster->get_eta();
      double r      = old_cluster->get_r();
      double z      = old_cluster->get_z();
      double phi    = old_cluster->get_phi();
      double ecore  = old_cluster->get_ecore();
      double chi2   = old_cluster->get_chi2();
      double prob   = old_cluster->get_prob();
      if ( verbosity > 10 )
        {
          cout << "for old cluster:"  << endl;
          cout << "energy " << energy << endl;
          //cout << "eta    " << eta    << endl;
          cout << "r      " << r      << endl;
          cout << "z      " << z      << endl;
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
      //new_cluster->set_eta(eta);
      new_cluster->set_r(r);
      new_cluster->set_z(z);
      new_cluster->set_phi(phi);
      new_cluster->set_ecore(ecore);
      new_cluster->set_chi2(chi2);
      new_cluster->set_prob(prob);
      // --- add the new cluster to the new cluster container
      new_hcalin_clusters->AddCluster(new_cluster);
    }



  // --------
  // --- HCALOUT
  // --------

  // --- make new cluster object
  RawClusterContainer* new_hcalout_clusters = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_HCALOUT_MOD"); // this node is created in createnode
  if ( verbosity > 0 )
    {
      cout << "Regular clusters node: " << clustersOH3 << endl;
      cout << "Modified clusters node: " << new_hcalout_clusters << endl;
    }
  if ( !clustersOH3 || !new_hcalout_clusters )
    {
      cout << "One or more invalid pointers, exiting event" << endl;
      return 0;
    }

  // --- print sizes of old and new objects for diagnostic purposes
  if ( verbosity > 0 )
    {
      cout << "process_event: entering with # original clusters = " << clustersOH3->size() << endl;
      cout << "process_event: entering with # new clusters = " << new_hcalout_clusters->size() << endl;
    }

  // --- loop over hcalout clusters
  RawClusterContainer::Range hcalout_range = clustersOH3->getClusters();
  for ( RawClusterContainer::Iterator hcalout_iter = hcalout_range.first; hcalout_iter != hcalout_range.second; ++hcalout_iter )
    {
      // --- get the current cluster
      RawCluster* old_cluster = hcalout_iter->second;
      double energy = old_cluster->get_energy();
      //double eta    = old_cluster->get_eta();
      double r      = old_cluster->get_r();
      double z      = old_cluster->get_z();
      double phi    = old_cluster->get_phi();
      double ecore  = old_cluster->get_ecore();
      double chi2   = old_cluster->get_chi2();
      double prob   = old_cluster->get_prob();
      if ( verbosity > 10 )
        {
          cout << "for old cluster:"  << endl;
          cout << "energy " << energy << endl;
          //cout << "eta    " << eta    << endl;
          cout << "r      " << r      << endl;
          cout << "z      " << z      << endl;
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
      //new_cluster->set_eta(eta);
      new_cluster->set_r(r);
      new_cluster->set_z(z);
      new_cluster->set_phi(phi);
      new_cluster->set_ecore(ecore);
      new_cluster->set_chi2(chi2);
      new_cluster->set_prob(prob);
      // --- add the new cluster to the new cluster container
      new_hcalout_clusters->AddCluster(new_cluster);
    }



  if ( verbosity > 0 )
    {
      cout << "process_event: exiting with # original cemc clusters = " << clustersEM3->size() << endl;
      cout << "process_event: exiting with # new cemc clusters = " << new_cemc_clusters->size() << endl;
      cout << "process_event: exiting with # original hcalin clusters = " << clustersIH3->size() << endl;
      cout << "process_event: exiting with # new hcalin clusters = " << new_hcalin_clusters->size() << endl;
      cout << "process_event: exiting with # original hcalout clusters = " << clustersOH3->size() << endl;
      cout << "process_event: exiting with # new hcalout clusters = " << new_hcalout_clusters->size() << endl;
    }

  return 0;

}
