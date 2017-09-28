#include "JetPerformance.h"

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
#include <phhepmc/PHHepMCGenEvent.h>
#include <HepMC/GenEvent.h>
#include <g4cemc/RawClusterContainer.h>
#include <g4cemc/RawCluster.h>

#include <g4detectors/PHG4ScintillatorSlatContainer.h>
#include <g4eval/JetEvalStack.h>
#include <g4hough/SvtxTrackMap.h>
#include <g4hough/SvtxTrack.h>

#include <g4eval/SvtxEvalStack.h>


using namespace std;

float deltaR( float eta1, float eta2, float phi1, float phi2 ) {
  float deta = eta1 - eta2;
  float dphi = phi1 - phi2;
  if (dphi > pi) dphi -= 2*pi;
  if (dphi < -pi) dphi += 2*pi;

  return sqrt( pow( deta, 2 ) + pow( dphi, 2 ) );

}


JetPerformance::JetPerformance(const std::string &name) : SubsysReco("JETPERFORMANCE")
{
  outfilename = name;

  //add other initializers here
}

int JetPerformance::Init(PHCompositeNode *topnode)
{

  file = new TFile(outfilename.c_str(),"RECREATE");
  nevents=0;

  tree = new TTree("tree","a tree");
  tree->Branch("nevents",&nevents,"nevents/I");

  Set_Tree_Branches();

  hRecoJetpT = new TH1F("hRecoJetpT","Reco jet pT spectra",nbins_pt, ptbins);
  hRecoJeteta = new TH1F("hRecoJeteta","Reco jet eta spectra",nbins_eta, etabins);
  hRecoJetphi = new TH1F("hRecoJetphi","Reco jet phi spectra",30, -1*pi, pi);

  hReco_NoHcalIn_JetpT = new TH1F("hReco_NoHcalIn_JetpT","Reco_NoHcalIn_ jet pT spectra",nbins_pt, ptbins);
  hReco_NoHcalIn_Jeteta = new TH1F("hReco_NoHcalIn_Jeteta","Reco_NoHcalIn_ jet eta spectra",nbins_eta, etabins);
  hReco_NoHcalIn_Jetphi = new TH1F("hReco_NoHcalIn_Jetphi","Reco_NoHcalIn_ jet phi spectra",30, -1*pi, pi);
  hReco_NoHcalIn_GenJetdelR = new TH1F("hReco_NoHcalIn_GenJetdelR", "Delta R between reco_noHcalIn_ jet and gen jet",25, 0, 0.5);

  hGenJetpT = new TH1F("hGenJetpT","Gen jet pT spectra",nbins_pt, ptbins);
  hGenJeteta = new TH1F("hGenJeteta","Gen jet eta spectra",nbins_eta, etabins);
  hGenJetphi = new TH1F("hGenJetphi","Gen jet phi spectra",30, -1*pi, pi);

  hRecoGenJetdelR = new TH1F("hRecoGenJetdelR", "Delta R between reco jet and gen jet",25, 0, 0.5);

  hJER_allpT_allEta = new TH1F("hJER_allpT_allEta","", 50, 0, 2);
  hJER_noHcalIn_allpT_allEta = new TH1F("hJER_noHcalIn_allpT_allEta","", 50, 0, 2);

  for(int i = 0; i<nbins_pt; ++i){
    for(int j = 0; j<nbins_eta; ++j){
      hJER[i][j] = new TH1F(Form("hJER_ptbin%d_etabin%d",i, j),"",50, 0, 2);
      hJER_noHcalIn[i][j] = new TH1F(Form("hJER_noHcalIn_ptbin%d_etabin%d",i, j),"",50, 0, 2);
    }
  }
  
 
  return 0;
}



