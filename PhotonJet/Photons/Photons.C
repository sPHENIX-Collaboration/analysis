#include "Photons.h"

#include <phool/getClass.h>
#include <fun4all/Fun4AllServer.h>
#include <phool/PHCompositeNode.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4Hit.h>

#include <TLorentzVector.h>
#include <iostream>
#include <g4jets/JetMap.h>
#include <g4jets/Jet.h>
#include <g4cemc/RawTowerContainer.h>
#include <g4cemc/RawTowerGeomContainer.h>
#include <g4cemc/RawTower.h>
#include <g4cemc/RawTowerGeom.h>

#include <g4cemc/RawClusterContainer.h>
#include <g4cemc/RawCluster.h>

#include <g4detectors/PHG4ScintillatorSlatContainer.h>
#include <g4eval/JetEvalStack.h>
#include <g4hough/SvtxTrackMap.h>
#include <g4hough/SvtxTrack.h>

#include <g4eval/SvtxEvalStack.h>
#include <sstream>

#include <phhepmc/PHHepMCGenEvent.h>
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>
using namespace std;




Photons::Photons(const std::string &name) : SubsysReco("PHOTONS")
{
  outfilename = name;

  //add other initializers here
  //default use isocone algorithm
  use_isocone=1;
  

  //default central arm
  _etalow=-1;
  _etahi=1;

  nevents=0;
  //default use 0.3 jet cone
  jet_cone_size=0.3;
}

int Photons::Init(PHCompositeNode *topnode)
{



  file = new TFile(outfilename.c_str(),"RECREATE");


  histo = new TH1F("histo","histo",100,-3,3);

  tree = new TTree("tree","a tree");
  tree->Branch("nevents",&nevents,"nevents/I");

  Set_Tree_Branches();






  return 0;
}



