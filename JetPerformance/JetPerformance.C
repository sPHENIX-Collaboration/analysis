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

  hGenJetpT = new TH1F("hGenJetpT","Gen jet pT spectra",nbins_pt, ptbins);
  hGenJeteta = new TH1F("hGenJeteta","Gen jet eta spectra",nbins_eta, etabins);
  hGenJetphi = new TH1F("hGenJetphi","Gen jet phi spectra",30, -1*pi, pi);

  hRecoGenJetdelR = new TH1F("hRecoGenJetdelR", "Delta R between reco jet and gen jet",25, 0, 0.5);

  hJER_allpT_allEta = new TH1F("hJER_allpT_allEta","", 50, 0, 2);

  for(int i = 0; i<nbins_pt; ++i){
    hJES_Mean_jteta[i] = new TH1F(Form("hJES_Mean_jteta_ptbin%d",i), "", nbins_eta, etabins);
    hJES_Sigma_jteta[i] = new TH1F(Form("hJES_Sigma_jteta_ptbin%d",i), "", nbins_eta, etabins);
    for(int j = 0; j<nbins_eta; ++j){
      hJER[i][j] = new TH1F(Form("hJER_ptbin%d_etabin%d",i, j),"",50, 0, 2);
      if(i == 0){
	hJES_Mean_jtpt[j] = new TH1F(Form("hJES_Mean_jtpT_etabin%d",j), "", nbins_pt, ptbins);
	hJES_Sigma_jtpt[j] = new TH1F(Form("hJES_Sigma_jtpT_etabin%d",j), "", nbins_pt, ptbins);
      }
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
  JetEvalStack* _jetevalstack = new JetEvalStack(topnode,"AntiKt_Tower_r04","AntiKt_Truth_r04");
  // PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();

  if(!truth_jets){
    cout<<"no truth jets"<<endl;
    return 0;
  }
  if(!reco_jets){
    cout<<"no reco jets"<<endl;
    return 0;
  }
  if(!_jetevalstack){
    cout<<"no good truth jets"<<endl;
    return 0;
  }


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

    hGenJetpT->Fill(truthjetpt);
    hGenJeteta->Fill(truthjeteta);
    hGenJetphi->Fill(truthjetphi);    

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

    hRecoJetpT->Fill(recojetpt);
    hRecoJeteta->Fill(recojeteta);
    hRecoJetphi->Fill(recojetphi);
    

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

      int ptbin = -1;
      int etabin = -1;

      for(unsigned x = 0; x<nbins_pt; x++){
	if(recojetpt > ptbins[x]){
	  ptbin = x;
	}
      }
      
      for(unsigned y = 0; y<nbins_eta; y++){
	if(recojeteta > etabins[y]){
	  etabin = y;
	}
      }

      double jer = recojetpt/truthjetpt;
      recogenpTratio = jer;
      double delR = deltaR(recojeteta, truthjeteta, recojetphi, truthjetphi);
      delRrecotruth = delR;
      
      hJER_allpT_allEta->Fill(jer);
      if(etabin == -1 || ptbin == -1) continue;      
      
      hRecoGenJetdelR->Fill(delR);

      hJER[ptbin][etabin]->Fill(jer);
      
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


int JetPerformance::End(PHCompositeNode *topnode)
{
  std::cout<<" DONE PROCESSING "<<std::endl;

  std::cout<<" Fitting the JES/JER  "<<std::endl;

  //! get the histogram mean and rms at the moment - later we can get it by fitting histograms and minimizing with minuit

  for(int i = 0; i<nbins_pt; ++i){
    for(int j = 0;j<nbins_eta; ++j){
      double mean = hJER[i][j]->GetMean();
      double sigma = hJER[i][j]->GetRMS();
      hJES_Mean_jtpt[j]->SetBinContent(i, mean);
      hJES_Sigma_jtpt[j]->SetBinContent(i, sigma);
      hJES_Mean_jteta[i]->SetBinContent(j, mean);
      hJES_Sigma_jteta[i]->SetBinContent(j, sigma);	
    }
  }
  
  
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


}