int JetPerformance::process_event(PHCompositeNode *topnode)
{
 
  cout<<"at event number "<<nevents<<endl;

  //get the nodes from the NodeTree
  JetMap* truth_jets = findNode::getClass<JetMap>(topnode,"AntiKt_Truth_r04");
  JetMap *reco_jets = findNode::getClass<JetMap>(topnode,"AntiKt_Tower_r04");
  JetMap *reco_noHcalIn_jets = findNode::getClass<JetMap>(topnode,"AntiKt_Tower_noHcalIn_r04");
  // JetMap *reco_jets = findNode::getClass<JetMap>(topnode,"AntiKt_Tower_r04_Sub1");
  // JetMap *reco_jets_noHcalIn = findNode::getClass<JetMap>(topnode,"AntiKt_Tower_noHcalIn_r04_Sub1");
  JetEvalStack* _jetevalstack = new JetEvalStack(topnode,"AntiKt_Tower_r04","AntiKt_Truth_r04");
  // PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();

  PHHepMCGenEvent *genevt = findNode::getClass<PHHepMCGenEvent>(topnode, "PHHepMCGenEvent");
  HepMC::GenEvent* theEvent = genevt->getEvent();
  //theEvent->print();
  
  if(!truth_jets){
    cout<<"no truth jets"<<endl;
    return 0;
  }
  if(!reco_jets){
    cout<<"no reco jets"<<endl;
    return 0;
  }
  if(!reco_noHcalIn_jets){
    cout<<"no reco jets"<<endl;
    return 0;
  }
  // if(!_jetevalstack){
  //   cout<<"no good truth jets"<<endl;
  //   return 0;
  // }


  JetRecoEval* recoeval = _jetevalstack->get_reco_eval();

  /***************************************

   TRUTH JETS

  ***************************************/


  for(JetMap::Iter iter = truth_jets->begin(); iter!=truth_jets->end(); ++iter){
    Jet *jet = iter->second;
    
    truthjetpt = jet->get_pt();
    if(truthjetpt<10.)
      continue;
    
    truthjeteta = jet->get_eta();
    if(fabs(truthjeteta)>2.0)
      continue;

    truthjetpx = jet->get_px();
    truthjetpy = jet->get_py();
    truthjetpz = jet->get_pz();
    truthjetphi = jet->get_phi();
    truthjetmass = jet->get_mass();
    truthjetp = jet->get_p();
    truthjetenergy = jet->get_e();
    
    truthjettree->Fill();

    hGenJetpT->Fill(truthjetpt);
    hGenJeteta->Fill(truthjeteta);
    hGenJetphi->Fill(truthjetphi);    

  }



  /***************************************

   RECONSTRUCTED JETS

  ***************************************/



  for(JetMap::Iter iter = reco_jets->begin(); iter!=reco_jets->end(); ++iter){
    Jet *jet = iter->second;
    recojetpt = jet->get_pt();
    if(recojetpt<4.)
      continue;    
    
    recojeteta = jet->get_eta();
    if(fabs(recojeteta)>2.0)
      continue;
    recojetid = jet->get_id();
    recojetpx = jet->get_px();
    recojetpy = jet->get_py();
    recojetpz = jet->get_pz();
    recojetphi = jet->get_phi();
    recojetmass = jet->get_mass();
    recojetp = jet->get_p();
    recojetenergy = jet->get_e();

    hRecoJetpT->Fill(recojetpt);
    hRecoJeteta->Fill(recojeteta);
    hRecoJetphi->Fill(recojetphi);

    for(JetMap::Iter iter = truth_jets->begin(); iter!=truth_jets->end(); ++iter){
      Jet *jet_t = iter->second;
      
      truthjetpt = jet_t->get_pt();
      if(truthjetpt<10.)
	continue;
      
      truthjeteta = jet_t->get_eta();
      if(fabs(truthjeteta)>2.0)
	continue;

      truthjetphi = jet_t->get_phi();
      truthjetpx = jet_t->get_px();
      truthjetpy = jet_t->get_py();
      truthjetpz = jet_t->get_pz();
      truthjetmass = jet_t->get_mass();
      truthjetp = jet_t->get_p();
      truthjetenergy = jet_t->get_e();    
      truthjetid = jet_t->get_id();      
      
      double delR = deltaR(recojeteta, truthjeteta, recojetphi, truthjetphi);

      if(delR < 0.2){

	recojetsubid = 1;

	HepMC::GenParticle* parton = recoeval->get_mother_parton(jet, theEvent, 0.4);
	if(!parton)
	  parton_flavor = -1;
	else parton_flavor = parton->pdg_id());

	int ptbin = -1, etabin = -1;
	
	for(unsigned x = 0; x<nbins_pt; x++){
	  if(truthjetpt > ptbins[x]){
	    ptbin = x;
	  }
	}
	
	for(unsigned y = 0; y<nbins_eta; y++){
	  if(truthjeteta > etabins[y]){
	    etabin = y;
	  }
	}
	
	double jer = recojetpt/truthjetpt;
	recogenpTratio = jer;
	delRrecotruth = delR;
	
	hJER_allpT_allEta->Fill(jer);
	
	hRecoGenJetdelR->Fill(delR);
	
	if(etabin != -1 && ptbin != -1)      
	  hJER[ptbin][etabin]->Fill(jer);
	
      }else {
	
	truthjetid = 0;
	truthjetp = 0;
	truthjetphi = 0;
	truthjeteta = 0;
	truthjetpt = 0;
	truthjetenergy = 0;
	truthjetpx = 0;
	truthjetpy = 0;
	truthjetpz = 0;
	delRrecotruth = 0;
	recogenpTratio = 0;
	recojetsubid = 0;
	parton_flavor = 0;
	
      }      
    
    }

    recojettree->Fill();
  
  }
  



  for(JetMap::Iter iter = reco_noHcalIn_jets->begin(); iter!=reco_noHcalIn_jets->end(); ++iter){
    Jet *jet = iter->second;
    reco_noHcalIn_jetpt = jet->get_pt();
    if(reco_noHcalIn_jetpt<4.)
      continue;    
    
    reco_noHcalIn_jeteta = jet->get_eta();
    if(fabs(reco_noHcalIn_jeteta)>2.0)
      continue;
    reco_noHcalIn_jetid = jet->get_id();
    reco_noHcalIn_jetpx = jet->get_px();
    reco_noHcalIn_jetpy = jet->get_py();
    reco_noHcalIn_jetpz = jet->get_pz();
    reco_noHcalIn_jetphi = jet->get_phi();
    reco_noHcalIn_jetmass = jet->get_mass();
    reco_noHcalIn_jetp = jet->get_p();
    reco_noHcalIn_jetenergy = jet->get_e();

    hReco_NoHcalIn_JetpT->Fill(reco_noHcalIn_jetpt);
    hReco_NoHcalIn_Jeteta->Fill(reco_noHcalIn_jeteta);
    hReco_NoHcalIn_Jetphi->Fill(reco_noHcalIn_jetphi);

    for(JetMap::Iter iter = truth_jets->begin(); iter!=truth_jets->end(); ++iter){
      Jet *jet_t = iter->second;
      
      truthjetpt = jet_t->get_pt();
      if(truthjetpt<10.)
	continue;
      
      truthjeteta = jet_t->get_eta();
      if(fabs(truthjeteta)>2.0)
	continue;

      truthjetphi = jet_t->get_phi();
      truthjetpx = jet_t->get_px();
      truthjetpy = jet_t->get_py();
      truthjetpz = jet_t->get_pz();
      truthjetmass = jet_t->get_mass();
      truthjetp = jet_t->get_p();
      truthjetenergy = jet_t->get_e();    
      truthjetid = jet_t->get_id();      
      
      double delR = deltaR(reco_noHcalIn_jeteta, truthjeteta, reco_noHcalIn_jetphi, truthjetphi);

      if(delR < 0.2){

	reco_noHcalIn_jetsubid = 1;

	HepMC::GenParticle* parton = recoeval->get_mother_parton(jet, theEvent, 0.4);
	if(!parton)
	  reco_noHcalIn_parton_flavor = -1;
	else
	  reco_noHcalIn_parton_flavor = parton->pdg_id());

	int ptbin = -1, etabin = -1;
	
	for(unsigned x = 0; x<nbins_pt; x++){
	  if(truthjetpt > ptbins[x]){
	    ptbin = x;
	  }
	}
	
	for(unsigned y = 0; y<nbins_eta; y++){
	  if(truthjeteta > etabins[y]){
	    etabin = y;
	  }
	}
	
	double jer = reco_noHcalIn_jetpt/truthjetpt;
	reco_noHcalIn_genpTratio = jer;
	delRreco_noHcalIn_truth = delR;
	
	hJER_noHcalIn_allpT_allEta->Fill(jer);
	
	hReco_NoHcalIn_GenJetdelR->Fill(delR);
	
	if(etabin != -1 && ptbin != -1)      
	  hJER_noHcalIn[ptbin][etabin]->Fill(jer);
	
      }else {
	
	truthjetid = 0;
	truthjetp = 0;
	truthjetphi = 0;
	truthjeteta = 0;
	truthjetpt = 0;
	truthjetenergy = 0;
	truthjetpx = 0;
	truthjetpy = 0;
	truthjetpz = 0;
	delRreco_noHcalIn_truth = 0;
	reco_noHcalIn_genpTratio = 0;
	reco_noHcalIn_jetsubid = 0;
	reco_noHcalIn_parton_flavor = 0;
	
      }
    
  
    }
      
    reco_noHcalIn_jettree->Fill();      

  }
  

  
  
  nevents++;
  tree->Fill();
  return 0;

}


