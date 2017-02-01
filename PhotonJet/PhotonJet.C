#include "PhotonJet.h"

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




PhotonJet::PhotonJet(const std::string &name) : SubsysReco("PHOTONJET")
{
  outfilename = name;

  //add other initializers here
  //default use isocone algorithm
  use_isocone=1;
  
  //default use 0.3 jet cone
  jet_cone_size=0.3;
}

int PhotonJet::Init(PHCompositeNode *topnode)
{



  file = new TFile(outfilename.c_str(),"RECREATE");
  nevents=0;

  histo = new TH1F("histo","histo",100,-3,3);

  tree = new TTree("tree","a tree");
  tree->Branch("nevents",&nevents,"nevents/I");

  Set_Tree_Branches();






  return 0;
}



int PhotonJet::process_event(PHCompositeNode *topnode)
{
 
  cout<<"at event number "<<nevents<<endl;


  //get the nodes from the NodeTree


  //get the right size jets
  ostringstream truthjetsize;
  ostringstream recojetsize;

  truthjetsize.str("");
  truthjetsize<<"AntiKt_Truth_r";
  recojetsize.str("");
  recojetsize<<"AntiKt_Tower_r";

  if(jet_cone_size==0.2){
    truthjetsize<<"02";
    recojetsize<<"02";
    }
  else if(jet_cone_size==0.3){
    truthjetsize<<"03";
    recojetsize<<"03";
  }
  else if(jet_cone_size==0.4){
    truthjetsize<<"04";
    recojetsize<<"04";
  }
   else if(jet_cone_size==0.5){
    truthjetsize<<"05";
    recojetsize<<"05";
  }
  //if its some other number just set it to 0.4
   else{
     truthjetsize<<"04";
     recojetsize<<"04";
   }

  JetMap* truth_jets = findNode::getClass<JetMap>(topnode,truthjetsize.str().c_str());
  JetMap *reco_jets = findNode::getClass<JetMap>(topnode,recojetsize.str().c_str());
  PHG4TruthInfoContainer* truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topnode,"G4TruthInfo");
  RawClusterContainer *clusters = findNode::getClass<RawClusterContainer>(topnode,"CLUSTER_CEMC");
  SvtxTrackMap *trackmap = findNode::getClass<SvtxTrackMap>(topnode,"SvtxTrackMap");
  
  //for truth jet matching
  JetEvalStack* _jetevalstack = new JetEvalStack(topnode,recojetsize.str().c_str(),truthjetsize.str().c_str());
  PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();

  //for truth track matching
  SvtxEvalStack *svtxevalstack = new SvtxEvalStack(topnode);
  svtxevalstack->next_event(topnode);

  



  if(!truth_jets){
      cout<<"no truth jets"<<endl;
      return 0;
    }
  if(!reco_jets){
      cout<<"no reco jets"<<endl;
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
  if(!trackmap){
    cout<<"no track map"<<endl;
    return 0;
  }
  if(!_jetevalstack){
    cout<<"no good truth jets"<<endl;
    return 0;
  }


  JetRecoEval* recoeval = _jetevalstack->get_reco_eval();
  SvtxTrackEval *trackeval = svtxevalstack->get_track_eval();

 
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

  GET ALL THE HEPMC EVENT LEVEL TRUTH PARTICLES

  ************************************************/
  
  PHHepMCGenEvent *hepmcevent = findNode::getClass<PHHepMCGenEvent>(topnode,"PHHepMCGenEvent");

  if(!hepmcevent){
    cout<<"hepmc event node is missing, BAILING"<<endl;
    return 0;
  }

  HepMC::GenEvent *truthevent = hepmcevent->getEvent();
  if(!truthevent){
    cout<< PHWHERE <<"no evt pointer under phhepmvgenevent found "<<endl;
    return 0;
  }
  numparticlesinevent=0;
  process_id = truthevent->signal_process_id();
  for(HepMC::GenEvent::particle_const_iterator iter = truthevent->particles_begin(); iter!=truthevent->particles_end();++iter){

    truthenergy = (*iter)->momentum().e();   
    truthpid = (*iter)->pdg_id();
    trutheta = (*iter)->momentum().pseudoRapidity();
    truthphi = (*iter)->momentum().phi();
    truthpx = (*iter)->momentum().px();
    truthpy = (*iter)->momentum().py();
    truthpz = (*iter)->momentum().pz();
    truthpt = sqrt(truthpx*truthpx+truthpy*truthpy);
   

    truthtree->Fill();
    numparticlesinevent++;
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
    
    if(clus_pt<0.5)
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

    cluster_tree->Fill();

    //only interested in high pt photons to be isolated
    if(clus_pt<mincluspt)
      continue;
    if(fabs(clus_eta)>(1.0-isoconeradius) && use_isocone)
      continue;

    float energysum = ConeSum(cluster,clusters,trackmap,isoconeradius);
    bool conecut = energysum > 0.1*clus_energy;
    if(conecut && use_isocone)
      continue;
   

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
	if(clustruthpt<mincluspt)
	  continue;
	
	TLorentzVector vec;
	vec.SetPxPyPzE(clustruthpx,clustruthpy,clustruthpz,clustruthenergy);
	clustruthphi = vec.Phi();
	clustrutheta = vec.Eta();
	if(fabs(clustruthphi-clus_phi)>0.03 || fabs(clustrutheta-clus_eta)>0.03)
	  continue;
	
	//once the photon is found and the values are set
	//just break out of the loop 
	break;


      }
    

    }

 
    isolated_clusters->Fill();
    
    GetRecoHadronsAndJets(cluster, trackmap, reco_jets,recoeval,trackeval,truthinfo);

  }



  /***********************************************

  GET THE SVTX TRACKS

  ************************************************/
  


  for(SvtxTrackMap::Iter iter = trackmap->begin(); iter!=trackmap->end(); ++iter){

    SvtxTrack *track = iter->second;

    //get reco info
    tr_px = track->get_px();
    tr_py = track->get_py();
    tr_pz = track->get_pz();
    tr_p = sqrt(tr_px*tr_px+tr_py*tr_py+tr_pz*tr_pz);
    
    tr_pt = sqrt(tr_px*tr_px+tr_py*tr_py);
    
    if(tr_pt<0.5)
      continue;
  
    tr_phi = track->get_phi();
    tr_eta = track->get_eta();

    if(fabs(tr_eta)>1)
      continue;

    charge = track->get_charge();
    chisq = track->get_chisq();
    ndf = track->get_ndf();
    dca = track->get_dca();
    tr_x = track->get_x();
    tr_y = track->get_y();
    tr_z = track->get_z();
    
   
    //get truth info
    PHG4Particle *truthtrack = trackeval->max_truth_particle_by_nclusters(track);
    truth_is_primary = truthinfo->is_primary(truthtrack);
    
    truthtrackpx = truthtrack->get_px();
    truthtrackpy = truthtrack->get_py();
    truthtrackpz = truthtrack->get_pz();
    truthtrackp = sqrt(truthtrackpx*truthtrackpx+truthtrackpy*truthtrackpy+truthtrackpz*truthtrackpz);
    
    truthtracke = truthtrack->get_e();
    TLorentzVector *Truthtrack = new TLorentzVector();
    Truthtrack->SetPxPyPzE(truthtrackpx,truthtrackpy,truthtrackpz,truthtracke);
    truthtrackpt = Truthtrack->Pt();
    truthtrackphi = Truthtrack->Phi();
    truthtracketa = Truthtrack->Eta();
    truthtrackpid = truthtrack->get_pid();


    tracktree->Fill();

  }







  /***************************************

   TRUTH JETS

   ***************************************/


  for(JetMap::Iter iter = truth_jets->begin(); iter!=truth_jets->end(); ++iter){
    Jet *jet = iter->second;
    
    truthjetpt = jet->get_pt();
    if(truthjetpt<10.)
      continue;
    
    truthjeteta = jet->get_eta();
    if(fabs(truthjeteta)>2.)
      continue;

    truthjetpx = jet->get_px();
    truthjetpy = jet->get_py();
    truthjetpz = jet->get_pz();
    truthjetphi = jet->get_phi();
    truthjetmass = jet->get_mass();
    truthjetp = jet->get_p();
    truthjetenergy = jet->get_e();
    
    truthjettree->Fill();
    


  }



