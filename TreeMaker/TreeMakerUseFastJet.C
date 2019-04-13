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

// --- fast jet
#include <fastjet/ClusterSequence.hh>

// --- stuff
#include <TLorentzVector.h>

using std::cout;
using std::endl;
using std::vector;

using fastjet::PseudoJet;
using fastjet::ClusterSequence;
using fastjet::JetDefinition;
using fastjet::antikt_algorithm;



int TreeMaker::UseFastJet(PHCompositeNode *topNode)
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

  vector<PseudoJet> clusters;
  vector<PseudoJet> mod_clusters;
  double R = 0.4;
  JetDefinition jet_def(antikt_algorithm, R);
  // PseudoJet pj_cluster;
  // PseudoJet pj_mod_cluster;

  // --- loop over cemc clusters
  int cluster_counter = 0;
  RawClusterContainer::Range cemc_range = cemc_clusters->getClusters();
  for ( RawClusterContainer::Iterator cemc_iter = cemc_range.first; cemc_iter != cemc_range.second; ++cemc_iter )
    {
      // --- get the current cluster
      RawCluster* cluster = cemc_iter->second;
      double e   = cluster->get_energy();
      double phi = cluster->get_phi();
      double r   = cluster->get_r();
      double z   = cluster->get_z();
      double eta = -log(tan(atan2(r,z)/2.0));
      double pt  = e/cosh(eta);
      TLorentzVector tlv_cluster;
      tlv_cluster.SetPtEtaPhiE(pt,eta,phi,e);
      clusters.push_back(PseudoJet(tlv_cluster));
      ++cluster_counter;
    }
  ClusterSequence cs_cluster(clusters, jet_def);
  vector<PseudoJet> clusterjets = sorted_by_pt(cs_cluster.inclusive_jets());
  if ( verbosity > 1 ) cout << "number of clusters " << cluster_counter << " number of cluster jets" << clusterjets.size() << endl;

  // --- loop over new_cemc clusters
  int mod_cluster_counter = 0;
  RawClusterContainer::Range new_cemc_range = new_cemc_clusters->getClusters();
  for ( RawClusterContainer::Iterator new_cemc_iter = new_cemc_range.first; new_cemc_iter != new_cemc_range.second; ++new_cemc_iter )
    {
      // --- get the current cluster
      RawCluster* cluster = new_cemc_iter->second;
      double e   = cluster->get_energy();
      double phi = cluster->get_phi();
      double r   = cluster->get_r();
      double z   = cluster->get_z();
      double eta = -log(tan(atan2(r,z)/2.0));
      double pt  = e/cosh(eta);
      TLorentzVector tlv_cluster;
      tlv_cluster.SetPtEtaPhiE(pt,eta,phi,e);
      mod_clusters.push_back(PseudoJet(tlv_cluster));
      ++mod_cluster_counter;
    }
  ClusterSequence cs_mod_cluster(mod_clusters, jet_def);
  vector<PseudoJet> mod_clusterjets = sorted_by_pt(cs_mod_cluster.inclusive_jets());
  if ( verbosity > 1 ) cout << "number of clusters " << cluster_counter << " number of cluster jets" << mod_clusterjets.size() << endl;


  // clusterJet_n = 0;
  // for (unsigned i = 0; i < clusterjets.size(); i++) {
  //   if (clusterjets[i].pt() < 5) continue;
  //   clusterJet_e[i]   = clusterjets[i].e();
  //   clusterJet_pt[i]  = clusterjets[i].pt();
  //   clusterJet_eta[i] = clusterjets[i].eta();
  //   clusterJet_phi[i] = clusterjets[i].phi_std();
  //   vector<PseudoJet> constituents = clusterjets[i].constituents();
  //   clusterJet_nConst[i] = constituents.size();
  //   clusterJet_n++;
  // }

  // if ( verbosity > 1 )
  //   {
  //     cout << "process_event: cemc_esum " << cemc_esum << endl;
  //     cout << "process_event: new_cemc_esum " << new_cemc_esum << endl;
  //   }



  return 0;

}
