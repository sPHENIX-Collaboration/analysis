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
#include <phhepmc/PHHepMCGenEventMap.h>
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
  tree->Branch("weight",&weight, "weight/F");
  tree->Branch("signal_processID",&signal_processID, "signal_processID/I");
  tree->Branch("mpi",&mpi, "mpi/I");
  tree->Branch("event_scale",&event_scale, "event_scale/I");
  tree->Branch("alpha_QCD",&alpha_QCD, "alpha_QCD/I");
  tree->Branch("alpha_QED",&alpha_QED, "alpha_QED/I");

  Set_Tree_Branches();

  hRecoJetpT_R02 = new TH1F("hRecoJetpT_R02","Reco jet pT spectra",nbins_pt, ptbins);
  hRecoJeteta_R02 = new TH1F("hRecoJeteta_R02","Reco jet eta spectra",nbins_eta, etabins);
  hRecoJetphi_R02 = new TH1F("hRecoJetphi_R02","Reco jet phi spectra",30, -1*pi, pi);

  hReco_NoHcalIn_JetpT_R02 = new TH1F("hReco_NoHcalIn_JetpT_R02","Reco_NoHcalIn_ jet pT spectra",nbins_pt, ptbins);
  hReco_NoHcalIn_Jeteta_R02 = new TH1F("hReco_NoHcalIn_Jeteta_R02","Reco_NoHcalIn_ jet eta spectra",nbins_eta, etabins);
  hReco_NoHcalIn_Jetphi_R02 = new TH1F("hReco_NoHcalIn_Jetphi_R02","Reco_NoHcalIn_ jet phi spectra",30, -1*pi, pi);
  hReco_NoHcalIn_GenJetdelR_R02 = new TH1F("hReco_NoHcalIn_GenJetdelR_R02", "Delta R between reco_noHcalIn_ jet and gen jet",25, 0, 0.5);

  hGenJetpT_R02 = new TH1F("hGenJetpT_R02","Gen jet pT spectra",nbins_pt, ptbins);
  hGenJeteta_R02 = new TH1F("hGenJeteta_R02","Gen jet eta spectra",nbins_eta, etabins);
  hGenJetphi_R02 = new TH1F("hGenJetphi_R02","Gen jet phi spectra",30, -1*pi, pi);

  hRecoGenJetdelR_R02 = new TH1F("hRecoGenJetdelR_R02", "Delta R between reco jet and gen jet",25, 0, 0.5);

  hJER_allpT_allEta_R02 = new TH1F("hJER_allpT_allEta_R02","", 50, 0, 2);
  hJER_noHcalIn_allpT_allEta_R02 = new TH1F("hJER_noHcalIn_allpT_allEta_R02","", 50, 0, 2);

  for(int i = 0; i<nbins_pt; ++i){
    for(int j = 0; j<nbins_eta; ++j){
      hJER_R02[i][j] = new TH1F(Form("hJER_R02_ptbin%d_etabin%d",i, j),"",50, 0, 2);
      hJER_noHcalIn_R02[i][j] = new TH1F(Form("hJER_R02_noHcalIn_ptbin%d_etabin%d",i, j),"",50, 0, 2);
    }
  }

  hRecoJetpT_R03 = new TH1F("hRecoJetpT_R03","Reco jet pT spectra",nbins_pt, ptbins);
  hRecoJeteta_R03 = new TH1F("hRecoJeteta_R03","Reco jet eta spectra",nbins_eta, etabins);
  hRecoJetphi_R03 = new TH1F("hRecoJetphi_R03","Reco jet phi spectra",30, -1*pi, pi);

  hReco_NoHcalIn_JetpT_R03 = new TH1F("hReco_NoHcalIn_JetpT_R03","Reco_NoHcalIn_ jet pT spectra",nbins_pt, ptbins);
  hReco_NoHcalIn_Jeteta_R03 = new TH1F("hReco_NoHcalIn_Jeteta_R03","Reco_NoHcalIn_ jet eta spectra",nbins_eta, etabins);
  hReco_NoHcalIn_Jetphi_R03 = new TH1F("hReco_NoHcalIn_Jetphi_R03","Reco_NoHcalIn_ jet phi spectra",30, -1*pi, pi);
  hReco_NoHcalIn_GenJetdelR_R03 = new TH1F("hReco_NoHcalIn_GenJetdelR_R03", "Delta R between reco_noHcalIn_ jet and gen jet",25, 0, 0.5);

  hGenJetpT_R03 = new TH1F("hGenJetpT_R03","Gen jet pT spectra",nbins_pt, ptbins);
  hGenJeteta_R03 = new TH1F("hGenJeteta_R03","Gen jet eta spectra",nbins_eta, etabins);
  hGenJetphi_R03 = new TH1F("hGenJetphi_R03","Gen jet phi spectra",30, -1*pi, pi);

  hRecoGenJetdelR_R03 = new TH1F("hRecoGenJetdelR_R03", "Delta R between reco jet and gen jet",25, 0, 0.5);

  hJER_allpT_allEta_R03 = new TH1F("hJER_allpT_allEta_R03","", 50, 0, 2);
  hJER_noHcalIn_allpT_allEta_R03 = new TH1F("hJER_noHcalIn_allpT_allEta_R03","", 50, 0, 2);

  for(int i = 0; i<nbins_pt; ++i){
    for(int j = 0; j<nbins_eta; ++j){
      hJER_R03[i][j] = new TH1F(Form("hJER_R03_ptbin%d_etabin%d",i, j),"",50, 0, 2);
      hJER_noHcalIn_R03[i][j] = new TH1F(Form("hJER_R03_noHcalIn_ptbin%d_etabin%d",i, j),"",50, 0, 2);
    }
  }


  hRecoJetpT_R04 = new TH1F("hRecoJetpT_R04","Reco jet pT spectra",nbins_pt, ptbins);
  hRecoJeteta_R04 = new TH1F("hRecoJeteta_R04","Reco jet eta spectra",nbins_eta, etabins);
  hRecoJetphi_R04 = new TH1F("hRecoJetphi_R04","Reco jet phi spectra",30, -1*pi, pi);

  hReco_NoHcalIn_JetpT_R04 = new TH1F("hReco_NoHcalIn_JetpT_R04","Reco_NoHcalIn_ jet pT spectra",nbins_pt, ptbins);
  hReco_NoHcalIn_Jeteta_R04 = new TH1F("hReco_NoHcalIn_Jeteta_R04","Reco_NoHcalIn_ jet eta spectra",nbins_eta, etabins);
  hReco_NoHcalIn_Jetphi_R04 = new TH1F("hReco_NoHcalIn_Jetphi_R04","Reco_NoHcalIn_ jet phi spectra",30, -1*pi, pi);
  hReco_NoHcalIn_GenJetdelR_R04 = new TH1F("hReco_NoHcalIn_GenJetdelR_R04", "Delta R between reco_noHcalIn_ jet and gen jet",25, 0, 0.5);

  hGenJetpT_R04 = new TH1F("hGenJetpT_R04","Gen jet pT spectra",nbins_pt, ptbins);
  hGenJeteta_R04 = new TH1F("hGenJeteta_R04","Gen jet eta spectra",nbins_eta, etabins);
  hGenJetphi_R04 = new TH1F("hGenJetphi_R04","Gen jet phi spectra",30, -1*pi, pi);

  hRecoGenJetdelR_R04 = new TH1F("hRecoGenJetdelR_R04", "Delta R between reco jet and gen jet",25, 0, 0.5);

  hJER_allpT_allEta_R04 = new TH1F("hJER_allpT_allEta_R04","", 50, 0, 2);
  hJER_noHcalIn_allpT_allEta_R04 = new TH1F("hJER_noHcalIn_allpT_allEta_R04","", 50, 0, 2);

  for(int i = 0; i<nbins_pt; ++i){
    for(int j = 0; j<nbins_eta; ++j){
      hJER_R04[i][j] = new TH1F(Form("hJER_R04_ptbin%d_etabin%d",i, j),"",50, 0, 2);
      hJER_noHcalIn_R04[i][j] = new TH1F(Form("hJER_R04_noHcalIn_ptbin%d_etabin%d",i, j),"",50, 0, 2);
    }
  }

  
 
  return 0;
}