/***************************************

   RECONSTRUCTED JETS

   ***************************************/



  for(JetMap::Iter iter = reco_jets->begin(); iter!=reco_jets->end(); ++iter){
    Jet *jet = iter->second;
    Jet *truthjet = recoeval->max_truth_jet_by_energy(jet);
    recojetpt = jet->get_pt();
    if(recojetpt<4.)
      continue;
    
    recojeteta = jet->get_eta();
    if(fabs(recojeteta)>2.)
      continue;
    recojetid = jet->get_id();
    recojetpx = jet->get_px();
    recojetpy = jet->get_py();
    recojetpz = jet->get_pz();
    recojetphi = jet->get_phi();
    recojetmass = jet->get_mass();
    recojetp = jet->get_p();
    recojetenergy = jet->get_e();

    if(truthjet){
      truthjetid = truthjet->get_id();
      truthjetp = truthjet->get_p();
      truthjetphi = truthjet->get_phi();
      truthjeteta = truthjet->get_eta();
      truthjetpt = truthjet->get_pt();
      truthjetenergy = truthjet->get_e();
      truthjetpx = truthjet->get_px();
      truthjetpy = truthjet->get_py();
      truthjetpz = truthjet->get_pz();
    }
    else{
      truthjetid = 0;
      truthjetp = 0;
      truthjetphi = 0;
      truthjeteta = 0;
      truthjetpt = 0;
      truthjetenergy = 0;
      truthjetpx = 0;
      truthjetpy = 0;
      truthjetpz = 0;
      
    }
    recojettree->Fill();

  }

  


  nevents++;
  tree->Fill();
  return 0;

}