int Photons::process_event(PHCompositeNode *topnode)
{
 
  cout<<"at event number "<<nevents<<endl;


  //get the nodes from the NodeTree


  PHG4TruthInfoContainer* truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topnode,"G4TruthInfo");
  RawClusterContainer *clusters = findNode::getClass<RawClusterContainer>(topnode,"CLUSTER_CEMC");
  RawClusterContainer *fclusters=0;
  if(_etalow>1){
    fclusters = findNode::getClass<RawClusterContainer>(topnode,"CLUSTER_FEMC");
  }
  PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();


  RawClusterContainer *hcalin_clusters = findNode::getClass<RawClusterContainer>(topnode,"CLUSTER_HCALIN");


  if(!fclusters && _etalow>1){
    cout<<"not forward cluster info"<<endl;
    return 0;
  }
  if(!truthinfo){
      cout<<"no truth track info"<<endl;
      return 0;
    }
  if(!clusters){
    cout<<"no cluster info"<<endl;
    return 0;
  }
  if(!hcalin_clusters){
    cout<<"no hcal in cluster info"<<endl;
    return 0;
  }





 
  for( PHG4TruthInfoContainer::ConstIterator iter = range.first; iter!=range.second; ++iter){
    
    PHG4Particle *truth = iter->second;
    
    truthpx = truth->get_px();
    truthpy = truth->get_py();
    truthpz = truth->get_pz();
    truthp = sqrt(truthpx*truthpx+truthpy*truthpy+truthpz*truthpz);
    truthenergy = truth->get_e();
    
    TLorentzVector vec;
    vec.SetPxPyPzE(truthpx,truthpy,truthpz,truthenergy);
    
    truthpt = sqrt(truthpx*truthpx+truthpy*truthpy);
  
    
    truthphi = vec.Phi();
    trutheta = vec.Eta();
    truthpid = truth->get_pid();
    
    truth_g4particles->Fill();
  }

  
  /***********************************************

  GET THE HCAL_INNER CLUSTERS
  (for looking for leakage between emcal towers and/or tunneling)
  ************************************************/

  RawClusterContainer::ConstRange begin_end_hcal = hcalin_clusters->getClusters();
  RawClusterContainer::ConstIterator hcaliter;

  for(hcaliter = begin_end_hcal.first; hcaliter!=begin_end_hcal.second;++hcaliter){
    RawCluster *cluster = hcaliter->second;

    hcal_energy = cluster->get_energy();
    hcal_eta = cluster->get_eta();
    hcal_theta = 2.*TMath::ATan((TMath::Exp(-1.*clus_eta)));
    hcal_pt = hcal_energy*TMath::Sin(hcal_theta);
    hcal_phi = cluster->get_phi();

    if(hcal_pt<0.2)
      continue;

    TLorentzVector *clus = new TLorentzVector();
    clus->SetPtEtaPhiE(hcal_pt,hcal_eta,hcal_phi,hcal_energy);
    float dumarray[4];
    clus->GetXYZT(dumarray);
    hcal_x = dumarray[0];
    hcal_y = dumarray[1];
    hcal_z = dumarray[2];
    hcal_t = dumarray[3];

    hcal_px = hcal_pt*TMath::Cos(hcal_phi);
    hcal_py = hcal_pt*TMath::Sin(hcal_phi);
    hcal_pz = sqrt(hcal_energy*hcal_energy-hcal_px*hcal_px*hcal_py*hcal_py);

    //find the associated truth high pT photon with this reconstructed photon
  
    for( PHG4TruthInfoContainer::ConstIterator iter = range.first; iter!=range.second; ++iter){
    
      PHG4Particle *truth = iter->second;
      
      hclustruthpid = truth->get_pid();
      if(hclustruthpid==22){
	hclustruthpx = truth->get_px();
	hclustruthpy = truth->get_py();
	hclustruthpz = truth->get_pz();
	hclustruthenergy = truth->get_e();
	hclustruthpt = sqrt(clustruthpx*clustruthpx+clustruthpy*clustruthpy);
	if(hclustruthpt<0.3)
	  continue;
	
	TLorentzVector vec;
	vec.SetPxPyPzE(hclustruthpx,hclustruthpy,hclustruthpz,hclustruthenergy);
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
  if(_etalow>1){
    RawClusterContainer::ConstRange fclus = fclusters->getClusters();
    RawClusterContainer::ConstIterator fclusiter;
    
    for(fclusiter = fclus.first; fclusiter!=fclus.second; ++fclusiter){
      RawCluster *cluster = fclusiter->second;
      
      fclusenergy = cluster->get_energy();
      fclus_eta = cluster->get_eta();
      fclus_phi = cluster->get_phi();
      fclus_theta =  2.*TMath::ATan((TMath::Exp(-1.*fclus_eta))); 
      fclus_pt = fclusenergy*TMath::Sin(fclus_theta);
      
      fclus_px = fclus_pt*TMath::Cos(fclus_phi);
      fclus_py = fclus_pt*TMath::Sin(fclus_phi);
      fclus_pz = fclusenergy*TMath::Cos(fclus_theta);
      
      for( PHG4TruthInfoContainer::ConstIterator fiter = range.first; fiter!=range.second; ++fiter){
	
	PHG4Particle *truth = fiter->second;
	
	fclustruthpid = truth->get_pid();
	if(fclustruthpid==22){
	  fclustruthpx = truth->get_px();
	  fclustruthpy = truth->get_py();
	  fclustruthpz = truth->get_pz();
	  fclustruthenergy = truth->get_e();
	  fclustruthpt = sqrt(fclustruthpx*fclustruthpx+fclustruthpy*fclustruthpy);
	  if(fclustruthpt<0.3)
	    continue;
	  
	  TLorentzVector vec;
	  vec.SetPxPyPzE(fclustruthpx,fclustruthpy,fclustruthpz,fclustruthenergy);
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

  GET THE EMCAL CLUSTERS

  ************************************************/


  RawClusterContainer::ConstRange begin_end = clusters->getClusters();
  RawClusterContainer::ConstIterator clusiter;
  
  for(clusiter = begin_end.first; clusiter!=begin_end.second; ++clusiter){

    RawCluster *cluster = clusiter->second;

    clus_energy = cluster->get_energy();
    clus_eta = cluster->get_eta();
    clus_theta = 2.*TMath::ATan((TMath::Exp(-1.*clus_eta)));
    clus_pt = clus_energy*TMath::Sin(clus_theta);
    clus_phi = cluster->get_phi();
    
  
    if(clus_pt<0.2)
      continue;
   
    
    TLorentzVector *clus = new TLorentzVector();
    clus->SetPtEtaPhiE(clus_pt,clus_eta,clus_phi,clus_energy);
    
    float dumarray[4];
    clus->GetXYZT(dumarray);
    clus_x = dumarray[0];
    clus_y = dumarray[1];
    clus_z = dumarray[2];
    clus_t = dumarray[3];

    clus_px = clus_pt*TMath::Cos(clus_phi);
    clus_py = clus_pt*TMath::Sin(clus_phi);
    clus_pz = sqrt(clus_energy*clus_energy-clus_px*clus_px-clus_py*clus_py);

   

   
    //find the associated truth high pT photon with this reconstructed photon
  
    for( PHG4TruthInfoContainer::ConstIterator iter = range.first; iter!=range.second; ++iter){
    
      PHG4Particle *truth = iter->second;
      
      clustruthpid = truth->get_pid();
      if(clustruthpid==22){
	clustruthpx = truth->get_px();
	clustruthpy = truth->get_py();
	clustruthpz = truth->get_pz();
	clustruthenergy = truth->get_e();
	clustruthpt = sqrt(clustruthpx*clustruthpx+clustruthpy*clustruthpy);
	if(clustruthpt<0.3)
	  continue;
	
	TLorentzVector vec;
	vec.SetPxPyPzE(clustruthpx,clustruthpy,clustruthpz,clustruthenergy);
	clustruthphi = vec.Phi();
	clustrutheta = vec.Eta();
	//once found it break out
	break;

      }
    }

    cluster_tree->Fill();

  }

  nevents++;
  tree->Fill();
  return 0;

}


int Photons::End(PHCompositeNode *topnode)
{
  std::cout<<" DONE PROCESSING "<<endl;

  file->Write();
  file->Close();
  return 0;
}



void Photons::Set_Tree_Branches()
{


  inhcal_tree = new TTree("hcalclustree","a tree with inner hcal cluster information");
  inhcal_tree->Branch("hcal_energy",&hcal_energy,"hcal_energy/F");
  inhcal_tree->Branch("hcal_eta",&hcal_eta,"hcal_eta/F");
  inhcal_tree->Branch("hcal_phi",&hcal_phi,"hcal_phi/F");
  inhcal_tree->Branch("hcal_pt",&hcal_pt,"hcal_pt/F");
  inhcal_tree->Branch("hcal_theta",&hcal_theta,"hcal_theta/F");
  inhcal_tree->Branch("hcal_x",&hcal_x,"hcal_x/F");
  inhcal_tree->Branch("hcal_y",&hcal_y,"hcal_y/F");
  inhcal_tree->Branch("hcal_z",&hcal_z,"hcal_z/F");
  inhcal_tree->Branch("hcal_t",&hcal_t,"hcal_t/F");
  inhcal_tree->Branch("hcal_px",&hcal_px,"hcal_px/F");
  inhcal_tree->Branch("hcal_py",&hcal_py,"hcal_py/F");
  inhcal_tree->Branch("hcal_pz",&hcal_pz,"hcal_pz/F");
  inhcal_tree->Branch("nevents",&nevents,"nevents/I");
  inhcal_tree->Branch("hclustruthpx",&hclustruthpx,"hclustruthpx/F");
  inhcal_tree->Branch("hclustruthpy",&hclustruthpy,"hclustruthpy/F");
  inhcal_tree->Branch("hclustruthpz",&hclustruthpz,"hclustruthpz/F");
  inhcal_tree->Branch("hclustruthenergy",&hclustruthenergy,"hclustruthenergy/F");
  inhcal_tree->Branch("hclustruthpt",&hclustruthpt,"hclustruthpt/F");
  inhcal_tree->Branch("hclustruthphi",&hclustruthphi,"hclustruthphi/F");
  inhcal_tree->Branch("hclustrutheta",&hclustrutheta,"hclustrutheta/F");
  
  cluster_tree = new TTree("clustertree","A tree with EMCal cluster information");
  cluster_tree->Branch("clus_energy",&clus_energy,"clus_energy/F");
  cluster_tree->Branch("clus_eta",&clus_eta,"clus_eta/F");
  cluster_tree->Branch("clus_phi",&clus_phi,"clus_phi/F");
  cluster_tree->Branch("clus_pt",&clus_pt,"clus_pt/F");
  cluster_tree->Branch("clus_theta",&clus_theta,"clus_theta/F");
  cluster_tree->Branch("clus_x",&clus_x,"clus_x/F");
  cluster_tree->Branch("clus_y",&clus_y,"clus_y/F");
  cluster_tree->Branch("clus_z",&clus_z,"clus_z/F");
  cluster_tree->Branch("clus_t",&clus_t,"clus_t/F");
  cluster_tree->Branch("clus_px",&clus_px,"clus_px/F");
  cluster_tree->Branch("clus_py",&clus_py,"clus_py/F");
  cluster_tree->Branch("clus_pz",&clus_pz,"clus_pz/F");
  cluster_tree->Branch("nevents",&nevents,"nevents/I");
  cluster_tree->Branch("clustruthpx",&clustruthpx,"clustruthpx/F");
  cluster_tree->Branch("clustruthpy",&clustruthpy,"clustruthpy/F");
  cluster_tree->Branch("clustruthpz",&clustruthpz,"clustruthpz/F");
  cluster_tree->Branch("clustruthenergy",&clustruthenergy,"clustruthenergy/F");
  cluster_tree->Branch("clustruthpt",&clustruthpt,"clustruthpt/F");
  cluster_tree->Branch("clustruthphi",&clustruthphi,"clustruthphi/F");
  cluster_tree->Branch("clustrutheta",&clustrutheta,"clustrutheta/F");

  fcluster_tree = new TTree("fclustertree","A tree with FEMCal cluster information");
  fcluster_tree->Branch("fclusenergy",&fclusenergy,"fclusenergy/F");
  fcluster_tree->Branch("fclus_eta",&fclus_eta,"fclus_eta/F");
  fcluster_tree->Branch("fclus_phi",&fclus_phi,"fclus_phi/F");
  fcluster_tree->Branch("fclus_pt",&fclus_pt,"fclus_pt/F");
  fcluster_tree->Branch("fclus_theta",&fclus_theta,"fclus_theta/F");
  fcluster_tree->Branch("fclus_px",&fclus_px,"fclus_px/F");
  fcluster_tree->Branch("fclus_py",&fclus_py,"fclus_py/F");
  fcluster_tree->Branch("fclus_pz",&fclus_pz,"fclus_pz/F");
  fcluster_tree->Branch("nevents",&nevents,"nevents/I");
  fcluster_tree->Branch("fclustruthpx",&fclustruthpx,"fclustruthpx/F");
  fcluster_tree->Branch("fclustruthpy",&fclustruthpy,"fclustruthpy/F");
  fcluster_tree->Branch("fclustruthpz",&fclustruthpz,"fclustruthpz/F");
  fcluster_tree->Branch("fclustruthenergy",&fclustruthenergy,"fclustruthenergy/F");
  fcluster_tree->Branch("fclustruthpt",&fclustruthpt,"fclustruthpt/F");
  fcluster_tree->Branch("fclustruthphi",&fclustruthphi,"fclustruthphi/F");
  fcluster_tree->Branch("fclustrutheta",&fclustrutheta,"fclustrutheta/F");



  truth_g4particles = new TTree("truthtree_g4","a tree with all truth g4 particles");
  truth_g4particles->Branch("truthpx",&truthpx,"truthpx/F");
  truth_g4particles->Branch("truthpy",&truthpy,"truthpy/F");
  truth_g4particles->Branch("truthpz",&truthpz,"truthpz/F");
  truth_g4particles->Branch("truthp",&truthp,"truthp/F");
  truth_g4particles->Branch("truthenergy",&truthenergy,"truthenergy/F");
  truth_g4particles->Branch("truthphi",&truthphi,"truthphi/F");
  truth_g4particles->Branch("trutheta",&trutheta,"trutheta/F");
  truth_g4particles->Branch("truthpt",&truthpt,"truthpt/F");
  truth_g4particles->Branch("truthpid",&truthpid,"truthpid/I");
  truth_g4particles->Branch("nevents",&nevents,"nevents/I");
 

 
}



