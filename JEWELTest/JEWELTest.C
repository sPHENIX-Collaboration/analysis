#include "JEWELTest.h"

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

using namespace std;

JEWELTest::JEWELTest(const std::string &name) : SubsysReco("JEWELTEST")
{
  outfilename = name;

  //add other initializers here
}

int JEWELTest::Init(PHCompositeNode *topnode)
{

  file = new TFile(outfilename.c_str(),"RECREATE");
  nevents=0;

  histo = new TH1F("histo","histo",100,-3,3);

  tree = new TTree("tree","a tree");
  tree->Branch("nevents",&nevents,"nevents/I");

  Set_Tree_Branches();

  return 0;
}

int JEWELTest::process_event(PHCompositeNode *topnode)
{
 
  cout<<"at event number "<<nevents<<endl;

  //get the nodes from the NodeTree
  JetMap* truth_jets = findNode::getClass<JetMap>(topnode,"AntiKt_Truth_r04");
  JetMap *reco_jets = findNode::getClass<JetMap>(topnode,"AntiKt_Tower_r04");
  PHG4TruthInfoContainer* truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topnode,"G4TruthInfo");
  RawClusterContainer *clusters = findNode::getClass<RawClusterContainer>(topnode,"CLUSTER_CEMC");
  SvtxTrackMap *trackmap = findNode::getClass<SvtxTrackMap>(topnode,"SvtxTrackMap");
  JetEvalStack* _jetevalstack = new JetEvalStack(topnode,"AntiKt_Tower_r04","AntiKt_Truth_r04");
  PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();

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

  //! ******** Truth particles *********  
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
    if(truthpt<0.5)
      continue;
    
    truthphi = vec.Phi();
    trutheta = vec.Eta();
    truthpid = truth->get_pid();
    
    truth_g4particles->Fill();

  }

  //! ******** EMCal clusters *********  
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
    
  }

  //! ******** tracks *********  
  SvtxEvalStack *svtxevalstack = new SvtxEvalStack(topnode);
  svtxevalstack->next_event(topnode);

  SvtxTrackEval *trackeval = svtxevalstack->get_track_eval();

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

  //! ******** GEN Jets *********
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


  //! ******** RECO Jets *********
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


int JEWELTest::End(PHCompositeNode *topnode)
{
  std::cout<<" DONE PROCESSING "<<endl;

  file->Write();
  file->Close();
  return 0;
}


void JEWELTest::Set_Tree_Branches()
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


  truth_g4particles = new TTree("truthtree","a tree with all truth particles");
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



