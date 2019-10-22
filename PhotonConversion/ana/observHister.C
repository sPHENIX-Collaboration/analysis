#include <iostream>
using namespace std;

#include "TFile.h"
#include "Scalar.h"
#include "TTree.h"
#include "TChain.h"
#include "TLegend.h"
#include "math.h"
#include "TH1.h"
#include "TH2.h"
#include "TEfficiency.h"
#include "TLine.h"
#include "TGraphAsymmErrors.h"

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

void matchPlot(TChain* ttree,TFile* out_file){
  int b_match;
  int b_unmatch;
  ttree->SetBranchAddress("nMatched",     & b_match   );
  ttree->SetBranchAddress("nUnmatched",     &b_unmatch   );
  unsigned totalmatch=0;
  unsigned totaltrack=0; 
  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);

    totalmatch+=b_match;
    totaltrack+=b_match;
    totaltrack+=b_unmatch;
  }
  cout<<"Match "<<totalmatch<<" tracks\n";
  cout<<"Matched "<<(float)totalmatch/totaltrack*100.<<"\% of tracks"<<endl;  
}

void makeVtxR(TChain* ttree,TFile* out_file){
  float vtxr;
  float tvtxr;
  ttree->SetBranchAddress("vtx_radius",&vtxr);
  ttree->SetBranchAddress("tvtx_radius",&tvtxr);

  std::vector<TH1F*> plots;
  plots.push_back(new TH1F("vtx_reco","",40,0,30));
  plots.push_back(new TH1F("vtx_truth","",40,0,30));
  
  plots[0]->Sumw2();
  plots[1]->Sumw2();

  double calc=0;
  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    plots[0]->Fill(vtxr);
    plots[1]->Fill(tvtxr);
    calc+=TMath::Abs(vtxr-tvtxr);
  }
  calc/=ttree->GetEntries();
  for (int i = 0; i < 2; ++i)
  {
    plots[i]->Scale(1./ttree->GetEntries(),"width");
  }
  out_file->Write();
  std::cout<<"mean deviation="<<calc<<std::endl;
}