int JetPerformance::process_event(PHCompositeNode *topnode)
{
 
  cout<<"at event number "<<nevents<<endl;

  //get the nodes from the NodeTree
  JetMap* truth_jets_R02 = findNode::getClass<JetMap>(topnode,"AntiKt_Truth_r02");
  JetMap *reco_jets_R02 = findNode::getClass<JetMap>(topnode,"AntiKt_Tower_r02");
  JetMap *reco_noHcalIn_jets_R02 = findNode::getClass<JetMap>(topnode,"AntiKt_Tower_noHcalIn_r02");
  // std::cout<<"Got the R = 0.2 Jets"<<std::endl;
  // JetMap *reco_jets_R02 = findNode::getClass<JetMap>(topnode,"AntiKt_Tower_r02_Sub1");
  // JetMap *reco_jets_noHcalIn_R02 = findNode::getClass<JetMap>(topnode,"AntiKt_Tower_noHcalIn_r02_Sub1");
  // JetEvalStack* _jetevalstack_R02 = new JetEvalStack(topnode,"AntiKt_Tower_r02","AntiKt_Truth_r02");
  JetMap* truth_jets_R03 = findNode::getClass<JetMap>(topnode,"AntiKt_Truth_r03");
  JetMap *reco_jets_R03 = findNode::getClass<JetMap>(topnode,"AntiKt_Tower_r03");
  JetMap *reco_noHcalIn_jets_R03 = findNode::getClass<JetMap>(topnode,"AntiKt_Tower_noHcalIn_r03");
  // std::cout<<"Got the R = 0.3 Jets"<<std::endl;
  // JetMap *reco_jets_R03 = findNode::getClass<JetMap>(topnode,"AntiKt_Tower_r03_Sub1");
  // JetMap *reco_jets_noHcalIn_R03 = findNode::getClass<JetMap>(topnode,"AntiKt_Tower_noHcalIn_r03_Sub1");
  // JetEvalStack* _jetevalstack_R03 = new JetEvalStack(topnode,"AntiKt_Tower_r03","AntiKt_Truth_r03");
  JetMap* truth_jets_R04 = findNode::getClass<JetMap>(topnode,"AntiKt_Truth_r04");
  JetMap *reco_jets_R04 = findNode::getClass<JetMap>(topnode,"AntiKt_Tower_r04");
  JetMap *reco_noHcalIn_jets_R04 = findNode::getClass<JetMap>(topnode,"AntiKt_Tower_noHcalIn_r04");
  // std::cout<<"Got the R = 0.4 Jets"<<std::endl;
  // JetMap *reco_jets_R04 = findNode::getClass<JetMap>(topnode,"AntiKt_Tower_r04_Sub1");
  // JetMap *reco_jets_noHcalIn_R04 = findNode::getClass<JetMap>(topnode,"AntiKt_Tower_noHcalIn_r04_Sub1");
  // JetEvalStack* _jetevalstack_R04 = new JetEvalStack(topnode,"AntiKt_Tower_r04","AntiKt_Truth_r04");
  PHG4TruthInfoContainer* truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topnode,"G4TruthInfo");
  PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();
  // std::cout<<"Got the Truth particles "<<std::endl;

  PHHepMCGenEventMap *genevtMap = findNode::getClass<PHHepMCGenEventMap>(topnode, "PHHepMCGenEventMap");
  // genevtMap->identify();
  // if(!genevtMap){
  //   std::cout<<"failed to get the gen event Map"<<std::endl;
  //   return 0;
  // }
  
  PHHepMCGenEvent *genevt = genevtMap->get(0);
  HepMC::GenEvent* theEvent = genevt->getEvent();
  HepMC::WeightContainer eventweights = theEvent->weights();
  // std::cout<<"Got the gen event and the weights"<<std::endl;

  if(eventweights.empty()){
    cout<<"event has no weight"<<endl;
    return 0;
  }

  // cout<<"first weight = "<<eventweights.front()<<endl;
  // eventweights.print();
  
  weight = (!theEvent->weights().empty()) ? theEvent->weights()[0] : 1.0;
  signal_processID = theEvent->signal_process_id();
  mpi = theEvent->mpi();
  event_scale = theEvent->event_scale();
  alpha_QCD = theEvent->alphaQCD();
  alpha_QED = theEvent->alphaQED();

  // std::cout<<"event weight = "<<theEvent->weight()<<std::endl;
  
  if(!truthinfo){
    std::cout<<"no truth track info"<<std::endl;
    return 0;
  }
  if(!truth_jets_R02){
    cout<<"no truth R02 jets"<<endl;
    return 0;
  }
  if(!reco_jets_R02){
    cout<<"no reco R02 jets"<<endl;
    return 0;
  }
  if(!reco_noHcalIn_jets_R02){
    cout<<"no reco R02 no hcalin jets"<<endl;
    return 0;
  }
  if(!truth_jets_R03){
    cout<<"no truth R03 jets"<<endl;
    return 0;
  }
  if(!reco_jets_R03){
    cout<<"no reco R03 jets"<<endl;
    return 0;
  }
  if(!reco_noHcalIn_jets_R03){
    cout<<"no reco R03 no hcalin jets"<<endl;
    return 0;
  }
  if(!truth_jets_R04){
    cout<<"no truth R04 jets"<<endl;
    return 0;
  }
  if(!reco_jets_R04){
    cout<<"no reco R04 jets"<<endl;
    return 0;
  }
  if(!reco_noHcalIn_jets_R04){
    cout<<"no reco R04 no hcalin jets"<<endl;
    return 0;
  }
  // if(!_jetevalstack){
  //   cout<<"no good truth jets"<<endl;
  //   return 0;
  // }


  // JetRecoEval* recoeval = _jetevalstack->get_reco_eval();

  //! Clear all the vectors
  clearallvectors();

  //! ******** Truth particles *********  
  for( PHG4TruthInfoContainer::ConstIterator iter = range.first; iter!=range.second; ++iter){

    PHG4Particle *truth = iter->second;
    
    truthpx.push_back(truth->get_px());
    truthpy.push_back(truth->get_py());
    truthpz.push_back(truth->get_pz());
    truthp.push_back(sqrt(truth->get_px()*truth->get_px()+truth->get_py()*truth->get_py()+truth->get_pz()*truth->get_pz()));
    truthenergy.push_back(truth->get_e());
    
    TLorentzVector vec;
    vec.SetPxPyPzE(truth->get_px(),truth->get_py(),truth->get_pz(),truth->get_e());
    
    truthpt.push_back(sqrt(truth->get_px()*truth->get_px()+truth->get_py()*truth->get_py()));    
    truthphi.push_back(vec.Phi());
    trutheta.push_back(vec.Eta());
    truthpid.push_back(truth->get_pid());
    

  }

  
  /***************************************

   TRUTH JETS

  ***************************************/


  for(JetMap::Iter iter = truth_jets_R02->begin(); iter!=truth_jets_R02->end(); ++iter){

    Jet *jet = iter->second;

    if(jet->get_pt()<5.)
      continue;
    if(fabs(jet->get_eta())>2.0)
      continue;
    
    truthjetpt_R02.push_back(jet->get_pt());
    truthjeteta_R02.push_back(jet->get_eta());
    truthjetpx_R02.push_back(jet->get_px());
    truthjetpy_R02.push_back(jet->get_py());
    truthjetpz_R02.push_back(jet->get_pz());
    truthjetphi_R02.push_back(jet->get_phi());
    truthjetmass_R02.push_back(jet->get_mass());
    truthjetp_R02.push_back(jet->get_p());
    truthjetenergy_R02.push_back(jet->get_e());
    

    hGenJetpT_R02->Fill(jet->get_pt());
    hGenJeteta_R02->Fill(jet->get_eta());
    hGenJetphi_R02->Fill(jet->get_phi());    

  }


  for(JetMap::Iter iter = truth_jets_R03->begin(); iter!=truth_jets_R03->end(); ++iter){

    Jet *jet = iter->second;

    if(jet->get_pt()<5.)
      continue;
    if(fabs(jet->get_eta())>2.0)
      continue;
    
    truthjetpt_R03.push_back(jet->get_pt());
    truthjeteta_R03.push_back(jet->get_eta());
    truthjetpx_R03.push_back(jet->get_px());
    truthjetpy_R03.push_back(jet->get_py());
    truthjetpz_R03.push_back(jet->get_pz());
    truthjetphi_R03.push_back(jet->get_phi());
    truthjetmass_R03.push_back(jet->get_mass());
    truthjetp_R03.push_back(jet->get_p());
    truthjetenergy_R03.push_back(jet->get_e());
    

    hGenJetpT_R03->Fill(jet->get_pt());
    hGenJeteta_R03->Fill(jet->get_eta());
    hGenJetphi_R03->Fill(jet->get_phi());    

  }


  for(JetMap::Iter iter = truth_jets_R04->begin(); iter!=truth_jets_R04->end(); ++iter){

    Jet *jet = iter->second;

    if(jet->get_pt()<5.)
      continue;
    if(fabs(jet->get_eta())>2.0)
      continue;
    
    truthjetpt_R04.push_back(jet->get_pt());
    truthjeteta_R04.push_back(jet->get_eta());
    truthjetpx_R04.push_back(jet->get_px());
    truthjetpy_R04.push_back(jet->get_py());
    truthjetpz_R04.push_back(jet->get_pz());
    truthjetphi_R04.push_back(jet->get_phi());
    truthjetmass_R04.push_back(jet->get_mass());
    truthjetp_R04.push_back(jet->get_p());
    truthjetenergy_R04.push_back(jet->get_e());
    

    hGenJetpT_R04->Fill(jet->get_pt());
    hGenJeteta_R04->Fill(jet->get_eta());
    hGenJetphi_R04->Fill(jet->get_phi());    

  }



  /***************************************

   RECONSTRUCTED JETS

  ***************************************/



  for(JetMap::Iter iter = reco_jets_R02->begin(); iter!=reco_jets_R02->end(); ++iter){

    Jet *jet = iter->second;
    if(jet->get_pt()<5.)
      continue;        
    if(fabs(jet->get_eta())>2.0)
      continue;
    
    recojeteta_R02.push_back(jet->get_eta());
    recojetpt_R02.push_back(jet->get_pt());
    recojetid_R02.push_back(jet->get_id());
    recojetpx_R02.push_back(jet->get_px());
    recojetpy_R02.push_back(jet->get_py());
    recojetpz_R02.push_back(jet->get_pz());
    recojetphi_R02.push_back(jet->get_phi());
    recojetmass_R02.push_back(jet->get_mass());
    recojetp_R02.push_back(jet->get_p());
    recojetenergy_R02.push_back(jet->get_e());

    hRecoJetpT_R02->Fill(jet->get_pt());
    hRecoJeteta_R02->Fill(jet->get_eta());
    hRecoJetphi_R02->Fill(jet->get_phi());

    for(JetMap::Iter iter = truth_jets_R02->begin(); iter!=truth_jets_R02->end(); ++iter){
      Jet *jet_t = iter->second;
      
      if(jet_t->get_pt()<5.)
      	continue;      
      if(fabs(jet_t->get_eta())>2.0)
      	continue;

      truthjetpt_reco_R02.push_back(jet_t->get_pt());
      truthjeteta_reco_R02.push_back(jet_t->get_eta());
      truthjetphi_reco_R02.push_back(jet_t->get_phi());
      truthjetpx_reco_R02.push_back(jet_t->get_px());
      truthjetpy_reco_R02.push_back(jet_t->get_py());
      truthjetpz_reco_R02.push_back(jet_t->get_pz());
      truthjetmass_reco_R02.push_back(jet_t->get_mass());
      truthjetp_reco_R02.push_back(jet_t->get_p());
      truthjetenergy_reco_R02.push_back(jet_t->get_e());    
      truthjetid_R02.push_back(jet_t->get_id());      
      
      double delR = deltaR(jet->get_eta(), jet_t->get_eta(), jet->get_phi(), jet_t->get_phi());

      if(delR < 0.2){

	recojetsubid_R02.push_back(1);

	// HepMC::GenParticle* parton = recoeval->get_mother_parton(jet, theEvent, 0.4);
	// if(!parton)
	//   parton_flavor = -1;
	// else parton_flavor = parton->pdg_id();
	parton_flavor_R02.push_back(parton_tagging(jet, theEvent, 0.2));
	parton_frag_zt_R02.push_back(parton_frag(jet, theEvent, 0.2));
	
	int ptbin = -1, etabin = -1;
	
	for(unsigned x = 0; x<nbins_pt; x++){
	  if(jet_t->get_pt() > ptbins[x]){
	    ptbin = x;
	  }
	}
	
	for(unsigned y = 0; y<nbins_eta; y++){
	  if(jet_t->get_eta() > etabins[y]){
	    etabin = y;
	  }
	}
	
	double jer = jet->get_pt()/jet_t->get_pt();

	recogenpTratio_R02.push_back(jer);
	delRrecotruth_R02.push_back(delR);
	
	hJER_allpT_allEta_R02->Fill(jer);
	
	hRecoGenJetdelR_R02->Fill(delR);
	
	if(etabin != -1 && ptbin != -1)      
	  hJER_R02[ptbin][etabin]->Fill(jer);
	
      }else {
	
	truthjetid_R02.push_back(-999);
	truthjetp_reco_R02.push_back(-999);
	truthjetphi_reco_R02.push_back(-999);
	truthjeteta_reco_R02.push_back(-999);
	truthjetpt_reco_R02.push_back(-999);
	truthjetenergy_reco_R02.push_back(-999);
	truthjetpx_reco_R02.push_back(-999);
	truthjetpy_reco_R02.push_back(-999);
	truthjetpz_reco_R02.push_back(-999);
	delRrecotruth_R02.push_back(-999);
	recogenpTratio_R02.push_back(-999);
	recojetsubid_R02.push_back(-999);
	parton_flavor_R02.push_back(-999);
	
      }      
    
    }

  
  }
  



  for(JetMap::Iter iter = reco_noHcalIn_jets_R02->begin(); iter!=reco_noHcalIn_jets_R02->end(); ++iter){
    Jet *jet = iter->second;

    if(jet->get_pt()<5.)
      continue;
    if(fabs(jet->get_eta())>2.0)
      continue;

    reco_noHcalIn_jetpt_R02.push_back(jet->get_pt());
    reco_noHcalIn_jeteta_R02.push_back(jet->get_eta());
    reco_noHcalIn_jetid_R02.push_back(jet->get_id());
    reco_noHcalIn_jetpx_R02.push_back(jet->get_px());
    reco_noHcalIn_jetpy_R02.push_back(jet->get_py());
    reco_noHcalIn_jetpz_R02.push_back(jet->get_pz());
    reco_noHcalIn_jetphi_R02.push_back(jet->get_phi());
    reco_noHcalIn_jetmass_R02.push_back(jet->get_mass());
    reco_noHcalIn_jetp_R02.push_back(jet->get_p());
    reco_noHcalIn_jetenergy_R02.push_back(jet->get_e());

    hReco_NoHcalIn_JetpT_R02->Fill(jet->get_pt());
    hReco_NoHcalIn_Jeteta_R02->Fill(jet->get_eta());
    hReco_NoHcalIn_Jetphi_R02->Fill(jet->get_phi());

    for(JetMap::Iter iter = truth_jets_R02->begin(); iter!=truth_jets_R02->end(); ++iter){
      Jet *jet_t = iter->second;
      
      if(jet_t->get_pt()<5.)
      	continue;      
      if(fabs(jet_t->get_eta())>2.0)
      	continue;

      truthjetpt_reco_noHcalIn_R02.push_back(jet_t->get_pt());
      truthjeteta_reco_noHcalIn_R02.push_back(jet_t->get_eta());
      truthjetphi_reco_noHcalIn_R02.push_back(jet_t->get_phi());
      truthjetpx_reco_noHcalIn_R02.push_back(jet_t->get_px());
      truthjetpy_reco_noHcalIn_R02.push_back(jet_t->get_py());
      truthjetpz_reco_noHcalIn_R02.push_back(jet_t->get_pz());
      truthjetmass_reco_noHcalIn_R02.push_back(jet_t->get_mass());
      truthjetp_reco_noHcalIn_R02.push_back(jet_t->get_p());
      truthjetenergy_reco_noHcalIn_R02.push_back(jet_t->get_e());    
      truthjetid_R02.push_back(jet_t->get_id());      
      
      double delR = deltaR(jet->get_eta(), jet_t->get_eta(), jet->get_phi(), jet_t->get_phi());

      if(delR < 0.2){

	reco_noHcalIn_jetsubid_R02.push_back(1);

	// HepMC::GenParticle* parton = recoeval->get_mother_parton(jet, theEvent, 0.4);
	// if(!parton)
	//   reco_noHcalIn_parton_flavor = -1;
	// else
	//   reco_noHcalIn_parton_flavor = parton->pdg_id();
	reco_noHcalIn_parton_flavor_R02.push_back(parton_tagging(jet, theEvent, 0.2));
	reco_noHcalIn_parton_frag_zt_R02.push_back(parton_frag(jet, theEvent, 0.2));

	int ptbin = -1, etabin = -1;
	
	for(unsigned x = 0; x<nbins_pt; x++){
	  if(jet_t->get_pt() > ptbins[x]){
	    ptbin = x;
	  }
	}
	
	for(unsigned y = 0; y<nbins_eta; y++){
	  if(jet_t->get_eta() > etabins[y]){
	    etabin = y;
	  }
	}
	
	double jer = jet->get_pt()/jet_t->get_pt();

	reco_noHcalIn_genpTratio_R02.push_back(jer);
	delRreco_noHcalIn_truth_R02.push_back(delR);
	
	hJER_noHcalIn_allpT_allEta_R02->Fill(jer);
	
	hReco_NoHcalIn_GenJetdelR_R02->Fill(delR);
	
	if(etabin != -1 && ptbin != -1)      
	  hJER_noHcalIn_R02[ptbin][etabin]->Fill(jer);
	
      }else {
	
	truthjetid_R02.push_back(-999);
	truthjetp_reco_noHcalIn_R02.push_back(-999);
	truthjetphi_reco_noHcalIn_R02.push_back(-999);
	truthjeteta_reco_noHcalIn_R02.push_back(-999);
	truthjetpt_reco_noHcalIn_R02.push_back(-999);
	truthjetenergy_reco_noHcalIn_R02.push_back(-999);
	truthjetpx_reco_noHcalIn_R02.push_back(-999);
	truthjetpy_reco_noHcalIn_R02.push_back(-999);
	truthjetpz_reco_noHcalIn_R02.push_back(-999);
	delRreco_noHcalIn_truth_R02.push_back(-999);
	reco_noHcalIn_genpTratio_R02.push_back(-999);
	reco_noHcalIn_jetsubid_R02.push_back(-999);
	reco_noHcalIn_parton_flavor_R02.push_back(-999);
	
      }
    
  
    }
      
  }
  

  
  //! R = 0.3


  for(JetMap::Iter iter = reco_jets_R03->begin(); iter!=reco_jets_R03->end(); ++iter){

    Jet *jet = iter->second;
    if(jet->get_pt()<5.)
      continue;        
    if(fabs(jet->get_eta())>2.0)
      continue;
    
    recojeteta_R03.push_back(jet->get_eta());
    recojetpt_R03.push_back(jet->get_pt());
    recojetid_R03.push_back(jet->get_id());
    recojetpx_R03.push_back(jet->get_px());
    recojetpy_R03.push_back(jet->get_py());
    recojetpz_R03.push_back(jet->get_pz());
    recojetphi_R03.push_back(jet->get_phi());
    recojetmass_R03.push_back(jet->get_mass());
    recojetp_R03.push_back(jet->get_p());
    recojetenergy_R03.push_back(jet->get_e());

    hRecoJetpT_R03->Fill(jet->get_pt());
    hRecoJeteta_R03->Fill(jet->get_eta());
    hRecoJetphi_R03->Fill(jet->get_phi());

    for(JetMap::Iter iter = truth_jets_R03->begin(); iter!=truth_jets_R03->end(); ++iter){
      Jet *jet_t = iter->second;
      
      if(jet_t->get_pt()<5.)
      	continue;      
      if(fabs(jet_t->get_eta())>2.0)
      	continue;

      truthjetpt_reco_R03.push_back(jet_t->get_pt());
      truthjeteta_reco_R03.push_back(jet_t->get_eta());
      truthjetphi_reco_R03.push_back(jet_t->get_phi());
      truthjetpx_reco_R03.push_back(jet_t->get_px());
      truthjetpy_reco_R03.push_back(jet_t->get_py());
      truthjetpz_reco_R03.push_back(jet_t->get_pz());
      truthjetmass_reco_R03.push_back(jet_t->get_mass());
      truthjetp_reco_R03.push_back(jet_t->get_p());
      truthjetenergy_reco_R03.push_back(jet_t->get_e());    
      truthjetid_R03.push_back(jet_t->get_id());      
      
      double delR = deltaR(jet->get_eta(), jet_t->get_eta(), jet->get_phi(), jet_t->get_phi());

      if(delR < 0.3){

	recojetsubid_R03.push_back(1);

	// HepMC::GenParticle* parton = recoeval->get_mother_parton(jet, theEvent, 0.4);
	// if(!parton)
	//   parton_flavor = -1;
	// else parton_flavor = parton->pdg_id();
	parton_flavor_R03.push_back(parton_tagging(jet, theEvent, 0.3));
	parton_frag_zt_R03.push_back(parton_frag(jet, theEvent, 0.3));

	int ptbin = -1, etabin = -1;
	
	for(unsigned x = 0; x<nbins_pt; x++){
	  if(jet_t->get_pt() > ptbins[x]){
	    ptbin = x;
	  }
	}
	
	for(unsigned y = 0; y<nbins_eta; y++){
	  if(jet_t->get_eta() > etabins[y]){
	    etabin = y;
	  }
	}
	
	double jer = jet->get_pt()/jet_t->get_pt();

	recogenpTratio_R03.push_back(jer);
	delRrecotruth_R03.push_back(delR);
	
	hJER_allpT_allEta_R03->Fill(jer);
	
	hRecoGenJetdelR_R03->Fill(delR);
	
	if(etabin != -1 && ptbin != -1)      
	  hJER_R03[ptbin][etabin]->Fill(jer);
	
      }else {
	
	truthjetid_R03.push_back(-999);
	truthjetp_reco_R03.push_back(-999);
	truthjetphi_reco_R03.push_back(-999);
	truthjeteta_reco_R03.push_back(-999);
	truthjetpt_reco_R03.push_back(-999);
	truthjetenergy_reco_R03.push_back(-999);
	truthjetpx_reco_R03.push_back(-999);
	truthjetpy_reco_R03.push_back(-999);
	truthjetpz_reco_R03.push_back(-999);
	delRrecotruth_R03.push_back(-999);
	recogenpTratio_R03.push_back(-999);
	recojetsubid_R03.push_back(-999);
	parton_flavor_R03.push_back(-999);
	
      }      
    
    }

  
  }
  



  for(JetMap::Iter iter = reco_noHcalIn_jets_R03->begin(); iter!=reco_noHcalIn_jets_R03->end(); ++iter){
    Jet *jet = iter->second;

    if(jet->get_pt()<5.)
      continue;
    if(fabs(jet->get_eta())>2.0)
      continue;

    reco_noHcalIn_jetpt_R03.push_back(jet->get_pt());
    reco_noHcalIn_jeteta_R03.push_back(jet->get_eta());
    reco_noHcalIn_jetid_R03.push_back(jet->get_id());
    reco_noHcalIn_jetpx_R03.push_back(jet->get_px());
    reco_noHcalIn_jetpy_R03.push_back(jet->get_py());
    reco_noHcalIn_jetpz_R03.push_back(jet->get_pz());
    reco_noHcalIn_jetphi_R03.push_back(jet->get_phi());
    reco_noHcalIn_jetmass_R03.push_back(jet->get_mass());
    reco_noHcalIn_jetp_R03.push_back(jet->get_p());
    reco_noHcalIn_jetenergy_R03.push_back(jet->get_e());

    hReco_NoHcalIn_JetpT_R03->Fill(jet->get_pt());
    hReco_NoHcalIn_Jeteta_R03->Fill(jet->get_eta());
    hReco_NoHcalIn_Jetphi_R03->Fill(jet->get_phi());

    for(JetMap::Iter iter = truth_jets_R03->begin(); iter!=truth_jets_R03->end(); ++iter){
      Jet *jet_t = iter->second;
      
      if(jet_t->get_pt()<5.)
      	continue;      
      if(fabs(jet_t->get_eta())>2.0)
      	continue;

      truthjetpt_reco_noHcalIn_R03.push_back(jet_t->get_pt());
      truthjeteta_reco_noHcalIn_R03.push_back(jet_t->get_eta());
      truthjetphi_reco_noHcalIn_R03.push_back(jet_t->get_phi());
      truthjetpx_reco_noHcalIn_R03.push_back(jet_t->get_px());
      truthjetpy_reco_noHcalIn_R03.push_back(jet_t->get_py());
      truthjetpz_reco_noHcalIn_R03.push_back(jet_t->get_pz());
      truthjetmass_reco_noHcalIn_R03.push_back(jet_t->get_mass());
      truthjetp_reco_noHcalIn_R03.push_back(jet_t->get_p());
      truthjetenergy_reco_noHcalIn_R03.push_back(jet_t->get_e());    
      truthjetid_R03.push_back(jet_t->get_id());      
      
      double delR = deltaR(jet->get_eta(), jet_t->get_eta(), jet->get_phi(), jet_t->get_phi());

      if(delR < 0.3){

	reco_noHcalIn_jetsubid_R03.push_back(1);

	// HepMC::GenParticle* parton = recoeval->get_mother_parton(jet, theEvent, 0.4);
	// if(!parton)
	//   reco_noHcalIn_parton_flavor = -1;
	// else
	//   reco_noHcalIn_parton_flavor = parton->pdg_id();
	reco_noHcalIn_parton_flavor_R03.push_back(parton_tagging(jet, theEvent, 0.3));
	reco_noHcalIn_parton_frag_zt_R03.push_back(parton_frag(jet, theEvent, 0.3));

	int ptbin = -1, etabin = -1;
	
	for(unsigned x = 0; x<nbins_pt; x++){
	  if(jet_t->get_pt() > ptbins[x]){
	    ptbin = x;
	  }
	}
	
	for(unsigned y = 0; y<nbins_eta; y++){
	  if(jet_t->get_eta() > etabins[y]){
	    etabin = y;
	  }
	}
	
	double jer = jet->get_pt()/jet_t->get_pt();

	reco_noHcalIn_genpTratio_R03.push_back(jer);
	delRreco_noHcalIn_truth_R03.push_back(delR);
	
	hJER_noHcalIn_allpT_allEta_R03->Fill(jer);
	
	hReco_NoHcalIn_GenJetdelR_R03->Fill(delR);
	
	if(etabin != -1 && ptbin != -1)      
	  hJER_noHcalIn_R03[ptbin][etabin]->Fill(jer);
	
      }else {
	
	truthjetid_R03.push_back(-999);
	truthjetp_reco_noHcalIn_R03.push_back(-999);
	truthjetphi_reco_noHcalIn_R03.push_back(-999);
	truthjeteta_reco_noHcalIn_R03.push_back(-999);
	truthjetpt_reco_noHcalIn_R03.push_back(-999);
	truthjetenergy_reco_noHcalIn_R03.push_back(-999);
	truthjetpx_reco_noHcalIn_R03.push_back(-999);
	truthjetpy_reco_noHcalIn_R03.push_back(-999);
	truthjetpz_reco_noHcalIn_R03.push_back(-999);
	delRreco_noHcalIn_truth_R03.push_back(-999);
	reco_noHcalIn_genpTratio_R03.push_back(-999);
	reco_noHcalIn_jetsubid_R03.push_back(-999);
	reco_noHcalIn_parton_flavor_R03.push_back(-999);
	
      }
    
  
    }
      
  }
  

  //! R = 0.4 


  for(JetMap::Iter iter = reco_jets_R04->begin(); iter!=reco_jets_R04->end(); ++iter){

    Jet *jet = iter->second;
    if(jet->get_pt()<5.)
      continue;        
    if(fabs(jet->get_eta())>2.0)
      continue;
    
    recojeteta_R04.push_back(jet->get_eta());
    recojetpt_R04.push_back(jet->get_pt());
    recojetid_R04.push_back(jet->get_id());
    recojetpx_R04.push_back(jet->get_px());
    recojetpy_R04.push_back(jet->get_py());
    recojetpz_R04.push_back(jet->get_pz());
    recojetphi_R04.push_back(jet->get_phi());
    recojetmass_R04.push_back(jet->get_mass());
    recojetp_R04.push_back(jet->get_p());
    recojetenergy_R04.push_back(jet->get_e());

    hRecoJetpT_R04->Fill(jet->get_pt());
    hRecoJeteta_R04->Fill(jet->get_eta());
    hRecoJetphi_R04->Fill(jet->get_phi());

    for(JetMap::Iter iter = truth_jets_R04->begin(); iter!=truth_jets_R04->end(); ++iter){
      Jet *jet_t = iter->second;
      
      if(jet_t->get_pt()<5.)
      	continue;      
      if(fabs(jet_t->get_eta())>2.0)
      	continue;


      truthjetpt_reco_R04.push_back(jet_t->get_pt());
      truthjeteta_reco_R04.push_back(jet_t->get_eta());
      truthjetphi_reco_R04.push_back(jet_t->get_phi());
      truthjetpx_reco_R04.push_back(jet_t->get_px());
      truthjetpy_reco_R04.push_back(jet_t->get_py());
      truthjetpz_reco_R04.push_back(jet_t->get_pz());
      truthjetmass_reco_R04.push_back(jet_t->get_mass());
      truthjetp_reco_R04.push_back(jet_t->get_p());
      truthjetenergy_reco_R04.push_back(jet_t->get_e());    
      truthjetid_R04.push_back(jet_t->get_id());      
      
      double delR = deltaR(jet->get_eta(), jet_t->get_eta(), jet->get_phi(), jet_t->get_phi());

      if(delR < 0.4){

	recojetsubid_R04.push_back(1);

	// HepMC::GenParticle* parton = recoeval->get_mother_parton(jet, theEvent, 0.4);
	// if(!parton)
	//   parton_flavor = -1;
	// else parton_flavor = parton->pdg_id();
	parton_flavor_R04.push_back(parton_tagging(jet, theEvent, 0.4));
	parton_frag_zt_R04.push_back(parton_frag(jet, theEvent, 0.4));

	int ptbin = -1, etabin = -1;
	
	for(unsigned x = 0; x<nbins_pt; x++){
	  if(jet_t->get_pt() > ptbins[x]){
	    ptbin = x;
	  }
	}
	
	for(unsigned y = 0; y<nbins_eta; y++){
	  if(jet_t->get_eta() > etabins[y]){
	    etabin = y;
	  }
	}
	
	double jer = jet->get_pt()/jet_t->get_pt();

	recogenpTratio_R04.push_back(jer);
	delRrecotruth_R04.push_back(delR);
	
	hJER_allpT_allEta_R04->Fill(jer);
	
	hRecoGenJetdelR_R04->Fill(delR);
	
	if(etabin != -1 && ptbin != -1)      
	  hJER_R04[ptbin][etabin]->Fill(jer);
	
      }else {
	
	truthjetid_R04.push_back(-999);
	truthjetp_reco_R04.push_back(-999);
	truthjetphi_reco_R04.push_back(-999);
	truthjeteta_reco_R04.push_back(-999);
	truthjetpt_reco_R04.push_back(-999);
	truthjetenergy_reco_R04.push_back(-999);
	truthjetpx_reco_R04.push_back(-999);
	truthjetpy_reco_R04.push_back(-999);
	truthjetpz_reco_R04.push_back(-999);
	delRrecotruth_R04.push_back(-999);
	recogenpTratio_R04.push_back(-999);
	recojetsubid_R04.push_back(-999);
	parton_flavor_R04.push_back(-999);
	
      }      
    
    }

  
  }
  



  for(JetMap::Iter iter = reco_noHcalIn_jets_R04->begin(); iter!=reco_noHcalIn_jets_R04->end(); ++iter){
    Jet *jet = iter->second;

    if(jet->get_pt()<5.)
      continue;
    if(fabs(jet->get_eta())>2.0)
      continue;

    reco_noHcalIn_jetpt_R04.push_back(jet->get_pt());
    reco_noHcalIn_jeteta_R04.push_back(jet->get_eta());
    reco_noHcalIn_jetid_R04.push_back(jet->get_id());
    reco_noHcalIn_jetpx_R04.push_back(jet->get_px());
    reco_noHcalIn_jetpy_R04.push_back(jet->get_py());
    reco_noHcalIn_jetpz_R04.push_back(jet->get_pz());
    reco_noHcalIn_jetphi_R04.push_back(jet->get_phi());
    reco_noHcalIn_jetmass_R04.push_back(jet->get_mass());
    reco_noHcalIn_jetp_R04.push_back(jet->get_p());
    reco_noHcalIn_jetenergy_R04.push_back(jet->get_e());

    hReco_NoHcalIn_JetpT_R04->Fill(jet->get_pt());
    hReco_NoHcalIn_Jeteta_R04->Fill(jet->get_eta());
    hReco_NoHcalIn_Jetphi_R04->Fill(jet->get_phi());

    for(JetMap::Iter iter = truth_jets_R04->begin(); iter!=truth_jets_R04->end(); ++iter){
      Jet *jet_t = iter->second;
      
      if(jet_t->get_pt()<5.)
      	continue;      
      if(fabs(jet_t->get_eta())>2.0)
      	continue;

      truthjetpt_reco_noHcalIn_R04.push_back(jet_t->get_pt());
      truthjeteta_reco_noHcalIn_R04.push_back(jet_t->get_eta());
      truthjetphi_reco_noHcalIn_R04.push_back(jet_t->get_phi());
      truthjetpx_reco_noHcalIn_R04.push_back(jet_t->get_px());
      truthjetpy_reco_noHcalIn_R04.push_back(jet_t->get_py());
      truthjetpz_reco_noHcalIn_R04.push_back(jet_t->get_pz());
      truthjetmass_reco_noHcalIn_R04.push_back(jet_t->get_mass());
      truthjetp_reco_noHcalIn_R04.push_back(jet_t->get_p());
      truthjetenergy_reco_noHcalIn_R04.push_back(jet_t->get_e());    
      truthjetid_R04.push_back(jet_t->get_id());      
      
      double delR = deltaR(jet->get_eta(), jet_t->get_eta(), jet->get_phi(), jet_t->get_phi());

      if(delR < 0.4){

	reco_noHcalIn_jetsubid_R04.push_back(1);

	// HepMC::GenParticle* parton = recoeval->get_mother_parton(jet, theEvent, 0.4);
	// if(!parton)
	//   reco_noHcalIn_parton_flavor = -1;
	// else
	//   reco_noHcalIn_parton_flavor = parton->pdg_id();
	reco_noHcalIn_parton_flavor_R04.push_back(parton_tagging(jet, theEvent, 0.4));
	reco_noHcalIn_parton_frag_zt_R04.push_back(parton_frag(jet, theEvent, 0.4));

	int ptbin = -1, etabin = -1;
	
	for(unsigned x = 0; x<nbins_pt; x++){
	  if(jet_t->get_pt() > ptbins[x]){
	    ptbin = x;
	  }
	}
	
	for(unsigned y = 0; y<nbins_eta; y++){
	  if(jet_t->get_eta() > etabins[y]){
	    etabin = y;
	  }
	}
	
	double jer = jet->get_pt()/jet_t->get_pt();

	reco_noHcalIn_genpTratio_R04.push_back(jer);
	delRreco_noHcalIn_truth_R04.push_back(delR);
	
	hJER_noHcalIn_allpT_allEta_R04->Fill(jer);
	
	hReco_NoHcalIn_GenJetdelR_R04->Fill(delR);
	
	if(etabin != -1 && ptbin != -1)      
	  hJER_noHcalIn_R04[ptbin][etabin]->Fill(jer);
	
      }else {
	
	truthjetid_R04.push_back(-999);
	truthjetp_reco_noHcalIn_R04.push_back(-999);
	truthjetphi_reco_noHcalIn_R04.push_back(-999);
	truthjeteta_reco_noHcalIn_R04.push_back(-999);
	truthjetpt_reco_noHcalIn_R04.push_back(-999);
	truthjetenergy_reco_noHcalIn_R04.push_back(-999);
	truthjetpx_reco_noHcalIn_R04.push_back(-999);
	truthjetpy_reco_noHcalIn_R04.push_back(-999);
	truthjetpz_reco_noHcalIn_R04.push_back(-999);
	delRreco_noHcalIn_truth_R04.push_back(-999);
	reco_noHcalIn_genpTratio_R04.push_back(-999);
	reco_noHcalIn_jetsubid_R04.push_back(-999);
	reco_noHcalIn_parton_flavor_R04.push_back(-999);
	
      }
    
  
    }
      
  }

  //! Fill all trees
  nevents++;
  truth_g4particles->Fill();
  truthjettree_R02->Fill();
  truthjettree_R03->Fill();
  truthjettree_R04->Fill();
  recojettree_R02->Fill();
  reco_noHcalIn_jettree_R02->Fill();      
  recojettree_R03->Fill();
  reco_noHcalIn_jettree_R03->Fill();      
  recojettree_R04->Fill();
  reco_noHcalIn_jettree_R04->Fill();      
  tree->Fill();

  return 0;

}




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


