//This is an analysis package designed to collect single photons
//They can be embedded or not, and can be in either the central
//or forward arms - there are trees for each


#include "Photons.h"

#include <fun4all/Fun4AllServer.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <TLorentzVector.h>
#include <g4jets/Jet.h>
#include <g4jets/JetMap.h>
#include <iostream>

#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>
#include <g4detectors/PHG4ScintillatorSlatContainer.h>
#include <g4eval/JetEvalStack.h>

#include <g4vertex/GlobalVertex.h>
#include <g4vertex/GlobalVertexMap.h>

#include <g4eval/SvtxEvalStack.h>
#include <sstream>

#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>
#include <phhepmc/PHHepMCGenEvent.h>
using namespace std;

#include <cassert>
#include <iostream>

Photons::Photons(const std::string &name)
  : SubsysReco("PHOTONS")
{
  outfilename = name;
  //initialize global variables to -999 so that they have a spot in memory
  initialize_to_zero();

  //add other initializers here


  //default central arm
  _etalow = -1;
  _etahi = 1;

  nevents = 0;

  //default no hijing embedding
  _embed = 0;
}

int Photons::Init(PHCompositeNode *topnode)
{
  file = new TFile(outfilename.c_str(), "RECREATE");

  histo = new TH1F("histo", "histo", 100, -3, 3);

  tree = new TTree("tree", "a tree");
  tree->Branch("nevents", &nevents, "nevents/I");

  //set all the tree branches
  Set_Tree_Branches();

  return 0;
}