void makepTEff(TChain* ttree,TFile* out_file){
  float pT;
  float tpT;
  float track_pT;
  ttree->SetBranchAddress("photon_pT",&pT);
  ttree->SetBranchAddress("tphoton_pT",&tpT);
  ttree->SetBranchAddress("track_pT",&track_pT);
  
  TH1F *pTeffPlot = new TH1F("#frac{#Delta#it{p}^{T}}{#it{p}_{#it{truth}}^{T}}","",40,-2,2);
  TH2F *pTefffuncPlot = new TH2F("pT_resolution_to_truthpt","",40,1,35,40,-1.5,1.5);
  TH1F *trackpTDist = new TH1F("truthpt","",40,0,35);
  pTeffPlot->Sumw2();
  pTefffuncPlot->Sumw2();
  trackpTDist->Sumw2();
  unsigned lowpTCount=0;

  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    pTeffPlot->Fill((pT-tpT)/tpT);
    pTefffuncPlot->Fill(tpT,(pT-tpT)/tpT);
    trackpTDist->Fill(track_pT); 
    if(tpT<.6)lowpTCount++;
  }
  pTeffPlot->Scale(1./ttree->GetEntries(),"width");
  pTefffuncPlot->Scale(1./ttree->GetEntries(),"width");
  trackpTDist->Scale(1./ttree->GetEntries(),"width");
  cout<<"Signal rejection through pT cut= "<<(float)lowpTCount/ttree->GetEntries()<<endl;
  cout<<"Signal rejection through pT cut= "<<sqrt((float)lowpTCount)/ttree->GetEntries()<<endl;
  out_file->Write();
}
void testCuts(TChain* ttree,TFile* out_file){
  float dphi;
  float prob;
  float track_pT;
  float deta;
  float radius;
  int layer;
  int dlayer;
  ttree->SetBranchAddress("cluster_dphi",&dphi);
  ttree->SetBranchAddress("cluster_prob",&prob);
  ttree->SetBranchAddress("track_layer",&layer);
  ttree->SetBranchAddress("track_dlayer",&dlayer);
  ttree->SetBranchAddress("track_pT",&track_pT);
  ttree->SetBranchAddress("track_deta",&deta);
  ttree->SetBranchAddress("vtx_radius",&radius);
  
  TH1F *layerDist = new TH1F("layer","",16,-.5,15.5);
  TH1F *probDist = new TH1F("clust_prob","",30,-.5,1.);
  TH1F *deta_plot = new TH1F("deta","",30,-.001,.01);
  TH1F *dlayer_plot = new TH1F("dlayer","",11,-.5,10.5);
  TH1F *r_plot = new TH1F("signal_vtx_radius_dist","",26,-.5,25.5);
  layerDist->Sumw2();
  probDist->Sumw2();
  deta_plot->Sumw2();
  dlayer_plot->Sumw2();
  r_plot->Sumw2();
  unsigned badLayCount=0;
  unsigned badClusCount=0;
  unsigned bigDetaCount=0;
  unsigned shortRadiusCount=0;

  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    if(layer==0)badLayCount++;
    if(dphi<0&&track_pT>.6){
      badClusCount++;
    }
    if(track_pT>.6&&dphi>=0){
      layerDist->Fill(layer);
      probDist->Fill(prob);
      deta_plot->Fill(deta);
      dlayer_plot->Fill(TMath::Abs(dlayer));
      if(deta>.0082||TMath::Abs(dlayer)>9)bigDetaCount++;
      else{
        r_plot->Fill(radius);
        if(radius<1.84) shortRadiusCount++;
      }
    }

  }
  layerDist->Scale(1./ttree->GetEntries());
  deta_plot->Scale(1./ttree->GetEntries());
  dlayer_plot->Scale(1./ttree->GetEntries());
  r_plot->Scale(1./ttree->GetEntries());
  cout<<"Signal rejection through layer cut= "<<(float)badLayCount/ttree->GetEntries()<<endl;
  cout<<"error= "<<sqrt((float)badLayCount)/ttree->GetEntries()<<endl;
  cout<<"Signal rejection through clus cut= "<<(float)badClusCount/ttree->GetEntries()<<endl;
  cout<<"error= "<<sqrt((float)badClusCount)/ttree->GetEntries()<<endl;
  cout<<"Signal rejection through deta cut= "<<(float)bigDetaCount/ttree->GetEntries()<<endl;
  cout<<"error= "<<sqrt((float)bigDetaCount)/ttree->GetEntries()<<endl;
  cout<<"Signal rejection through radius cut= "<<(float)shortRadiusCount/ttree->GetEntries()<<endl;
  cout<<"error= "<<sqrt((float)shortRadiusCount)/ttree->GetEntries()<<endl;
  out_file->Write();
}
void makeRefitDist(TChain* ttree, TFile *out_file){
  float diffx;
  float diffy;
  float diffz;
  ttree->SetBranchAddress("refitdiffx",&diffx);
  ttree->SetBranchAddress("refitdiffy",&diffy);
  ttree->SetBranchAddress("refitdiffz",&diffz);
  TH1F *diffplotx= new TH1F("x_{0}-x_{refit}","",40,-10,10);
  TH1F *diffploty= new TH1F("y_{0}-y_{refit}","",40,-10,10);
  TH1F *diffplotz= new TH1F("z_{0}-z_{refit}","",40,-10,10);

  diffplotx->Sumw2();
  diffploty->Sumw2();
  diffplotz->Sumw2();

  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    diffplotx->Fill(diffx);
    diffploty->Fill(diffy);
    diffplotz->Fill(diffz);
  }
  
  diffplotx->Scale(1./ttree->GetEntries(),"width");
  diffploty->Scale(1./ttree->GetEntries(),"width");
  diffplotz->Scale(1./ttree->GetEntries(),"width");

  out_file->Write();
}

void observHister()
{
  string treePath = "/sphenix/user/vassalli/minBiasConversion/conversiontruthanaout.root";
  string treeExtension = ".root";
  unsigned int nFiles=100;
  TFile *out_file = new TFile("obsrvplots.root","RECREATE");
  TChain *ttree = new TChain("observTree");
  ttree->Add(treePath.c_str());
  cout<<"Total events= "<<ttree->GetEntries()<<'\n';
  matchPlot(ttree,out_file);
}