int PhotonJet::End(PHCompositeNode *topnode)
{
  std::cout<<" DONE PROCESSING "<<endl;

  file->Write();
  file->Close();
  return 0;
}

void PhotonJet::GetRecoHadronsAndJets(RawCluster *trig,
				      SvtxTrackMap *tracks,
				      JetMap *jets,
				      JetRecoEval *recoeval,
				      SvtxTrackEval *trackeval,
				      PHG4TruthInfoContainer *alltruth)
{

  float trig_phi = trig->get_phi();
  float trig_eta = trig->get_eta();

  PHG4TruthInfoContainer::Range range = alltruth->GetPrimaryParticleRange();

  for( PHG4TruthInfoContainer::ConstIterator iter = range.first; iter!=range.second; ++iter){
    
    PHG4Particle *truth = iter->second;
    
    clustruthpid = truth->get_pid();
    if(clustruthpid==22){
      clustruthpx = truth->get_px();
      clustruthpy = truth->get_py();
      clustruthpz = truth->get_pz();
      clustruthenergy = truth->get_e();
      clustruthpt = sqrt(clustruthpx*clustruthpx+clustruthpy*clustruthpy);
      if(clustruthpt<mincluspt)
	continue;
      
      TLorentzVector vec;
      vec.SetPxPyPzE(clustruthpx,clustruthpy,clustruthpz,clustruthenergy);
      clustruthphi = vec.Phi();
      clustrutheta = vec.Eta();
      if(fabs(clustruthphi-trig_phi)>0.03 || fabs(clustrutheta-trig_eta)>0.03)
	continue;
      
      //once the values are set, we've found the truth photon so just break out of this loop
      break; 
    }
  }


  for(SvtxTrackMap::Iter iter = tracks->begin(); iter!=tracks->end(); ++iter){

    SvtxTrack *track = iter->second;

    _tr_px = track->get_px();
    _tr_py = track->get_py();
    _tr_pz = track->get_pz();
    _tr_pt = sqrt(_tr_px*_tr_px+_tr_py*_tr_py);
    if(_tr_pt<0.5)
      continue;

    _tr_p = sqrt(_tr_px*_tr_px+_tr_py*_tr_py+_tr_pz*_tr_pz);
    _tr_phi = track->get_phi();
    _tr_eta = track->get_eta();
    _charge = track->get_charge();
    _chisq = track->get_chisq();
    _ndf = track->get_ndf();
    _dca = track->get_dca();
    _tr_x = track->get_x();
    _tr_y = track->get_y();
    _tr_z = track->get_z();

    haddphi = trig_phi-_tr_phi;
    if(haddphi<pi2)
      haddphi+=2.*pi;
    if(haddphi>threepi2)
      haddphi-=2.*pi;

    PHG4Particle *truthtrack = trackeval->max_truth_particle_by_nclusters(track);
    _truth_is_primary = alltruth->is_primary(truthtrack);
    
    _truthtrackpx = truthtrack->get_px();
    _truthtrackpy = truthtrack->get_py();
    _truthtrackpz = truthtrack->get_pz();
    _truthtrackp = sqrt(truthtrackpx*truthtrackpx+truthtrackpy*truthtrackpy+truthtrackpz*truthtrackpz);
    
    _truthtracke = truthtrack->get_e();
    TLorentzVector *Truthtrack = new TLorentzVector();
    Truthtrack->SetPxPyPzE(truthtrackpx,truthtrackpy,truthtrackpz,truthtracke);
    _truthtrackpt = Truthtrack->Pt();
    _truthtrackphi = Truthtrack->Phi();
    _truthtracketa = Truthtrack->Eta();
    _truthtrackpid = truthtrack->get_pid();



    haddeta = trig_eta-_tr_eta;

    hadpout = _tr_p*TMath::Sin(haddphi);

    isophot_had_tree->Fill();
    

  }
  for(JetMap::Iter iter = jets->begin(); iter!=jets->end(); ++iter){
    Jet* jet = iter->second;
    Jet *truthjet = recoeval->max_truth_jet_by_energy(jet);

    _recojetpt = jet->get_pt();
    if(_recojetpt<4.0) 
      continue;
      
    _recojeteta = jet->get_eta();
    if(fabs(_recojeteta)>1.)
      continue;

    _recojetpx = jet->get_px();
    _recojetpy = jet->get_py();
    _recojetpz = jet->get_pz();
    _recojetphi = jet->get_phi();
    _recojetmass = jet->get_mass();
    _recojetp = jet->get_p();
    _recojetenergy = jet->get_e();
    _recojetid = jet->get_id();

    if(truthjet){
      _truthjetid = truthjet->get_id();
      _truthjetp = truthjet->get_p();
      _truthjetphi = truthjet->get_phi();
      _truthjeteta = truthjet->get_eta();
      _truthjetpt = truthjet->get_pt();
      _truthjetenergy = truthjet->get_e();
      _truthjetpx = truthjet->get_px();
      _truthjetpy = truthjet->get_py();
      _truthjetpz = truthjet->get_pz();
    }
    else{
      _truthjetid = 0;
      _truthjetp = 0;
      _truthjetphi = 0;
      _truthjeteta = 0;
      _truthjetpt = 0;
      _truthjetenergy = 0;
      _truthjetpx = 0;
      _truthjetpy = 0;
      _truthjetpz = 0;

    }
    
    jetdphi = trig_phi-_recojetphi;
    if(jetdphi<pi2)
      jetdphi+=2.*pi;
    if(jetdphi>threepi2)
      jetdphi-=2.*pi;
    
    jetdeta = trig_eta-_recojeteta;
    jetpout = _recojetpt*TMath::Sin(jetdphi);

    isophot_jet_tree->Fill();
  }


}


