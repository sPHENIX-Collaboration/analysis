#include <iostream>
using namespace std;

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TLegend.h"
#include "math.h"
#include "TH1.h"
#include "TH2.h"
#include "TEfficiency.h"

TChain* handleFile(string name, string extension, string treename, unsigned int filecount){
  TChain *all = new TChain(treename.c_str());
  string temp;
  for (int i = 0; i < filecount; ++i)
  {

    ostringstream s;
    s<<i;
    temp = name+string(s.str())+extension;
    all->Add(temp.c_str());
  }
  return all;
}

int reportBackground(TChain* truthTree,TChain* recoTree,TFile* out_file){
  double signal = truthTree->GetEntries();
  signal*=(1-.012)*(1-.002)*(1-.011);
  int background = recoTree->GetEntries();
  background-=(int) signal;
  cout<<"For "<<truthTree->GetEntries()<<" truth conversions, I found "<<background<<" background events"<<endl;
  return (int) signal;
  /*float pT;
  float tpT;
  ttree->SetBranchAddress("photon_pT",&pT);
  ttree->SetBranchAddress("tphoton_pT",&tpT);
  
  TH1F *pTeffPlot = new TH1F("#frac{#Delta#it{p}^{T}}{#it{p}_{#it{truth}}^{T}}","",40,-2,2);
  TH2F *pTefffuncPlot = new TH2F("pT_resolution_to_truthpt","",40,1,35,40,-1.5,1.5);
  pTeffPlot->Sumw2();

  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    pTeffPlot->Fill((pT-tpT)/tpT);
    pTefffuncPlot->Fill(tpT,(pT-tpT)/tpT);
    }
    pTeffPlot->Scale(1./ttree->GetEntries(),"width");
    pTefffuncPlot->Scale(1./ttree->GetEntries(),"width");
    out_file->Write();*/
}

void reportCuts(TChain* _treeBackground,int signal){
  unsigned totalTracks;
  unsigned passedpTEtaQ;
  unsigned passedCluster;
  unsigned passedPair;
  unsigned passedVtx;
  unsigned passedPhoton;

  int sum_totalTracks=-1*signal;
  int sum_passedpTEtaQ=-1*signal;
  int sum_passedCluster=-1*signal;
  int sum_passedPair=-1*signal;
  int sum_passedVtx=-1*signal;
  int sum_passedPhoton=-1*signal;

  _treeBackground->SetBranchAddress("total",   &totalTracks);
  _treeBackground->SetBranchAddress("tracksPEQ",  &passedpTEtaQ);
  _treeBackground->SetBranchAddress("tracks_clus", &passedCluster);
  _treeBackground->SetBranchAddress("pairs", &passedPair);
  _treeBackground->SetBranchAddress("vtx", 	  &passedVtx);
  _treeBackground->SetBranchAddress("photon", 	  &passedPhoton);

  for(unsigned i=0; i<_treeBackground->GetEntries();i++){
    _treeBackground->GetEntry(i);
    sum_totalTracks+=totalTracks;
    sum_passedpTEtaQ+=passedpTEtaQ;
    sum_passedCluster+=passedCluster;
    sum_passedPair+=passedPair;
    sum_passedVtx+=passedVtx;
    sum_passedPhoton+=passedPhoton;
  }
  cout<<"Did RecoConversionEval with "<<sum_totalTracks<<" total tracks\n\t";
  cout<<1-(float)sum_passedpTEtaQ/sum_totalTracks<<"+/-"<<sqrt((float)sum_passedpTEtaQ)/sum_totalTracks<<" of tracks were rejected by pTEtaQ\n\t";
  cout<<1-(float)sum_passedCluster/sum_passedpTEtaQ<<"+/-"<<sqrt((float)sum_passedCluster)/sum_passedpTEtaQ<<" of remaining tracks were rejected by cluster\n\t";
  cout<<1-(float)sum_passedPair/sum_passedCluster<<"+/-"<<sqrt((float)sum_passedPair)/sum_passedCluster<<" of pairs were rejected by pair cuts\n\t";
  cout<<1-(float)sum_passedVtx/sum_passedPair<<"+/-"<<sqrt((float)sum_passedVtx)/sum_passedPair<<" of vtx were rejected by vtx cuts\n\t";
  cout<<1-(float)sum_passedPhoton/sum_passedVtx<<"+/-"<<sqrt((float)sum_passedPhoton)/sum_passedVtx<<" of photons were rejected by photon cuts\n";
  cout<<sum_passedPhoton<<" background events remain with "<<signal<<" signal events\n";
}