/*

//! tag jet flavor by parton matching, like PRL 113, 132301 (2014)
int JetPerformance::parton_tagging(Jet * this_jet,
				   HepMC::GenEvent* theEvent,
				   const double match_radius = 0.4)
{
  // float this_pt = this_jet->get_pt();
  float this_phi = this_jet->get_phi();
  float this_eta = this_jet->get_eta();

  int jet_flavor = -1;
  // double jet_parton_zt = 0;

  //std::cout << " truth jet #" << ijet_t << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << ", checking flavor" << std::endl;

  //TODO: lack taggign scheme of gluon splitting -> QQ_bar
  for (HepMC::GenEvent::particle_const_iterator p = theEvent->particles_begin(); p != theEvent->particles_end(); ++p) {

    int particleID = abs((*p)->pdg_id());
    
    //! check to see if its quark or gluon
    if(particleID < 6 || particleID == 21){
    
      float dR = deltaR((*p)->momentum().pseudoRapidity(), this_eta,
			(*p)->momentum().phi(), this_phi);
      if (dR < match_radius)
	jet_flavor = particleID;
      
      // int pidabs = abs((*p)->pdg_id());
      // const double zt = (*p)->momentum().perp() / this_pt;
      
      // jet_parton_zt = zt;
    }
  }
  
  return jet_flavor;
}

*/