float PhotonJet::ConeSum(RawCluster *cluster, 
			 RawClusterContainer *cluster_container,
			 SvtxTrackMap *trackmap,
			 float coneradius)
{

  float energyptsum=0;
  
  RawClusterContainer::ConstRange begin_end = cluster_container->getClusters();
  RawClusterContainer::ConstIterator clusiter;
  
  for(clusiter = begin_end.first; clusiter!=begin_end.second; ++clusiter){
    RawCluster *conecluster = clusiter->second;
    
    //check to make sure that the candidate isolated photon isn't being counted in the energy sum
    if(conecluster->get_energy() == cluster->get_energy())
      if(conecluster->get_phi() == cluster->get_phi())
	if(conecluster->get_eta() == cluster->get_eta())
	  continue;

    float cone_pt = conecluster->get_energy()/TMath::CosH(conecluster->get_eta());
    if(cone_pt<0.2)
      continue;
    
    float cone_e = conecluster->get_energy();
    float cone_eta = conecluster->get_eta();
    float cone_phi = conecluster->get_phi();
    


    float dphi = cluster->get_phi()-cone_phi;
    if(dphi<-1*pi)
      dphi+=2.*pi;
    if(dphi>pi)
      dphi-=2.*pi;
    
    float deta = cluster->get_eta()-cone_eta;

   
    float radius = sqrt(dphi*dphi+deta*deta);
   
    if(radius<coneradius){
      energyptsum+=cone_e;
    
    }
  }
  

  
 
  for(SvtxTrackMap::Iter iter = trackmap->begin(); iter!=trackmap->end(); ++iter){
    SvtxTrack *track = iter->second;

    float trackpx = track->get_px();
    float trackpy = track->get_py();
    float trackpt = sqrt(trackpx*trackpx+trackpy*trackpy);
    if(trackpt<0.2)
      continue;
    float trackphi = track->get_phi();
    float tracketa = track->get_eta();
    float dphi = cluster->get_phi()-trackphi;
    float deta = cluster->get_eta()-tracketa;
    float radius = sqrt(dphi*dphi+deta*deta);
  
    if(radius<coneradius){
      energyptsum+=trackpt;
    }
  }


  return energyptsum;

}