//! tag jet flavor by parton matching, like PRL 113, 132301 (2014)
float JetPerformance::parton_frag(Jet * this_jet,
				  HepMC::GenEvent* theEvent,
				  const double match_radius = 0.4)
{
  float this_pt = this_jet->get_pt();
  float this_phi = this_jet->get_phi();
  float this_eta = this_jet->get_eta();

  // int jet_flavor = -1;
  float jet_parton_zt = 0;

  //std::cout << " truth jet #" << ijet_t << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << ", checking flavor" << std::endl;

  //TODO: lack taggign scheme of gluon splitting -> QQ_bar
  for (HepMC::GenEvent::particle_const_iterator p = theEvent->particles_begin(); p != theEvent->particles_end(); ++p) {

    int particleID = abs((*p)->pdg_id());
    
    //! check to see if its quark or gluon
    if(particleID < 6 || particleID == 21){
    
      float dR = deltaR((*p)->momentum().pseudoRapidity(), this_eta,
			(*p)->momentum().phi(), this_phi);

      if (dR < match_radius){
	// jet_flavor = particleID;
	
	// int pidabs = abs((*p)->pdg_id());
	float zt = (*p)->momentum().perp() / this_pt;
	jet_parton_zt = zt;
      }
    }
  }
  
  return jet_parton_zt;
}