int Photons::process_event(PHCompositeNode *topnode)
{
  if (nevents % 10 == 0)
    cout << "at event number " << nevents << endl;



  //get the nodes from the NodeTree
  PHG4TruthInfoContainer *truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topnode, "G4TruthInfo");
  //Raw clusters
  RawClusterContainer *clusters = findNode::getClass<RawClusterContainer>(topnode, "CLUSTER_CEMC");

  //Recalib clusters
  RawClusterContainer *recal_clusters = findNode::getClass<RawClusterContainer>(topnode, "CLUSTER_POS_COR_CEMC");

  RawClusterContainer *fclusters = 0;
  if (_etalow > 1)
  {
    fclusters = findNode::getClass<RawClusterContainer>(topnode, "CLUSTER_FEMC");
  }

  RawClusterContainer *hcalin_clusters = findNode::getClass<RawClusterContainer>(topnode, "CLUSTER_HCALIN");

  RawTowerContainer *_towers = findNode::getClass<RawTowerContainer>(topnode, "TOWER_CALIB_CEMC");

  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topnode, "GlobalVertexMap");
  if (!vertexmap)
  {
    cout << "Photons::process_event - Fatal Error - GlobalVertexMap node is missing. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << endl;
    assert(vertexmap);  // force quit

    return 0;
  }

  if (vertexmap->empty())
  {
    cout << "Photons::process_event - Fatal Error - GlobalVertexMap node is empty. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << endl;
    return 0;
  }

  GlobalVertex *vtx = vertexmap->begin()->second;
  if (vtx == nullptr) return 0;

  if (!recal_clusters)
  {
    cout << "no recalibrated cemc clusters, bailing" << endl;
    return 0;
  }

  if (!_towers)
  {
    cout << "NO CALIBRATED CEMC TOWERS, BAILING" << endl;
    return 0;
  }
  if (!fclusters && _etalow > 1)
  {
    cout << "not forward cluster info" << endl;
    return 0;
  }
  if (!truthinfo)
  {
    cout << "no truth track info" << endl;
    return 0;
  }
  if (!clusters)
  {
    cout << "no cluster info" << endl;
    return 0;
  }

  if (!hcalin_clusters)
  {
    cout << "no hcal in cluster info" << endl;
    return 0;
  }

  if (Verbosity() > 1)
  {
    cout << "Getting truth particles" << endl;
  }

  PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();

  for (PHG4TruthInfoContainer::ConstIterator iter = range.first; iter != range.second; ++iter)
  {
    PHG4Particle *truth = iter->second;

    const int this_embed_id = truthinfo->isEmbeded(truth->get_track_id());

    //check that the truth particle is from the event generator
    //and not from the hijing background.
    //if it is from the hijing background this_embed_id == 0
    if (this_embed_id != 1 && _embed)
      continue;
    truthpid = truth->get_pid();
    truthpx = truth->get_px();
    truthpy = truth->get_py();
    truthpz = truth->get_pz();
    truthp = sqrt(truthpx * truthpx + truthpy * truthpy + truthpz * truthpz);
    truthenergy = truth->get_e();
 
    TLorentzVector vec;
    vec.SetPxPyPzE(truthpx, truthpy, truthpz, truthenergy);

    truthpt = sqrt(truthpx * truthpx + truthpy * truthpy);

    truthphi = vec.Phi();
    trutheta = vec.Eta();

    truth_g4particles->Fill();
  }

  /***********************************************

  GET THE HCAL_INNER CLUSTERS
  (for looking for leakage between emcal towers and/or tunneling)
  This map not be necessary in the future if we don't have an 
  inner HCal :/
  ************************************************/

  RawClusterContainer::ConstRange begin_end_hcal = hcalin_clusters->getClusters();
  RawClusterContainer::ConstIterator hcaliter;

  if (Verbosity() > 1)
  {
    cout << "Getting inner HCal clusters for energy leakage studies" << endl;
  }

  for (hcaliter = begin_end_hcal.first;
       hcaliter != begin_end_hcal.second; 
       ++hcaliter)
  {
    RawCluster *cluster = hcaliter->second;
    CLHEP::Hep3Vector vertex(vtx->get_x(), vtx->get_y(), vtx->get_z());
    CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetEVec(*cluster, vertex);
    hcal_energy = E_vec_cluster.mag();
    hcal_eta = E_vec_cluster.pseudoRapidity();
    hcal_theta = E_vec_cluster.getTheta();
    hcal_pt = E_vec_cluster.perp();
    hcal_phi = E_vec_cluster.getPhi();

    if (hcal_pt < 0.5)
      continue;

    TLorentzVector *clus = new TLorentzVector();
    clus->SetPtEtaPhiE(hcal_pt, hcal_eta, hcal_phi, hcal_energy);
    float dumarray[4];
    clus->GetXYZT(dumarray);
    hcal_x = dumarray[0];
    hcal_y = dumarray[1];
    hcal_z = dumarray[2];
    hcal_t = dumarray[3];

    hcal_px = hcal_pt * TMath::Cos(hcal_phi);
    hcal_py = hcal_pt * TMath::Sin(hcal_phi);
    hcal_pz = sqrt(hcal_energy * hcal_energy - hcal_px * hcal_px * hcal_py * hcal_py);

    //find the associated truth high pT photon with this reconstructed 
    //hcal cluster

    for (PHG4TruthInfoContainer::ConstIterator iter = range.first; iter != range.second; ++iter)
    {
      PHG4Particle *truth = iter->second;

      hclustruthpid = truth->get_pid();
      if (hclustruthpid == 22)
      {
        hclustruthpx = truth->get_px();
        hclustruthpy = truth->get_py();
        hclustruthpz = truth->get_pz();
        hclustruthenergy = truth->get_e();
        hclustruthpt = sqrt(clustruthpx * clustruthpx + clustruthpy * clustruthpy);
        if (hclustruthpt < 0.5)
          continue;

        TLorentzVector vec;
        vec.SetPxPyPzE(hclustruthpx, hclustruthpy, hclustruthpz, hclustruthenergy);
        hclustruthphi = vec.Phi();
        hclustrutheta = vec.Eta();
        //once found it break out
        break;
      }
    }

    inhcal_tree->Fill();
  }

  /***********************************************

  GET THE FORWARD EMCAL CLUSTERS

  ************************************************/

  if (_etalow > 1)
  {
    if (Verbosity() > 1)
    {
      cout << "getting forward EMCal clusters" << endl;
    }

    RawClusterContainer::ConstRange fclus = fclusters->getClusters();
    RawClusterContainer::ConstIterator fclusiter;

    for (fclusiter = fclus.first; 
	 fclusiter != fclus.second; 
	 ++fclusiter)
    {
      RawCluster *cluster = fclusiter->second;

      CLHEP::Hep3Vector vertex(vtx->get_x(), vtx->get_y(), vtx->get_z());
      CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetEVec(*cluster, vertex);
      fclusenergy = E_vec_cluster.mag();
      fclus_eta = E_vec_cluster.pseudoRapidity();
      fclus_theta = E_vec_cluster.getTheta();
      fclus_pt = E_vec_cluster.perp();
      fclus_phi = E_vec_cluster.getPhi();

      fclus_px = fclus_pt * TMath::Cos(fclus_phi);
      fclus_py = fclus_pt * TMath::Sin(fclus_phi);
      fclus_pz = fclusenergy * TMath::Cos(fclus_theta);



      //find the truth photon that corresponds to this event
      for (PHG4TruthInfoContainer::ConstIterator fiter = range.first; 
	   fiter != range.second; 
	   ++fiter)
      {
        PHG4Particle *truth = fiter->second;

        fclustruthpid = truth->get_pid();
	//can run for photons or electrons
        if (fclustruthpid == 22 || abs(fclustruthpid) == 11)
        {
          fclustruthpx = truth->get_px();
          fclustruthpy = truth->get_py();
          fclustruthpz = truth->get_pz();
          fclustruthenergy = truth->get_e();
          fclustruthpt = sqrt(fclustruthpx * fclustruthpx + fclustruthpy * fclustruthpy);
          if (fclustruthpt < 0.3)
            continue;

          TLorentzVector vec;
          vec.SetPxPyPzE(fclustruthpx, fclustruthpy, fclustruthpz, fclustruthenergy);
          fclustruthphi = vec.Phi();
          fclustrutheta = vec.Eta();
          //once found it break out
          break;
        }
      }

      fcluster_tree->Fill();
    }
  }






  /***********************************************

  GET THE POSITION RECALIBRATED EMCAL CLUSTERS

  ************************************************/

  RawClusterContainer::ConstRange rbegin_end = recal_clusters->getClusters();
  RawClusterContainer::ConstIterator rclusiter;

  if (Verbosity() > 1)
    cout << "Getting the position recalibrated clusters" << endl;

  //check that we are analyzing central arms
  //position correction doesn't exist for forward arms
  if (_etahi < 1.1)
  {
    for (rclusiter = rbegin_end.first; 
	 rclusiter != rbegin_end.second; 
	 ++rclusiter)
      {

      RawCluster *cluster = rclusiter->second;

      CLHEP::Hep3Vector vertex(vtx->get_x(), vtx->get_y(), vtx->get_z());
      CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetEVec(*cluster, vertex);
      rclus_energy = E_vec_cluster.mag();
      rclus_eta = E_vec_cluster.pseudoRapidity();
      rclus_theta = E_vec_cluster.getTheta();
      rclus_pt = E_vec_cluster.perp();
      rclus_phi = E_vec_cluster.getPhi();

      if (rclus_pt < mincluspt)
        continue;
      if(Verbosity() > 1)
	cout<<"passed recal pt cut"<<endl;
      TLorentzVector *clus = new TLorentzVector();
      clus->SetPtEtaPhiE(rclus_pt, rclus_eta, rclus_phi, rclus_energy);

      float dumarray[4];
      clus->GetXYZT(dumarray);
      rclus_x = dumarray[0];
      rclus_y = dumarray[1];
      rclus_z = dumarray[2];
      rclus_t = dumarray[3];

      rclus_px = rclus_pt * TMath::Cos(rclus_phi);
      rclus_py = rclus_pt * TMath::Sin(rclus_phi);
      rclus_pz = sqrt(rclus_energy * rclus_energy - rclus_px * rclus_px - rclus_py * rclus_py);
      rclus_ecore = cluster->get_ecore();
      rclus_prob = cluster->get_prob();
      rclus_chi2 = cluster->get_chi2();

      //find the associated truth high pT photon with this reconstructed photon

      for (PHG4TruthInfoContainer::ConstIterator iter = range.first; iter != range.second; ++iter)
      {
        PHG4Particle *truth = iter->second;

        clustruthpid = truth->get_pid();
        const int this_embed_id = truthinfo->isEmbeded(truth->get_track_id());
        
	//if it is embedded make sure that it is from the event 
	//generation and not the HIJING background
	if (this_embed_id != 1 && _embed)
          continue;

	//can run for photons or electrons in the EMCal
        if (clustruthpid == 22 || fabs(clustruthpid) == 11)
        {
          clustruthpx = truth->get_px();
          clustruthpy = truth->get_py();
          clustruthpz = truth->get_pz();
          clustruthenergy = truth->get_e();
          clustruthpt = sqrt(clustruthpx * clustruthpx + clustruthpy * clustruthpy);

          if (clustruthpt < mincluspt)
            continue;

          TLorentzVector vec;
          vec.SetPxPyPzE(clustruthpx, clustruthpy, clustruthpz, clustruthenergy);
          clustruthphi = vec.Phi();
          clustrutheta = vec.Eta();
          //once found it break out
	  if(Verbosity() > 1)
	    cout<<"found recal truth photon"<<endl;
          break;
        }
      }
      if(Verbosity() > 1)
	cout<<"filling recal cluster tree"<<endl;
      
      recal_cluster_tree->Fill();
      }
  }





  /***********************************************

  GET THE REGULAR, NON POSITION CORRECTED EMCAL CLUSTERS

  ************************************************/

  RawClusterContainer::ConstRange begin_end = clusters->getClusters();
  RawClusterContainer::ConstIterator clusiter;

  if (Verbosity() > 1)
    cout << "Get the non-position recalibrated clusters" << endl;

  for (clusiter = begin_end.first; 
       clusiter != begin_end.second;
       ++clusiter)
  {
   
    RawCluster *cluster = clusiter->second;

    CLHEP::Hep3Vector vertex(vtx->get_x(), vtx->get_y(), vtx->get_z());
    CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetEVec(*cluster, vertex);
    clus_energy = E_vec_cluster.mag();
    clus_eta = E_vec_cluster.pseudoRapidity();
    clus_theta = E_vec_cluster.getTheta();
    clus_pt = E_vec_cluster.perp();
    clus_phi = E_vec_cluster.getPhi();

    clus_ecore = cluster->get_ecore();
    clus_chi2 = cluster->get_chi2();
    clus_prob = cluster->get_prob();

    if (clus_pt < mincluspt)
      continue;

    if(Verbosity() > 1)
      cout<<"passed cluster pt cut"<<endl;

    TLorentzVector *clus = new TLorentzVector();
    clus->SetPtEtaPhiE(clus_pt, clus_eta, clus_phi, clus_energy);

    float dumarray[4];
    clus->GetXYZT(dumarray);
    clus_x = dumarray[0];
    clus_y = dumarray[1];
    clus_z = dumarray[2];
    clus_t = dumarray[3];

    clus_px = clus_pt * TMath::Cos(clus_phi);
    clus_py = clus_pt * TMath::Sin(clus_phi);
    clus_pz = sqrt(clus_energy * clus_energy - clus_px * clus_px - clus_py * clus_py);

    //find the associated truth high pT photon with this reconstructed photon

    for (PHG4TruthInfoContainer::ConstIterator iter = range.first; iter != range.second; ++iter)
    {
      PHG4Particle *truth = iter->second;

      clustruthpid = truth->get_pid();
      const int this_embed_id = truthinfo->isEmbeded(truth->get_track_id());
      if (this_embed_id != 1 && _embed)
        continue;
      if (clustruthpid == 22 || fabs(clustruthpid) == 11)
      {
        clustruthpx = truth->get_px();
        clustruthpy = truth->get_py();
        clustruthpz = truth->get_pz();
        clustruthenergy = truth->get_e();
        clustruthpt = sqrt(clustruthpx * clustruthpx + clustruthpy * clustruthpy);

        if (clustruthpt < mincluspt)
          continue;

        TLorentzVector vec;
        vec.SetPxPyPzE(clustruthpx, clustruthpy, clustruthpz, clustruthenergy);
        clustruthphi = vec.Phi();
        clustrutheta = vec.Eta();
        //once found it break out

        break;
      }
    }

    /***********************************************

   DO THE EMCAL RECALIBRATION (if need be, recals are in database as of January 2018)
   I'm leaving the code here commented out in case it ever needs to be used
   again for new recalibrations e.g. for a new clusterizer

  ************************************************/
    /*
    std::vector<float> toweretas;
    std::vector<float> towerphis;
    std::vector<float> towerenergies;
    
    RawCluster::TowerConstRange towerbegend = cluster->get_towers();
    RawCluster::TowerConstIterator toweriter;
    
    //loop over the towers in the cluster
    for(toweriter=towerbegend.first;
	toweriter!=towerbegend.second;
	++toweriter){
      
      RawTower *tower = _towers->getTower(toweriter->first);
     
      int towereta = tower->get_bineta();
      int towerphi = tower->get_binphi();
      double towerenergy = tower->get_energy();

      //put the etabin, phibin, and energy into the corresponding vectors
      toweretas.push_back(towereta);
      towerphis.push_back(towerphi);
      towerenergies.push_back(towerenergy);

    }

    int ntowers = toweretas.size();
    
    //loop over the towers to determine the energy 
    //weighted eta and phi position of the cluster

    float etamult=0;
    float etasum=0;
    float phimult=0;
    float phisum=0;

    for(int j=0; j<ntowers; j++){
   
      float energymult = towerenergies.at(j)*toweretas.at(j);
      etamult+=energymult;
      etasum+=towerenergies.at(j);

      energymult = towerenergies.at(j)*towerphis.at(j);
      phimult+=energymult;
      phisum+=towerenergies.at(j);
    }
    float avgphi = phimult/phisum;
    float avgeta = etamult/etasum;
  
    fmodphi = fmod(avgphi,2.);
    fmodeta = fmod(avgeta,2.);
   
    

    cluster_tree->Fill();
    

  }



    

  //TOWER GEOMETRY, if needed can use to check the actual limits of the towers

  
   RawTowerGeomContainer *_towergeoms = findNode::getClass<RawTowerGeomContainer>(topnode,"TOWERGEOM_CEMC");

   if(!_towergeoms){
     cout<<"no tower geometry, bailing"<<endl;
     return 0;
   }

   RawTowerGeomContainer::ConstRange towbegend = 
     _towergeoms->get_tower_geometries();
   RawTowerGeomContainer::ConstIterator towiter;
   for(towiter=towbegend.first;
       towiter!=towbegend.second;
       ++towiter){


     RawTowerGeom *geom = towiter->second;

   
     
     double eta = geom->get_eta();
     double phi = geom->get_phi();
     double center_x = geom->get_center_x();
     double center_y = geom->get_center_y();
     double center_z = geom->get_center_z();
     double size_x = geom->get_size_x();
     double size_y = geom->get_size_y();
     double size_z = geom->get_size_z();
    
     cout<<eta<<"   "<<phi<<"   "<<center_x<<","<<center_y<<","<<center_z<<"     "<<size_x<<","<<size_y<<","<<size_z<<endl;
     
    */



    cluster_tree->Fill();
  }

  if(Verbosity() > 1)
    cout<<"Finished event in Photons package"<<endl;

  nevents++;
  tree->Fill();
  return 0;
}