int analyzeSignal(TChain* _signalCutTree,TFile *outFile){
  int _b_track_layer ;
  int _b_track_dlayer ;
  float _b_track_deta ;
  float _b_track_pT;
  float _b_ttrack_pT;
  float _b_vtx_radius ;
  float _b_tvtx_radius ;
  float _b_cluster_prob ;
  float _b_photon_pT ;
  float _b_tphoton_pT ;
  float _b_photon_m ;
  _signalCutTree->SetBranchAddress("track_deta", &_b_track_deta);
  _signalCutTree->SetBranchAddress("track_dlayer",&_b_track_dlayer);
  //    _signalCutTree->SetBranchAddress("track_layer", &_b_track_layer);
  _signalCutTree->SetBranchAddress("track_pT", &_b_track_pT);
  //_signalCutTree->SetBranchAddress("ttrack_pT", &_b_ttrack_pT);
   _signalCutTree->SetBranchAddress("vtx_radius", &_b_vtx_radius);
  //_signalCutTree->SetBranchAddress("tvtx_radius", &_b_tvtx_radius);
  // _signalCutTree->SetBranchAddress("vtx_chi2", &_b_vtx_chi2);
  _signalCutTree->SetBranchAddress("cluster_prob", &_b_cluster_prob);
  _signalCutTree->SetBranchAddress("photon_m", &_b_photon_m);
  _signalCutTree->SetBranchAddress("photon_pT", &_b_photon_pT);
  _signalCutTree->SetBranchAddress("tphoton_pT", &_b_tphoton_pT);
  unsigned pT=0, cluster=0, eta=0, photon=0,rsignal=0;

  TH1F *rate_plot = new TH1F("rateplot","",60,0,15);
  rate_plot->Sumw2();

  for(unsigned i=0; i<_signalCutTree->GetEntries();i++){
    _signalCutTree->GetEntry(i);
    if(_b_track_pT<.6) pT++;
    else if (_b_cluster_prob<0) cluster++;
    else if (_b_track_deta>.0082) eta++;
    else if (TMath::Abs(_b_track_dlayer)<=9&&_b_vtx_radius>1.84){
      if(_b_photon_m<.27||_b_photon_m>8.||_b_photon_pT<.039) photon++;
      else rsignal++;
    }
    rate_plot->Fill(_b_tphoton_pT);
  }
  cout<<"pT cut "<<pT<<" events\n";
  cout<<"cluster cut "<<cluster<<" events\n";
  cout<<"eta cut "<<eta<<" events\n";
  cout<<"photon cut "<<photon<<" events\n";
  rate_plot->Scale(1./6e5);
  outFile->Wrtie();
  return rsignal;
}

void minBiasRecoAna()
{
  string treePath = "/sphenix/user/vassalli/minBiasPythia/conversionembededminBiasanalysis";
  string truthTreePath = "/sphenix/user/vassalli/minBiasPythia/conversionembededminBiasTruthanalysis";
  string embedPath = "/sphenix/user/vassalli/RecoConversionTests/conversionembededonlineanalysis";
  string treeExtension = ".root";
  unsigned int nFiles=100;
  TFile *out_file = new TFile("minBiasplots.root","RECREATE");
  TChain *embed_truth_ttree = handleFile(embedPath,treeExtension,"cutTreeSignal",nFiles);
  TChain *embed_reco_ttree = handleFile(embedPath,treeExtension,"recoBackground",nFiles);
  TChain *cut_tree = new TChain("recoBackground");
  TChain *back_tree = new TChain("recoSignal");
  TChain *truth_ttree = new TChain("cutTreeSignal");
  string filename = "/sphenix/user/vassalli/minBiasConversion/conversionanaout.root";
  back_tree->Add(filename.c_str());
  cut_tree->Add(filename.c_str());
  filename = "/sphenix/user/vassalli/minBiasConversion/conversiontruthanaout.root";
  truth_ttree->Add(filename.c_str());
  reportCuts(cut_tree,analyzeSignal(truth_ttree));
/*  cout<<"///////////////////////////////////////////////////////////\n";
  cout<<"EMBEDED ANALYSIS:\n";
  reportCuts(embed_reco_ttree,analyzeSignal(embed_truth_ttree));
*/
}