void JetPerformance::clearallvectors()
{

  //all truth particles
  truthpx.clear();
  truthpy.clear();
  truthpz.clear();
  truthp.clear();
  truthphi.clear();
  trutheta.clear();
  truthpt.clear();
  truthenergy.clear();
  truthpid.clear();  

  //all truth jets
  truthjetpt_R02.clear();
  truthjetpx_R02.clear();
  truthjetpy_R02.clear();
  truthjetpz_R02.clear();
  truthjetphi_R02.clear();
  truthjeteta_R02.clear();
  truthjetmass_R02.clear();
  truthjetp_R02.clear();
  truthjetenergy_R02.clear();

  //all reco jets
  recojetpt_R02.clear();
  recojetpx_R02.clear();
  recojetpy_R02.clear();
  recojetpz_R02.clear();
  recojetphi_R02.clear();
  recojeteta_R02.clear();
  recojetmass_R02.clear();
  recojetp_R02.clear();
  recojetenergy_R02.clear();
  recojetid_R02.clear();
  truthjetid_R02.clear();
  truthjetpt_reco_R02.clear();
  truthjetpx_reco_R02.clear();
  truthjetpy_reco_R02.clear();
  truthjetpz_reco_R02.clear();
  truthjetphi_reco_R02.clear();
  truthjeteta_reco_R02.clear();
  truthjetmass_reco_R02.clear();
  truthjetp_reco_R02.clear();
  truthjetenergy_reco_R02.clear();
  recogenpTratio_R02.clear();
  delRrecotruth_R02.clear();
  parton_flavor_R02.clear();
  parton_frag_zt_R02.clear();
  recojetsubid_R02.clear();

  //all reco_noHcalIn_ jets
  reco_noHcalIn_jetpt_R02.clear();
  reco_noHcalIn_jetpx_R02.clear();
  reco_noHcalIn_jetpy_R02.clear();
  reco_noHcalIn_jetpz_R02.clear();
  reco_noHcalIn_jetphi_R02.clear();
  reco_noHcalIn_jeteta_R02.clear();
  reco_noHcalIn_jetmass_R02.clear();
  reco_noHcalIn_jetp_R02.clear();
  reco_noHcalIn_jetenergy_R02.clear();
  reco_noHcalIn_jetid_R02.clear();
  reco_noHcalIn_truthjetid_R02.clear();
  truthjetpt_reco_noHcalIn_R02.clear();
  truthjetpx_reco_noHcalIn_R02.clear();
  truthjetpy_reco_noHcalIn_R02.clear();
  truthjetpz_reco_noHcalIn_R02.clear();
  truthjetphi_reco_noHcalIn_R02.clear();
  truthjeteta_reco_noHcalIn_R02.clear();
  truthjetmass_reco_noHcalIn_R02.clear();
  truthjetp_reco_noHcalIn_R02.clear();
  truthjetenergy_reco_noHcalIn_R02.clear();
  reco_noHcalIn_genpTratio_R02.clear();
  delRreco_noHcalIn_truth_R02.clear();
  reco_noHcalIn_parton_flavor_R02.clear();
  reco_noHcalIn_parton_frag_zt_R02.clear();
  reco_noHcalIn_jetsubid_R02.clear();
  

  //all truth jets
  truthjetpt_R03.clear();
  truthjetpx_R03.clear();
  truthjetpy_R03.clear();
  truthjetpz_R03.clear();
  truthjetphi_R03.clear();
  truthjeteta_R03.clear();
  truthjetmass_R03.clear();
  truthjetp_R03.clear();
  truthjetenergy_R03.clear();

  //all reco jets
  recojetpt_R03.clear();
  recojetpx_R03.clear();
  recojetpy_R03.clear();
  recojetpz_R03.clear();
  recojetphi_R03.clear();
  recojeteta_R03.clear();
  recojetmass_R03.clear();
  recojetp_R03.clear();
  recojetenergy_R03.clear();
  recojetid_R03.clear();
  truthjetid_R03.clear();
  truthjetpt_reco_R03.clear();
  truthjetpx_reco_R03.clear();
  truthjetpy_reco_R03.clear();
  truthjetpz_reco_R03.clear();
  truthjetphi_reco_R03.clear();
  truthjeteta_reco_R03.clear();
  truthjetmass_reco_R03.clear();
  truthjetp_reco_R03.clear();
  truthjetenergy_reco_R03.clear();
  recogenpTratio_R03.clear();
  delRrecotruth_R03.clear();
  parton_flavor_R03.clear();
  parton_frag_zt_R03.clear();
  recojetsubid_R03.clear();

  //all reco_noHcalIn_ jets
  reco_noHcalIn_jetpt_R03.clear();
  reco_noHcalIn_jetpx_R03.clear();
  reco_noHcalIn_jetpy_R03.clear();
  reco_noHcalIn_jetpz_R03.clear();
  reco_noHcalIn_jetphi_R03.clear();
  reco_noHcalIn_jeteta_R03.clear();
  reco_noHcalIn_jetmass_R03.clear();
  reco_noHcalIn_jetp_R03.clear();
  reco_noHcalIn_jetenergy_R03.clear();
  reco_noHcalIn_jetid_R03.clear();
  reco_noHcalIn_truthjetid_R03.clear();
  truthjetpt_reco_noHcalIn_R03.clear();
  truthjetpx_reco_noHcalIn_R03.clear();
  truthjetpy_reco_noHcalIn_R03.clear();
  truthjetpz_reco_noHcalIn_R03.clear();
  truthjetphi_reco_noHcalIn_R03.clear();
  truthjeteta_reco_noHcalIn_R03.clear();
  truthjetmass_reco_noHcalIn_R03.clear();
  truthjetp_reco_noHcalIn_R03.clear();
  truthjetenergy_reco_noHcalIn_R03.clear();
  reco_noHcalIn_genpTratio_R03.clear();
  delRreco_noHcalIn_truth_R03.clear();
  reco_noHcalIn_parton_flavor_R03.clear();
  reco_noHcalIn_parton_frag_zt_R03.clear();
  reco_noHcalIn_jetsubid_R03.clear();


  //all truth jets
  truthjetpt_R04.clear();
  truthjetpx_R04.clear();
  truthjetpy_R04.clear();
  truthjetpz_R04.clear();
  truthjetphi_R04.clear();
  truthjeteta_R04.clear();
  truthjetmass_R04.clear();
  truthjetp_R04.clear();
  truthjetenergy_R04.clear();

  //all reco jets
  recojetpt_R04.clear();
  recojetpx_R04.clear();
  recojetpy_R04.clear();
  recojetpz_R04.clear();
  recojetphi_R04.clear();
  recojeteta_R04.clear();
  recojetmass_R04.clear();
  recojetp_R04.clear();
  recojetenergy_R04.clear();
  recojetid_R04.clear();
  truthjetid_R04.clear();
  truthjetpt_reco_R04.clear();
  truthjetpx_reco_R04.clear();
  truthjetpy_reco_R04.clear();
  truthjetpz_reco_R04.clear();
  truthjetphi_reco_R04.clear();
  truthjeteta_reco_R04.clear();
  truthjetmass_reco_R04.clear();
  truthjetp_reco_R04.clear();
  truthjetenergy_reco_R04.clear();
  recogenpTratio_R04.clear();
  delRrecotruth_R04.clear();
  parton_flavor_R04.clear();
  parton_frag_zt_R04.clear();
  recojetsubid_R04.clear();

  //all reco_noHcalIn_ jets
  reco_noHcalIn_jetpt_R04.clear();
  reco_noHcalIn_jetpx_R04.clear();
  reco_noHcalIn_jetpy_R04.clear();
  reco_noHcalIn_jetpz_R04.clear();
  reco_noHcalIn_jetphi_R04.clear();
  reco_noHcalIn_jeteta_R04.clear();
  reco_noHcalIn_jetmass_R04.clear();
  reco_noHcalIn_jetp_R04.clear();
  reco_noHcalIn_jetenergy_R04.clear();
  reco_noHcalIn_jetid_R04.clear();
  reco_noHcalIn_truthjetid_R04.clear();
  truthjetpt_reco_noHcalIn_R04.clear();
  truthjetpx_reco_noHcalIn_R04.clear();
  truthjetpy_reco_noHcalIn_R04.clear();
  truthjetpz_reco_noHcalIn_R04.clear();
  truthjetphi_reco_noHcalIn_R04.clear();
  truthjeteta_reco_noHcalIn_R04.clear();
  truthjetmass_reco_noHcalIn_R04.clear();
  truthjetp_reco_noHcalIn_R04.clear();
  truthjetenergy_reco_noHcalIn_R04.clear();
  reco_noHcalIn_genpTratio_R04.clear();
  delRreco_noHcalIn_truth_R04.clear();
  reco_noHcalIn_parton_flavor_R04.clear();
  reco_noHcalIn_parton_frag_zt_R04.clear();
  reco_noHcalIn_jetsubid_R04.clear();
    

}