void PhotonJet::Set_Tree_Branches()
{

  
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


  isolated_clusters = new TTree("isolated_clusters","a tree with isolated EMCal clusters");
  isolated_clusters->Branch("clus_energy",&clus_energy,"clus_energy/F");
  isolated_clusters->Branch("clus_eta",&clus_eta,"clus_eta/F");
  isolated_clusters->Branch("clus_phi",&clus_phi,"clus_phi/F");
  isolated_clusters->Branch("clus_pt",&clus_pt,"clus_pt/F");
  isolated_clusters->Branch("clus_theta",&clus_theta,"clus_theta/F");
  isolated_clusters->Branch("clus_x",&clus_x,"clus_x/F");
  isolated_clusters->Branch("clus_y",&clus_y,"clus_y/F");
  isolated_clusters->Branch("clus_z",&clus_z,"clus_z/F");
  isolated_clusters->Branch("clus_t",&clus_t,"clus_t/F");
  isolated_clusters->Branch("clus_px",&clus_px,"clus_px/F");
  isolated_clusters->Branch("clus_py",&clus_py,"clus_py/F");
  isolated_clusters->Branch("clus_pz",&clus_pz,"clus_pz/F");
  isolated_clusters->Branch("nevents",&nevents,"nenvents/I");
  isolated_clusters->Branch("clustruthenergy",&clustruthenergy,"clustruthenergy/F");
  isolated_clusters->Branch("clustruthpt",&clustruthpt,"clustruthpt/F");
  isolated_clusters->Branch("clustruthphi",&clustruthphi,"clustruthphi/F");
  isolated_clusters->Branch("clustrutheta",&clustrutheta,"clustrutheta/F");
  isolated_clusters->Branch("clustruthpx",&clustruthpx,"clustruthpx/F");
  isolated_clusters->Branch("clustruthpy",&clustruthpy,"clustruthpy/F");
  isolated_clusters->Branch("clustruthpz",&clustruthpz,"clustruthpz/F");

  tracktree = new TTree("tracktree","a tree with svtx tracks");
  tracktree->Branch("tr_px",&tr_px,"tr_px/F");
  tracktree->Branch("tr_py",&tr_py,"tr_py/F");
  tracktree->Branch("tr_pz",&tr_pz,"tr_pz/F");
  tracktree->Branch("tr_pt",&tr_pt,"tr_pt/F");
  tracktree->Branch("tr_phi",&tr_phi,"tr_phi/F");
  tracktree->Branch("tr_eta",&tr_eta,"tr_eta/F");
  tracktree->Branch("charge",&charge,"charge/I");
  tracktree->Branch("chisq",&chisq,"chisq/F");
  tracktree->Branch("ndf",&ndf,"ndf/I");
  tracktree->Branch("dca",&dca,"dca/F");
  tracktree->Branch("tr_x",&tr_x,"tr_x/F");
  tracktree->Branch("tr_y",&tr_y,"tr_y/F");
  tracktree->Branch("tr_z",&tr_z,"tr_z/F");
  tracktree->Branch("nevents",&nevents,"nevents/i");

  tracktree->Branch("truthtrackpx",&truthtrackpx,"truthtrackpx/F");
  tracktree->Branch("truthtrackpy",&truthtrackpy,"truthtrackpy/F");
  tracktree->Branch("truthtrackpz",&truthtrackpz,"truthtrackpz/F");
  tracktree->Branch("truthtrackp",&truthtrackp,"truthtrackp/F");
  tracktree->Branch("truthtracke",&truthtracke,"truthtracke/F");
  tracktree->Branch("truthtrackpt",&truthtrackpt,"truthtrackpt/F");
  tracktree->Branch("truthtrackphi",&truthtrackphi,"truthtrackphi/F");
  tracktree->Branch("truthtracketa",&truthtracketa,"truthtracketa/F");
  tracktree->Branch("truthtrackpid",&truthtrackpid,"truthtrackpid/I");
  tracktree->Branch("truth_is_primary",&truth_is_primary,"truth_is_primary/B");




  truthjettree = new TTree("truthjettree","a tree with truth jets");
  truthjettree->Branch("truthjetpt",&truthjetpt,"truthjetpt/F");
  truthjettree->Branch("truthjetpx",&truthjetpx,"truthjetpx/F");
  truthjettree->Branch("truthjetpy",&truthjetpy,"truthjetpy/F");
  truthjettree->Branch("truthjetpz",&truthjetpz,"truthjetpz/F");
  truthjettree->Branch("truthjetphi",&truthjetphi,"truthjetphi/F");
  truthjettree->Branch("truthjeteta",&truthjeteta,"truthjeteta/F");
  truthjettree->Branch("truthjetmass",&truthjetmass,"truthjetmass/F");
  truthjettree->Branch("truthjetp",&truthjetp,"truthjetp/F");
  truthjettree->Branch("truthjetenergy",&truthjetenergy,"truthjetenergy/F");
  truthjettree->Branch("nevents",&nevents,"nevents/I");


  recojettree = new TTree("recojettree","a tree with reco jets");
  recojettree->Branch("recojetpt",&recojetpt,"recojetpt/F");
  recojettree->Branch("recojetpx",&recojetpx,"recojetpx/F");
  recojettree->Branch("recojetpy",&recojetpy,"recojetpy/F");
  recojettree->Branch("recojetpz",&recojetpz,"recojetpz/F");
  recojettree->Branch("recojetphi",&recojetphi,"recojetphi/F");
  recojettree->Branch("recojeteta",&recojeteta,"recojeteta/F");
  recojettree->Branch("recojetmass",&recojetmass,"recojetmass/F");
  recojettree->Branch("recojetp",&recojetp,"recojetp/F");
  recojettree->Branch("recojetenergy",&recojetenergy,"recojetenergy/F");
  recojettree->Branch("nevents",&nevents,"nevents/I");
  recojettree->Branch("recojetid",&recojetid,"recojetid/F");
  recojettree->Branch("truthjetid",&truthjetid,"truthjetid/F");
  recojettree->Branch("truthjetp",&truthjetp,"truthjetp/F");
  recojettree->Branch("truthjetphi",&truthjetphi,"truthjetphi/F");
  recojettree->Branch("truthjeteta",&truthjeteta,"truthjeteta/F");
  recojettree->Branch("truthjetpt",&truthjetpt,"truthjetpt/F");
  recojettree->Branch("truthjetenergy",&truthjetenergy,"truthjetenergy/F");
  recojettree->Branch("truthjetpx",&truthjetpx,"truthjetpx/F");
  recojettree->Branch("truthjetpy",&truthjetpy,"truthjetpy/F");
  recojettree->Branch("truthjetpz",&truthjetpz,"truthjetpz/F");



  isophot_jet_tree = new TTree("isophoton-jets","a tree with correlated isolated photons and jets"); 
  isophot_jet_tree->Branch("clus_energy",&clus_energy,"clus_energy/F");
  isophot_jet_tree->Branch("clus_eta",&clus_eta,"clus_eta/F");
  isophot_jet_tree->Branch("clus_phi",&clus_phi,"clus_phi/F");
  isophot_jet_tree->Branch("clus_pt",&clus_pt,"clus_pt/F");
  isophot_jet_tree->Branch("clus_theta",&clus_theta,"clus_theta/F");
  isophot_jet_tree->Branch("clus_x",&clus_x,"clus_x/F");
  isophot_jet_tree->Branch("clus_y",&clus_y,"clus_y/F");
  isophot_jet_tree->Branch("clus_z",&clus_z,"clus_z/F");
  isophot_jet_tree->Branch("clus_t",&clus_t,"clus_t/F");
  isophot_jet_tree->Branch("clus_px",&clus_px,"clus_px/F");
  isophot_jet_tree->Branch("clus_py",&clus_py,"clus_py/F");
  isophot_jet_tree->Branch("clus_pz",&clus_pz,"clus_pz/F");
  isophot_jet_tree->Branch("clustruthenergy",&clustruthenergy,"clustruthenergy/F");
  isophot_jet_tree->Branch("clustruthpt",&clustruthpt,"clustruthpt/F");
  isophot_jet_tree->Branch("clustruthphi",&clustruthphi,"clustruthphi/F");
  isophot_jet_tree->Branch("clustrutheta",&clustrutheta,"clustrutheta/F");
  isophot_jet_tree->Branch("clustruthpx",&clustruthpx,"clustruthpx/F");
  isophot_jet_tree->Branch("clustruthpy",&clustruthpy,"clustruthpy/F");
  isophot_jet_tree->Branch("clustruthpz",&clustruthpz,"clustruthpz/F");
  isophot_jet_tree->Branch("_recojetpt",&_recojetpt,"_recojetpt/F");
  isophot_jet_tree->Branch("_recojetpx",&_recojetpx,"_recojetpx/F");
  isophot_jet_tree->Branch("_recojetpy",&_recojetpy,"_recojetpy/F");
  isophot_jet_tree->Branch("_recojetpz",&_recojetpz,"_recojetpz/F");
  isophot_jet_tree->Branch("_recojetphi",&_recojetphi,"_recojetphi/F");
  isophot_jet_tree->Branch("_recojeteta",&_recojeteta,"_recojeteta/F");
  isophot_jet_tree->Branch("_recojetmass",&_recojetmass,"_recojetmass/F");
  isophot_jet_tree->Branch("_recojetp",&_recojetp,"_recojetp/F");
  isophot_jet_tree->Branch("_recojetenergy",&_recojetenergy,"_recojetenergy/F");
  isophot_jet_tree->Branch("jetdphi",&jetdphi,"jetdphi/F");
  isophot_jet_tree->Branch("jetdeta",&jetdeta,"jetdeta/F");
  isophot_jet_tree->Branch("jetpout",&jetpout,"jetpout/F");
  isophot_jet_tree->Branch("nevents",&nevents,"nevents/I");
  isophot_jet_tree->Branch("_recojetid",&_recojetid,"_recojetid/F");
  isophot_jet_tree->Branch("_truthjetid",&_truthjetid,"_truthjetid/F");
  isophot_jet_tree->Branch("_truthjetp",&_truthjetp,"_truthjetp/F");
  isophot_jet_tree->Branch("_truthjetphi",&_truthjetphi,"_truthjetphi/F");
  isophot_jet_tree->Branch("_truthjeteta",&_truthjeteta,"_truthjeteta/F");
  isophot_jet_tree->Branch("_truthjetpt",&_truthjetpt,"_truthjetpt/F");
  isophot_jet_tree->Branch("_truthjetenergy",&_truthjetenergy,"_truthjetenergy/F");
  isophot_jet_tree->Branch("_truthjetpx",&_truthjetpx,"_truthjetpx/F");
  isophot_jet_tree->Branch("_truthjetpy",&_truthjetpy,"_truthjetpy/F");
  isophot_jet_tree->Branch("_truthjetpz",&_truthjetpz,"_truthjetpz/F");
  isophot_jet_tree->Branch("process_id",&process_id,"process_id/I");


  isophot_had_tree = new TTree("isophoton-hads","a tree with correlated isolated photons and hadrons"); 
  isophot_had_tree->Branch("clus_energy",&clus_energy,"clus_energy/F");
  isophot_had_tree->Branch("clus_eta",&clus_eta,"clus_eta/F");
  isophot_had_tree->Branch("clus_phi",&clus_phi,"clus_phi/F");
  isophot_had_tree->Branch("clus_pt",&clus_pt,"clus_pt/F");
  isophot_had_tree->Branch("clus_theta",&clus_theta,"clus_theta/F");
  isophot_had_tree->Branch("clus_x",&clus_x,"clus_x/F");
  isophot_had_tree->Branch("clus_y",&clus_y,"clus_y/F");
  isophot_had_tree->Branch("clus_z",&clus_z,"clus_z/F");
  isophot_had_tree->Branch("clus_t",&clus_t,"clus_t/F");
  isophot_had_tree->Branch("clus_px",&clus_px,"clus_px/F");
  isophot_had_tree->Branch("clus_py",&clus_py,"clus_py/F");
  isophot_had_tree->Branch("clus_pz",&clus_pz,"clus_pz/F");
  isophot_had_tree->Branch("clustruthenergy",&clustruthenergy,"clustruthenergy/F");
  isophot_had_tree->Branch("clustruthpt",&clustruthpt,"clustruthpt/F");
  isophot_had_tree->Branch("clustruthphi",&clustruthphi,"clustruthphi/F");
  isophot_had_tree->Branch("clustrutheta",&clustrutheta,"clustrutheta/F");
  isophot_had_tree->Branch("clustruthpx",&clustruthpx,"clustruthpx/F");
  isophot_had_tree->Branch("clustruthpy",&clustruthpy,"clustruthpy/F");
  isophot_had_tree->Branch("clustruthpz",&clustruthpz,"clustruthpz/F");


  isophot_had_tree->Branch("_tr_px",&_tr_px,"_tr_px/F");
  isophot_had_tree->Branch("_tr_py",&_tr_py,"_tr_py/F");
  isophot_had_tree->Branch("_tr_pz",&_tr_pz,"_tr_pz/F");
  isophot_had_tree->Branch("_tr_pt",&_tr_pt,"_tr_pt/F");
  isophot_had_tree->Branch("_tr_phi",&_tr_phi,"_tr_phi/F");
  isophot_had_tree->Branch("_tr_eta",&_tr_eta,"_tr_eta/F");
  isophot_had_tree->Branch("_charge",&_charge,"_charge/I");
  isophot_had_tree->Branch("_chisq",&_chisq,"_chisq/F");
  isophot_had_tree->Branch("_ndf",&_ndf,"_ndf/I");
  isophot_had_tree->Branch("_dca",&_dca,"_dca/F");
  isophot_had_tree->Branch("_tr_x",&_tr_x,"_tr_x/F");
  isophot_had_tree->Branch("_tr_y",&_tr_y,"_tr_y/F");
  isophot_had_tree->Branch("_tr_z",&_tr_z,"_tr_z/F");
  isophot_had_tree->Branch("haddphi",&haddphi,"haddphi/F");
  isophot_had_tree->Branch("hadpout",&hadpout,"hadpout/F");
  isophot_had_tree->Branch("haddeta",&haddeta,"haddeta/F");
  isophot_had_tree->Branch("nevents",&nevents,"nevents/I");
  isophot_had_tree->Branch("_truthtrackpx",&_truthtrackpx,"_truthtrackpx/F");
  isophot_had_tree->Branch("_truthtrackpy",&_truthtrackpy,"_truthtrackpy/F");
  isophot_had_tree->Branch("_truthtrackpz",&_truthtrackpz,"_truthtrackpz/F");
  isophot_had_tree->Branch("_truthtrackp",&_truthtrackp,"_truthtrackp/F");
  isophot_had_tree->Branch("_truthtracke",&_truthtracke,"_truthtracke/F");
  isophot_had_tree->Branch("_truthtrackpt",&_truthtrackpt,"_truthtrackpt/F");
  isophot_had_tree->Branch("_truthtrackphi",&_truthtrackphi,"_truthtrackphi/F");
  isophot_had_tree->Branch("_truthtracketa",&_truthtracketa,"_truthtracketa/F");
  isophot_had_tree->Branch("_truthtrackpid",&_truthtrackpid,"_truthtrackpid/I");
  isophot_had_tree->Branch("_truth_is_primary",&_truth_is_primary,"_truth_is_primary/B");
  isophot_had_tree->Branch("process_id",&process_id,"process_id/I");






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
  truth_g4particles->Branch("process_id",&process_id,"process_id/I");

  truthtree = new TTree("truthtree","a tree with all truth pythia particles");
  truthtree->Branch("truthpx",&truthpx,"truthpx/F");
  truthtree->Branch("truthpy",&truthpy,"truthpy/F");
  truthtree->Branch("truthpz",&truthpz,"truthpz/F");
  truthtree->Branch("truthp",&truthp,"truthp/F");
  truthtree->Branch("truthenergy",&truthenergy,"truthenergy/F");
  truthtree->Branch("truthphi",&truthphi,"truthphi/F");
  truthtree->Branch("trutheta",&trutheta,"trutheta/F");
  truthtree->Branch("truthpt",&truthpt,"truthpt/F");
  truthtree->Branch("truthpid",&truthpid,"truthpid/I");
  truthtree->Branch("nevents",&nevents,"nevents/I");
  truthtree->Branch("numparticlesinevent",&numparticlesinevent,"numparticlesinevent/I");
  truthtree->Branch("process_id",&process_id,"process_id/I");

}