int JetPerformance::End(PHCompositeNode *topnode)
{
  std::cout<<" DONE PROCESSING "<<std::endl;
  
  
  file->Write();
  file->Close();
  return 0;
}



void JetPerformance::Set_Tree_Branches()
{

  

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
  recojettree->Branch("recogenpTratio",&recogenpTratio,"recogenpTratio/F");
  recojettree->Branch("delRrecotruth",&delRrecotruth,"delRrecotruth/F");
  recojettree->Branch("recojetsubid",&recojetsubid,"recojetsubid/I");
  recojettree->Branch("parton_flavor",&parton_flavor,"parton_flavor/I");

  reco_noHcalIn_jettree = new TTree("reco_noHcalIn_jettree","a tree with reco_noHcalIn_ jets");
  reco_noHcalIn_jettree->Branch("reco_noHcalIn_jetpt",&reco_noHcalIn_jetpt,"reco_noHcalIn_jetpt/F");
  reco_noHcalIn_jettree->Branch("reco_noHcalIn_jetpx",&reco_noHcalIn_jetpx,"reco_noHcalIn_jetpx/F");
  reco_noHcalIn_jettree->Branch("reco_noHcalIn_jetpy",&reco_noHcalIn_jetpy,"reco_noHcalIn_jetpy/F");
  reco_noHcalIn_jettree->Branch("reco_noHcalIn_jetpz",&reco_noHcalIn_jetpz,"reco_noHcalIn_jetpz/F");
  reco_noHcalIn_jettree->Branch("reco_noHcalIn_jetphi",&reco_noHcalIn_jetphi,"reco_noHcalIn_jetphi/F");
  reco_noHcalIn_jettree->Branch("reco_noHcalIn_jeteta",&reco_noHcalIn_jeteta,"reco_noHcalIn_jeteta/F");
  reco_noHcalIn_jettree->Branch("reco_noHcalIn_jetmass",&reco_noHcalIn_jetmass,"reco_noHcalIn_jetmass/F");
  reco_noHcalIn_jettree->Branch("reco_noHcalIn_jetp",&reco_noHcalIn_jetp,"reco_noHcalIn_jetp/F");
  reco_noHcalIn_jettree->Branch("reco_noHcalIn_jetenergy",&reco_noHcalIn_jetenergy,"reco_noHcalIn_jetenergy/F");
  reco_noHcalIn_jettree->Branch("nevents",&nevents,"nevents/I");
  reco_noHcalIn_jettree->Branch("reco_noHcalIn_jetid",&reco_noHcalIn_jetid,"reco_noHcalIn_jetid/F");
  reco_noHcalIn_jettree->Branch("truthjetid",&truthjetid,"truthjetid/F");
  reco_noHcalIn_jettree->Branch("truthjetp",&truthjetp,"truthjetp/F");
  reco_noHcalIn_jettree->Branch("truthjetphi",&truthjetphi,"truthjetphi/F");
  reco_noHcalIn_jettree->Branch("truthjeteta",&truthjeteta,"truthjeteta/F");
  reco_noHcalIn_jettree->Branch("truthjetpt",&truthjetpt,"truthjetpt/F");
  reco_noHcalIn_jettree->Branch("truthjetenergy",&truthjetenergy,"truthjetenergy/F");
  reco_noHcalIn_jettree->Branch("truthjetpx",&truthjetpx,"truthjetpx/F");
  reco_noHcalIn_jettree->Branch("truthjetpy",&truthjetpy,"truthjetpy/F");
  reco_noHcalIn_jettree->Branch("truthjetpz",&truthjetpz,"truthjetpz/F");
  reco_noHcalIn_jettree->Branch("reco_noHcalIn_genpTratio",&reco_noHcalIn_genpTratio,"reco_noHcalIn_genpTratio/F");
  reco_noHcalIn_jettree->Branch("delRreco_noHcalIn_truth",&delRreco_noHcalIn_truth,"delRreco_noHcalIn_truth/F");
  reco_noHcalIn_jettree->Branch("reco_noHcalIn_jetsubid",&reco_noHcalIn_jetsubid,"reco_noHcalIn_jetsubid/I");
  reco_noHcalIn_jettree->Branch("reco_noHcalIn_parton_flavor",&reco_noHcalIn_parton_flavor,"reco_noHcalIn_parton_flavor/I");

}