int JetPerformance::End(PHCompositeNode *topnode)
{
  std::cout<<" DONE PROCESSING "<<std::endl;
  
  
  file->Write();
  file->Close();
  return 0;
}



void JetPerformance::Set_Tree_Branches()
{
  
  truth_g4particles = new TTree("truthtree","a tree with all truth particles");
  truth_g4particles->Branch("truthpx",&truthpx);
  truth_g4particles->Branch("truthpy",&truthpy);
  truth_g4particles->Branch("truthpz",&truthpz);
  truth_g4particles->Branch("truthp",&truthp);
  truth_g4particles->Branch("truthenergy",&truthenergy);
  truth_g4particles->Branch("truthphi",&truthphi);
  truth_g4particles->Branch("trutheta",&trutheta);
  truth_g4particles->Branch("truthpt",&truthpt);
  truth_g4particles->Branch("truthpid",&truthpid);


  truthjettree_R02 = new TTree("truthjettree_R02","a tree_R02 with truth jets");
  truthjettree_R02->Branch("truthjetpt_R02",&truthjetpt_R02);
  truthjettree_R02->Branch("truthjetpx_R02",&truthjetpx_R02);
  truthjettree_R02->Branch("truthjetpy_R02",&truthjetpy_R02);
  truthjettree_R02->Branch("truthjetpz_R02",&truthjetpz_R02);
  truthjettree_R02->Branch("truthjetphi_R02",&truthjetphi_R02);
  truthjettree_R02->Branch("truthjeteta_R02",&truthjeteta_R02);
  truthjettree_R02->Branch("truthjetmass_R02",&truthjetmass_R02);
  truthjettree_R02->Branch("truthjetp_R02",&truthjetp_R02);
  truthjettree_R02->Branch("truthjetenergy_R02",&truthjetenergy_R02);


  recojettree_R02 = new TTree("recojettree_R02","a tree_R02 with reco jets");
  recojettree_R02->Branch("recojetpt_R02",&recojetpt_R02);
  recojettree_R02->Branch("recojetpx_R02",&recojetpx_R02);
  recojettree_R02->Branch("recojetpy_R02",&recojetpy_R02);
  recojettree_R02->Branch("recojetpz_R02",&recojetpz_R02);
  recojettree_R02->Branch("recojetphi_R02",&recojetphi_R02);
  recojettree_R02->Branch("recojeteta_R02",&recojeteta_R02);
  recojettree_R02->Branch("recojetmass_R02",&recojetmass_R02);
  recojettree_R02->Branch("recojetp_R02",&recojetp_R02);
  recojettree_R02->Branch("recojetenergy_R02",&recojetenergy_R02);
  recojettree_R02->Branch("recojetid_R02",&recojetid_R02);
  recojettree_R02->Branch("truthjetid_R02",&truthjetid_R02);
  recojettree_R02->Branch("truthjetp_reco_R02",&truthjetp_R02);
  recojettree_R02->Branch("truthjetphi_reco_R02",&truthjetphi_reco_R02);
  recojettree_R02->Branch("truthjeteta_reco_R02",&truthjeteta_reco_R02);
  recojettree_R02->Branch("truthjetpt_reco_R02",&truthjetpt_reco_R02);
  recojettree_R02->Branch("truthjetenergy_reco_R02",&truthjetenergy_reco_R02);
  recojettree_R02->Branch("truthjetpx_reco_R02",&truthjetpx_reco_R02);
  recojettree_R02->Branch("truthjetpy_reco_R02",&truthjetpy_reco_R02);
  recojettree_R02->Branch("truthjetpz_reco_R02",&truthjetpz_reco_R02);
  recojettree_R02->Branch("recogenpTratio_R02",&recogenpTratio_R02);
  recojettree_R02->Branch("delRrecotruth_R02",&delRrecotruth_R02);
  recojettree_R02->Branch("recojetsubid_R02",&recojetsubid_R02);
  recojettree_R02->Branch("parton_flavor_R02",&parton_flavor_R02);
  recojettree_R02->Branch("parton_frag_zt_R02",&parton_frag_zt_R02);

  reco_noHcalIn_jettree_R02 = new TTree("reco_noHcalIn_jettree_R02","a tree_R02 with reco_noHcalIn_ jets");
  reco_noHcalIn_jettree_R02->Branch("reco_noHcalIn_jetpt_R02",&reco_noHcalIn_jetpt_R02);
  reco_noHcalIn_jettree_R02->Branch("reco_noHcalIn_jetpx_R02",&reco_noHcalIn_jetpx_R02);
  reco_noHcalIn_jettree_R02->Branch("reco_noHcalIn_jetpy_R02",&reco_noHcalIn_jetpy_R02);
  reco_noHcalIn_jettree_R02->Branch("reco_noHcalIn_jetpz_R02",&reco_noHcalIn_jetpz_R02);
  reco_noHcalIn_jettree_R02->Branch("reco_noHcalIn_jetphi_R02",&reco_noHcalIn_jetphi_R02);
  reco_noHcalIn_jettree_R02->Branch("reco_noHcalIn_jeteta_R02",&reco_noHcalIn_jeteta_R02);
  reco_noHcalIn_jettree_R02->Branch("reco_noHcalIn_jetmass_R02",&reco_noHcalIn_jetmass_R02);
  reco_noHcalIn_jettree_R02->Branch("reco_noHcalIn_jetp_R02",&reco_noHcalIn_jetp_R02);
  reco_noHcalIn_jettree_R02->Branch("reco_noHcalIn_jetenergy_R02",&reco_noHcalIn_jetenergy_R02);
  reco_noHcalIn_jettree_R02->Branch("reco_noHcalIn_jetid_R02",&reco_noHcalIn_jetid_R02);
  reco_noHcalIn_jettree_R02->Branch("truthjetid_R02",&truthjetid_R02,"truthjetid_R02/F");
  reco_noHcalIn_jettree_R02->Branch("truthjetp_reco_noHcalIn_R02",&truthjetp_R02);
  reco_noHcalIn_jettree_R02->Branch("truthjetphi_reco_noHcalIn_R02",&truthjetphi_reco_noHcalIn_R02);
  reco_noHcalIn_jettree_R02->Branch("truthjeteta_reco_noHcalIn_R02",&truthjeteta_reco_noHcalIn_R02);
  reco_noHcalIn_jettree_R02->Branch("truthjetpt_reco_noHcalIn_R02",&truthjetpt_reco_noHcalIn_R02);
  reco_noHcalIn_jettree_R02->Branch("truthjetenergy_reco_noHcalIn_R02",&truthjetenergy_reco_noHcalIn_R02);
  reco_noHcalIn_jettree_R02->Branch("truthjetpx_reco_noHcalIn_R02",&truthjetpx_reco_noHcalIn_R02);
  reco_noHcalIn_jettree_R02->Branch("truthjetpy_reco_noHcalIn_R02",&truthjetpy_reco_noHcalIn_R02);
  reco_noHcalIn_jettree_R02->Branch("truthjetpz_reco_noHcalIn_R02",&truthjetpz_reco_noHcalIn_R02);
  reco_noHcalIn_jettree_R02->Branch("reco_noHcalIn_genpTratio_R02",&reco_noHcalIn_genpTratio_R02);
  reco_noHcalIn_jettree_R02->Branch("delRreco_noHcalIn_truth_R02",&delRreco_noHcalIn_truth_R02);
  reco_noHcalIn_jettree_R02->Branch("reco_noHcalIn_jetsubid_R02",&reco_noHcalIn_jetsubid_R02);
  reco_noHcalIn_jettree_R02->Branch("reco_noHcalIn_parton_flavor_R02",&reco_noHcalIn_parton_flavor_R02);
  reco_noHcalIn_jettree_R02->Branch("reco_noHcalIn_parton_frag_zt_R02",&reco_noHcalIn_parton_frag_zt_R02);


  //! R = 0.3

  truthjettree_R03 = new TTree("truthjettree_R03","a tree_R03 with truth jets");
  truthjettree_R03->Branch("truthjetpt_R03",&truthjetpt_R03);
  truthjettree_R03->Branch("truthjetpx_R03",&truthjetpx_R03);
  truthjettree_R03->Branch("truthjetpy_R03",&truthjetpy_R03);
  truthjettree_R03->Branch("truthjetpz_R03",&truthjetpz_R03);
  truthjettree_R03->Branch("truthjetphi_R03",&truthjetphi_R03);
  truthjettree_R03->Branch("truthjeteta_R03",&truthjeteta_R03);
  truthjettree_R03->Branch("truthjetmass_R03",&truthjetmass_R03);
  truthjettree_R03->Branch("truthjetp_R03",&truthjetp_R03);
  truthjettree_R03->Branch("truthjetenergy_R03",&truthjetenergy_R03);


  recojettree_R03 = new TTree("recojettree_R03","a tree_R03 with reco jets");
  recojettree_R03->Branch("recojetpt_R03",&recojetpt_R03);
  recojettree_R03->Branch("recojetpx_R03",&recojetpx_R03);
  recojettree_R03->Branch("recojetpy_R03",&recojetpy_R03);
  recojettree_R03->Branch("recojetpz_R03",&recojetpz_R03);
  recojettree_R03->Branch("recojetphi_R03",&recojetphi_R03);
  recojettree_R03->Branch("recojeteta_R03",&recojeteta_R03);
  recojettree_R03->Branch("recojetmass_R03",&recojetmass_R03);
  recojettree_R03->Branch("recojetp_R03",&recojetp_R03);
  recojettree_R03->Branch("recojetenergy_R03",&recojetenergy_R03);
  recojettree_R03->Branch("recojetid_R03",&recojetid_R03);
  recojettree_R03->Branch("truthjetid_R03",&truthjetid_R03);
  recojettree_R03->Branch("truthjetp_reco_R03",&truthjetp_R03);
  recojettree_R03->Branch("truthjetphi_reco_R03",&truthjetphi_reco_R03);
  recojettree_R03->Branch("truthjeteta_reco_R03",&truthjeteta_reco_R03);
  recojettree_R03->Branch("truthjetpt_reco_R03",&truthjetpt_reco_R03);
  recojettree_R03->Branch("truthjetenergy_reco_R03",&truthjetenergy_reco_R03);
  recojettree_R03->Branch("truthjetpx_reco_R03",&truthjetpx_reco_R03);
  recojettree_R03->Branch("truthjetpy_reco_R03",&truthjetpy_reco_R03);
  recojettree_R03->Branch("truthjetpz_reco_R03",&truthjetpz_reco_R03);
  recojettree_R03->Branch("recogenpTratio_R03",&recogenpTratio_R03);
  recojettree_R03->Branch("delRrecotruth_R03",&delRrecotruth_R03);
  recojettree_R03->Branch("recojetsubid_R03",&recojetsubid_R03);
  recojettree_R03->Branch("parton_flavor_R03",&parton_flavor_R03);
  recojettree_R03->Branch("parton_frag_zt_R03",&parton_frag_zt_R03);

  reco_noHcalIn_jettree_R03 = new TTree("reco_noHcalIn_jettree_R03","a tree_R03 with reco_noHcalIn_ jets");
  reco_noHcalIn_jettree_R03->Branch("reco_noHcalIn_jetpt_R03",&reco_noHcalIn_jetpt_R03);
  reco_noHcalIn_jettree_R03->Branch("reco_noHcalIn_jetpx_R03",&reco_noHcalIn_jetpx_R03);
  reco_noHcalIn_jettree_R03->Branch("reco_noHcalIn_jetpy_R03",&reco_noHcalIn_jetpy_R03);
  reco_noHcalIn_jettree_R03->Branch("reco_noHcalIn_jetpz_R03",&reco_noHcalIn_jetpz_R03);
  reco_noHcalIn_jettree_R03->Branch("reco_noHcalIn_jetphi_R03",&reco_noHcalIn_jetphi_R03);
  reco_noHcalIn_jettree_R03->Branch("reco_noHcalIn_jeteta_R03",&reco_noHcalIn_jeteta_R03);
  reco_noHcalIn_jettree_R03->Branch("reco_noHcalIn_jetmass_R03",&reco_noHcalIn_jetmass_R03);
  reco_noHcalIn_jettree_R03->Branch("reco_noHcalIn_jetp_R03",&reco_noHcalIn_jetp_R03);
  reco_noHcalIn_jettree_R03->Branch("reco_noHcalIn_jetenergy_R03",&reco_noHcalIn_jetenergy_R03);
  reco_noHcalIn_jettree_R03->Branch("reco_noHcalIn_jetid_R03",&reco_noHcalIn_jetid_R03);
  reco_noHcalIn_jettree_R03->Branch("truthjetid_R03",&truthjetid_R03,"truthjetid_R03/F");
  reco_noHcalIn_jettree_R03->Branch("truthjetp_reco_noHcalIn_R03",&truthjetp_R03);
  reco_noHcalIn_jettree_R03->Branch("truthjetphi_reco_noHcalIn_R03",&truthjetphi_reco_noHcalIn_R03);
  reco_noHcalIn_jettree_R03->Branch("truthjeteta_reco_noHcalIn_R03",&truthjeteta_reco_noHcalIn_R03);
  reco_noHcalIn_jettree_R03->Branch("truthjetpt_reco_noHcalIn_R03",&truthjetpt_reco_noHcalIn_R03);
  reco_noHcalIn_jettree_R03->Branch("truthjetenergy_reco_noHcalIn_R03",&truthjetenergy_reco_noHcalIn_R03);
  reco_noHcalIn_jettree_R03->Branch("truthjetpx_reco_noHcalIn_R03",&truthjetpx_reco_noHcalIn_R03);
  reco_noHcalIn_jettree_R03->Branch("truthjetpy_reco_noHcalIn_R03",&truthjetpy_reco_noHcalIn_R03);
  reco_noHcalIn_jettree_R03->Branch("truthjetpz_reco_noHcalIn_R03",&truthjetpz_reco_noHcalIn_R03);
  reco_noHcalIn_jettree_R03->Branch("reco_noHcalIn_genpTratio_R03",&reco_noHcalIn_genpTratio_R03);
  reco_noHcalIn_jettree_R03->Branch("delRreco_noHcalIn_truth_R03",&delRreco_noHcalIn_truth_R03);
  reco_noHcalIn_jettree_R03->Branch("reco_noHcalIn_jetsubid_R03",&reco_noHcalIn_jetsubid_R03);
  reco_noHcalIn_jettree_R03->Branch("reco_noHcalIn_parton_flavor_R03",&reco_noHcalIn_parton_flavor_R03);
  reco_noHcalIn_jettree_R03->Branch("reco_noHcalIn_parton_frag_zt_R03",&reco_noHcalIn_parton_frag_zt_R03);



  //! R = 0.4

  truthjettree_R04 = new TTree("truthjettree_R04","a tree_R04 with truth jets");
  truthjettree_R04->Branch("truthjetpt_R04",&truthjetpt_R04);
  truthjettree_R04->Branch("truthjetpx_R04",&truthjetpx_R04);
  truthjettree_R04->Branch("truthjetpy_R04",&truthjetpy_R04);
  truthjettree_R04->Branch("truthjetpz_R04",&truthjetpz_R04);
  truthjettree_R04->Branch("truthjetphi_R04",&truthjetphi_R04);
  truthjettree_R04->Branch("truthjeteta_R04",&truthjeteta_R04);
  truthjettree_R04->Branch("truthjetmass_R04",&truthjetmass_R04);
  truthjettree_R04->Branch("truthjetp_R04",&truthjetp_R04);
  truthjettree_R04->Branch("truthjetenergy_R04",&truthjetenergy_R04);


  recojettree_R04 = new TTree("recojettree_R04","a tree_R04 with reco jets");
  recojettree_R04->Branch("recojetpt_R04",&recojetpt_R04);
  recojettree_R04->Branch("recojetpx_R04",&recojetpx_R04);
  recojettree_R04->Branch("recojetpy_R04",&recojetpy_R04);
  recojettree_R04->Branch("recojetpz_R04",&recojetpz_R04);
  recojettree_R04->Branch("recojetphi_R04",&recojetphi_R04);
  recojettree_R04->Branch("recojeteta_R04",&recojeteta_R04);
  recojettree_R04->Branch("recojetmass_R04",&recojetmass_R04);
  recojettree_R04->Branch("recojetp_R04",&recojetp_R04);
  recojettree_R04->Branch("recojetenergy_R04",&recojetenergy_R04);
  recojettree_R04->Branch("recojetid_R04",&recojetid_R04);
  recojettree_R04->Branch("truthjetid_R04",&truthjetid_R04);
  recojettree_R04->Branch("truthjetp_reco_R04",&truthjetp_R04);
  recojettree_R04->Branch("truthjetphi_reco_R04",&truthjetphi_reco_R04);
  recojettree_R04->Branch("truthjeteta_reco_R04",&truthjeteta_reco_R04);
  recojettree_R04->Branch("truthjetpt_reco_R04",&truthjetpt_reco_R04);
  recojettree_R04->Branch("truthjetenergy_reco_R04",&truthjetenergy_reco_R04);
  recojettree_R04->Branch("truthjetpx_reco_R04",&truthjetpx_reco_R04);
  recojettree_R04->Branch("truthjetpy_reco_R04",&truthjetpy_reco_R04);
  recojettree_R04->Branch("truthjetpz_reco_R04",&truthjetpz_reco_R04);
  recojettree_R04->Branch("recogenpTratio_R04",&recogenpTratio_R04);
  recojettree_R04->Branch("delRrecotruth_R04",&delRrecotruth_R04);
  recojettree_R04->Branch("recojetsubid_R04",&recojetsubid_R04);
  recojettree_R04->Branch("parton_flavor_R04",&parton_flavor_R04);
  recojettree_R04->Branch("parton_frag_zt_R04",&parton_frag_zt_R04);

  reco_noHcalIn_jettree_R04 = new TTree("reco_noHcalIn_jettree_R04","a tree_R04 with reco_noHcalIn_ jets");
  reco_noHcalIn_jettree_R04->Branch("reco_noHcalIn_jetpt_R04",&reco_noHcalIn_jetpt_R04);
  reco_noHcalIn_jettree_R04->Branch("reco_noHcalIn_jetpx_R04",&reco_noHcalIn_jetpx_R04);
  reco_noHcalIn_jettree_R04->Branch("reco_noHcalIn_jetpy_R04",&reco_noHcalIn_jetpy_R04);
  reco_noHcalIn_jettree_R04->Branch("reco_noHcalIn_jetpz_R04",&reco_noHcalIn_jetpz_R04);
  reco_noHcalIn_jettree_R04->Branch("reco_noHcalIn_jetphi_R04",&reco_noHcalIn_jetphi_R04);
  reco_noHcalIn_jettree_R04->Branch("reco_noHcalIn_jeteta_R04",&reco_noHcalIn_jeteta_R04);
  reco_noHcalIn_jettree_R04->Branch("reco_noHcalIn_jetmass_R04",&reco_noHcalIn_jetmass_R04);
  reco_noHcalIn_jettree_R04->Branch("reco_noHcalIn_jetp_R04",&reco_noHcalIn_jetp_R04);
  reco_noHcalIn_jettree_R04->Branch("reco_noHcalIn_jetenergy_R04",&reco_noHcalIn_jetenergy_R04);
  reco_noHcalIn_jettree_R04->Branch("reco_noHcalIn_jetid_R04",&reco_noHcalIn_jetid_R04);
  reco_noHcalIn_jettree_R04->Branch("truthjetid_R04",&truthjetid_R04,"truthjetid_R04/F");
  reco_noHcalIn_jettree_R04->Branch("truthjetp_reco_noHcalIn_R04",&truthjetp_R04);
  reco_noHcalIn_jettree_R04->Branch("truthjetphi_reco_noHcalIn_R04",&truthjetphi_reco_noHcalIn_R04);
  reco_noHcalIn_jettree_R04->Branch("truthjeteta_reco_noHcalIn_R04",&truthjeteta_reco_noHcalIn_R04);
  reco_noHcalIn_jettree_R04->Branch("truthjetpt_reco_noHcalIn_R04",&truthjetpt_reco_noHcalIn_R04);
  reco_noHcalIn_jettree_R04->Branch("truthjetenergy_reco_noHcalIn_R04",&truthjetenergy_reco_noHcalIn_R04);
  reco_noHcalIn_jettree_R04->Branch("truthjetpx_reco_noHcalIn_R04",&truthjetpx_reco_noHcalIn_R04);
  reco_noHcalIn_jettree_R04->Branch("truthjetpy_reco_noHcalIn_R04",&truthjetpy_reco_noHcalIn_R04);
  reco_noHcalIn_jettree_R04->Branch("truthjetpz_reco_noHcalIn_R04",&truthjetpz_reco_noHcalIn_R04);
  reco_noHcalIn_jettree_R04->Branch("reco_noHcalIn_genpTratio_R04",&reco_noHcalIn_genpTratio_R04);
  reco_noHcalIn_jettree_R04->Branch("delRreco_noHcalIn_truth_R04",&delRreco_noHcalIn_truth_R04);
  reco_noHcalIn_jettree_R04->Branch("reco_noHcalIn_jetsubid_R04",&reco_noHcalIn_jetsubid_R04);
  reco_noHcalIn_jettree_R04->Branch("reco_noHcalIn_parton_flavor_R04",&reco_noHcalIn_parton_flavor_R04);
  reco_noHcalIn_jettree_R04->Branch("reco_noHcalIn_parton_frag_zt_R04",&reco_noHcalIn_parton_frag_zt_R04);

  
}