int Photons::End(PHCompositeNode *topnode)
{
  std::cout << " DONE PROCESSING " << endl;

  file->Write();
  file->Close();
  return 0;
}

void Photons::Set_Tree_Branches()
{
  inhcal_tree = new TTree("hcalclustree", "a tree with inner hcal cluster information");
  inhcal_tree->Branch("hcal_energy", &hcal_energy, "hcal_energy/F");
  inhcal_tree->Branch("hcal_eta", &hcal_eta, "hcal_eta/F");
  inhcal_tree->Branch("hcal_phi", &hcal_phi, "hcal_phi/F");
  inhcal_tree->Branch("hcal_pt", &hcal_pt, "hcal_pt/F");
  inhcal_tree->Branch("hcal_theta", &hcal_theta, "hcal_theta/F");
  inhcal_tree->Branch("hcal_x", &hcal_x, "hcal_x/F");
  inhcal_tree->Branch("hcal_y", &hcal_y, "hcal_y/F");
  inhcal_tree->Branch("hcal_z", &hcal_z, "hcal_z/F");
  inhcal_tree->Branch("hcal_t", &hcal_t, "hcal_t/F");
  inhcal_tree->Branch("hcal_px", &hcal_px, "hcal_px/F");
  inhcal_tree->Branch("hcal_py", &hcal_py, "hcal_py/F");
  inhcal_tree->Branch("hcal_pz", &hcal_pz, "hcal_pz/F");
  inhcal_tree->Branch("nevents", &nevents, "nevents/I");
  inhcal_tree->Branch("hclustruthpx", &hclustruthpx, "hclustruthpx/F");
  inhcal_tree->Branch("hclustruthpy", &hclustruthpy, "hclustruthpy/F");
  inhcal_tree->Branch("hclustruthpz", &hclustruthpz, "hclustruthpz/F");
  inhcal_tree->Branch("hclustruthenergy", &hclustruthenergy, "hclustruthenergy/F");
  inhcal_tree->Branch("hclustruthpt", &hclustruthpt, "hclustruthpt/F");
  inhcal_tree->Branch("hclustruthphi", &hclustruthphi, "hclustruthphi/F");
  inhcal_tree->Branch("hclustrutheta", &hclustrutheta, "hclustrutheta/F");

  cluster_tree = new TTree("clustertree", "A tree with EMCal cluster information");
  cluster_tree->Branch("clus_energy", &clus_energy, "clus_energy/F");
  cluster_tree->Branch("clus_eta", &clus_eta, "clus_eta/F");
  cluster_tree->Branch("clus_phi", &clus_phi, "clus_phi/F");
  cluster_tree->Branch("clus_pt", &clus_pt, "clus_pt/F");
  cluster_tree->Branch("clus_theta", &clus_theta, "clus_theta/F");
  cluster_tree->Branch("clus_x", &clus_x, "clus_x/F");
  cluster_tree->Branch("clus_y", &clus_y, "clus_y/F");
  cluster_tree->Branch("clus_z", &clus_z, "clus_z/F");
  cluster_tree->Branch("clus_t", &clus_t, "clus_t/F");
  cluster_tree->Branch("clus_px", &clus_px, "clus_px/F");
  cluster_tree->Branch("clus_py", &clus_py, "clus_py/F");
  cluster_tree->Branch("clus_pz", &clus_pz, "clus_pz/F");
  cluster_tree->Branch("nevents", &nevents, "nevents/I");
  cluster_tree->Branch("clus_ecore", &clus_ecore, "clus_ecore/F");
  cluster_tree->Branch("clus_chi2", &clus_chi2, "clus_chi2/F");
  cluster_tree->Branch("clustruthpx", &clustruthpx, "clustruthpx/F");
  cluster_tree->Branch("clustruthpy", &clustruthpy, "clustruthpy/F");
  cluster_tree->Branch("clustruthpz", &clustruthpz, "clustruthpz/F");
  cluster_tree->Branch("clustruthenergy", &clustruthenergy, "clustruthenergy/F");
  cluster_tree->Branch("clustruthpt", &clustruthpt, "clustruthpt/F");
  cluster_tree->Branch("clustruthphi", &clustruthphi, "clustruthphi/F");
  cluster_tree->Branch("clustrutheta", &clustrutheta, "clustrutheta/F");
  cluster_tree->Branch("fmodphi", &fmodphi, "fmodphi/F");
  cluster_tree->Branch("fmodeta", &fmodeta, "fmodeta/F");

  recal_cluster_tree = new TTree("recalclustertree", "A tree with EMCal recalib cluster information");
  recal_cluster_tree->Branch("rclus_energy", &rclus_energy, "rclus_energy/F");
  recal_cluster_tree->Branch("rclus_eta", &rclus_eta, "rclus_eta/F");
  recal_cluster_tree->Branch("rclus_phi", &rclus_phi, "rclus_phi/F");
  recal_cluster_tree->Branch("rclus_pt", &rclus_pt, "rclus_pt/F");
  recal_cluster_tree->Branch("rclus_theta", &rclus_theta, "rclus_theta/F");
  recal_cluster_tree->Branch("rclus_ecore", &rclus_ecore, "rclus_ecore/F");
  recal_cluster_tree->Branch("rclus_chi2", &rclus_chi2, "rclus_chi2/F");
  recal_cluster_tree->Branch("rclus_x", &rclus_x, "rclus_x/F");
  recal_cluster_tree->Branch("rclus_y", &rclus_y, "rclus_y/F");
  recal_cluster_tree->Branch("rclus_z", &rclus_z, "rclus_z/F");
  recal_cluster_tree->Branch("rclus_t", &rclus_t, "rclus_t/F");
  recal_cluster_tree->Branch("rclus_px", &rclus_px, "rclus_px/F");
  recal_cluster_tree->Branch("rclus_py", &rclus_py, "rclus_py/F");
  recal_cluster_tree->Branch("rclus_pz", &rclus_pz, "rclus_pz/F");
  recal_cluster_tree->Branch("nevents", &nevents, "nevents/I");
  recal_cluster_tree->Branch("clustruthpx", &clustruthpx, "clustruthpx/F");
  recal_cluster_tree->Branch("clustruthpy", &clustruthpy, "clustruthpy/F");
  recal_cluster_tree->Branch("clustruthpz", &clustruthpz, "clustruthpz/F");
  recal_cluster_tree->Branch("clustruthenergy", &clustruthenergy, "clustruthenergy/F");
  recal_cluster_tree->Branch("clustruthpt", &clustruthpt, "clustruthpt/F");
  recal_cluster_tree->Branch("clustruthphi", &clustruthphi, "clustruthphi/F");
  recal_cluster_tree->Branch("clustrutheta", &clustrutheta, "clustrutheta/F");

  fcluster_tree = new TTree("fclustertree", "A tree with FEMCal cluster information");
  fcluster_tree->Branch("fclusenergy", &fclusenergy, "fclusenergy/F");
  fcluster_tree->Branch("fclus_eta", &fclus_eta, "fclus_eta/F");
  fcluster_tree->Branch("fclus_phi", &fclus_phi, "fclus_phi/F");
  fcluster_tree->Branch("fclus_pt", &fclus_pt, "fclus_pt/F");
  fcluster_tree->Branch("fclus_theta", &fclus_theta, "fclus_theta/F");
  fcluster_tree->Branch("fclus_px", &fclus_px, "fclus_px/F");
  fcluster_tree->Branch("fclus_py", &fclus_py, "fclus_py/F");
  fcluster_tree->Branch("fclus_pz", &fclus_pz, "fclus_pz/F");
  fcluster_tree->Branch("nevents", &nevents, "nevents/I");
  fcluster_tree->Branch("fclustruthpx", &fclustruthpx, "fclustruthpx/F");
  fcluster_tree->Branch("fclustruthpy", &fclustruthpy, "fclustruthpy/F");
  fcluster_tree->Branch("fclustruthpz", &fclustruthpz, "fclustruthpz/F");
  fcluster_tree->Branch("fclustruthenergy", &fclustruthenergy, "fclustruthenergy/F");
  fcluster_tree->Branch("fclustruthpt", &fclustruthpt, "fclustruthpt/F");
  fcluster_tree->Branch("fclustruthphi", &fclustruthphi, "fclustruthphi/F");
  fcluster_tree->Branch("fclustrutheta", &fclustrutheta, "fclustrutheta/F");

  truth_g4particles = new TTree("truthtree_g4", "a tree with all truth g4 particles");
  truth_g4particles->Branch("truthpx", &truthpx, "truthpx/F");
  truth_g4particles->Branch("truthpy", &truthpy, "truthpy/F");
  truth_g4particles->Branch("truthpz", &truthpz, "truthpz/F");
  truth_g4particles->Branch("truthp", &truthp, "truthp/F");
  truth_g4particles->Branch("truthenergy", &truthenergy, "truthenergy/F");
  truth_g4particles->Branch("truthphi", &truthphi, "truthphi/F");
  truth_g4particles->Branch("trutheta", &trutheta, "trutheta/F");
  truth_g4particles->Branch("truthpt", &truthpt, "truthpt/F");
  truth_g4particles->Branch("truthpid", &truthpid, "truthpid/I");
  truth_g4particles->Branch("nevents", &nevents, "nevents/I");
}

void Photons::initialize_to_zero()
{
  file = 0;
  tree = 0;
  cluster_tree = 0;
  truth_g4particles = 0;
  inhcal_tree = 0;
  fcluster_tree = 0;

  recal_cluster_tree = 0;
  rclus_energy = 0;
  rclus_eta = 0;
  rclus_phi = 0;
  rclus_pt = 0;
  rclus_px = 0;
  rclus_pz = 0;
  rclus_py = 0;
  rclus_theta = 0;
  rclus_x = 0;
  rclus_y = 0;
  rclus_z = 0;
  rclus_t = 0;

  nevents = 0;
  histo = 0;
  hcal_energy = 0;
  hcal_eta = 0;
  hcal_phi = 0;
  hcal_pt = 0;
  hcal_px = 0;
  hcal_py = 0;
  hcal_pz = 0;
  hcal_theta = 0;
  hcal_x = 0;
  hcal_y = 0;
  hcal_z = 0;
  hcal_t = 0;

  clus_energy = 0;
  clus_eta = 0;
  clus_phi = 0;
  clus_pt = 0;
  clus_px = 0;
  clus_py = 0;
  clus_pz = 0;
  clus_theta = 0;
  clus_x = 0;
  clus_y = 0;
  clus_z = 0;
  clus_t = 0;
  fmodphi = 0;
  fmodeta = 0;

  truthpx = 0;
  truthpy = 0;
  truthpz = 0;
  truthp = 0;
  truthphi = 0;
  trutheta = 0;
  truthpt = 0;
  truthenergy = 0;
  truthpid = 0;
  numparticlesinevent = 0;
  process_id = 0;

  clustruthpx = 0;
  clustruthpy = 0;
  clustruthpz = 0;
  clustruthenergy = 0;
  clustruthpt = 0;
  clustruthphi = 0;
  clustrutheta = 0;
  clustruthpid = 0;
  hclustruthpx = 0;
  hclustruthpy = 0;
  hclustruthpz = 0;
  hclustruthenergy = 0;
  hclustruthpt = 0;
  hclustruthphi = 0;
  hclustrutheta = 0;
  hclustruthpid = 0;

  fclusenergy = 0;
  fclus_eta = 0;
  fclus_phi = 0;
  fclus_theta = 0;
  fclus_pt = 0;
  fclustruthpid = 0;
  fclustruthpx = 0;
  fclustruthpy = 0;
  fclustruthpz = 0;
  fclustruthenergy = 0;
  fclustruthpt = 0;
  fclustruthphi = 0;
  fclustrutheta = 0;
  fclus_px = 0;
  fclus_py = 0;
  fclus_pz = 0;
}
