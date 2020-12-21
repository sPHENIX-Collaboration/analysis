#include <iostream>
#include <fstream>
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

namespace {
  unsigned replot=0;
}

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

void makephotonM(TChain* ttree,TChain* back,TFile* out_file){
  float photon_m;
  float tphoton_m;
  float back_m;
  float back_Vtx;
  float back_pt;
  std::vector<TH1F*> plots;
  ttree->SetBranchAddress("photon_m",     &photon_m   );
  back->SetBranchAddress("photon_m",     &back_m   );
  back->SetBranchAddress("vtx_radius",     &back_Vtx   );
  back->SetBranchAddress("track_pT",     &back_pt   );
  //ttree->SetBranchAddress("rephoton_m",     &rephoton_m   );
  plots.push_back(new TH1F("m^{#gamma}_{reco}","",60,0,.18));
  plots.push_back(new TH1F("m^{bkgd}_{reco}","",60,0,.18));
  //plots.push_back(new TH1F("m^{#gamma}_{recoRefit}","",40,-2,10));

  for (int i = 0; i < plots.size(); ++i)
  {
    plots[i]->Sumw2();
  }
  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    plots[0]->Fill(photon_m);
    // plots[1]->Fill(rephoton_m);
  }
  for (int event = 0; event < back->GetEntries(); ++event)
  {
    back->GetEvent(event);
    if(back_Vtx<21.&&back_pt>2.5)
    plots[1]->Fill(back_m);
    // plots[1]->Fill(rephoton_m);
  }
  cout<<"Background mass in range count= "<<plots[1]->Integral()<<'\n';
  for (int i = 0; i < plots.size(); ++i)
  {
    plots[i]->Scale(1./ttree->GetEntries(),"width");
  }
  out_file->Write();
  ttree->ResetBranchAddresses();
}

void makeVtxR(TChain* ttree, TTree* vtxTree,TFile* out_file){
  float vtxr;
  float vtxX;
  float vtxY;
  float tvtxr;
  ttree->SetBranchAddress("vtx_radius",&vtxr);
  ttree->SetBranchAddress("tvtx_radius",&tvtxr);
  vtxTree->SetBranchAddress("vtx_x",&vtxX);
  vtxTree->SetBranchAddress("vtx_y",&vtxY);

  std::vector<TH1F*> plots;
  plots.push_back(new TH1F("vtx_reco","",30,0,40));
  plots.push_back(new TH1F("vtx_corrected","",30,0,40));
  plots.push_back(new TH1F("vtx_truth","",30,0,40));

  plots[0]->Sumw2();
  plots[1]->Sumw2();
  plots[2]->Sumw2();

  double calc=0;
  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    plots[1]->Fill(vtxr);
    plots[2]->Fill(tvtxr);
    calc+=TMath::Abs(vtxr-tvtxr);
  }
  calc/=ttree->GetEntries();
  for (int i = 0; i < vtxTree->GetEntries(); ++i)
  {
    vtxTree->GetEvent(i);
    plots[0]->Fill(sqrt(vtxX*vtxX+vtxY*vtxY));
  }
  for (int i = 0; i < 3; ++i)
  {
    plots[i]->Scale(1./plots[2]->GetEntries(),"width");
  }
  out_file->Write();
  std::cout<<"mean deviation="<<calc<<std::endl;
}

void makeVtxRes(TChain* ttree,TFile* out_file){
  float r;
  float tr;
  ttree->SetBranchAddress("vtx_radius",&r);
  ttree->SetBranchAddress("tvtx_radius",&tr);
  TH1F *vtxeffPlot = new TH1F("#frac{#Deltar_{vtx}_^{#it{reco}}}{r_{vtx}^{#it{truth}}}","",40,-2,2);
  TH2F *vtxefffuncPlot = new TH2F("vtx_resolution_to_truthvtx","",20,0,21,40,-1.5,1.5);
  vtxeffPlot->Sumw2();
  vtxefffuncPlot->Sumw2();
  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    if(r<0) continue;
    vtxeffPlot->Fill((r-tr)/tr);
    vtxefffuncPlot->Fill(tr,(r-tr)/tr);
  }
  vtxeffPlot->Scale(1./ttree->GetEntries(),"width");
  vtxefffuncPlot->Scale(1./ttree->GetEntries(),"width");
  out_file->Write();
  ttree->ResetBranchAddresses();
}

void makeVtxEff(TChain* ttree,TFile* out_file){
  float r;
  float tr;
  float pT;
  ttree->SetBranchAddress("vtx_radius",&r);
  ttree->SetBranchAddress("tvtx_radius",&tr);
  ttree->SetBranchAddress("track_pT",&pT);
  TEfficiency *vtxEff;
  TH1F *recoR= new TH1F("vtxrecoR","",30,0,40);
  TH1F *truthR= new TH1F("vtxtruthR","",30,0,40);
  recoR->Sumw2();
  truthR->Sumw2();
  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    if(r>0) recoR->Fill(tr);
    truthR->Fill(tr);
  }
  vtxEff = new TEfficiency(*recoR,*truthR);
  vtxEff->SetName("vtxEff");
  vtxEff->Write();
  out_file->Write();
  ttree->ResetBranchAddresses();
}

void makepTRes(TChain* ttree){
  float pT;
  float tpT;
  ttree->SetBranchAddress("photon_pT",&pT);
  ttree->SetBranchAddress("tphoton_pT",&tpT);
  TH1F *pTRes = new TH1F("#frac{#it{p}^{T}_{reco}}{#it{p}_{#it{truth}}^{T}}","",40,0,2.5);
  pTRes->Sumw2();
  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    if(pT>0)pTRes->Fill(pT/tpT);
  }
  pTRes->Scale(1./pTRes->Integral());
  ttree->ResetBranchAddresses();
  pTRes->Write();
}

TEfficiency* makepTDist(TChain* ttree,TTree* allTree,TFile* out_file){
  float pT;
  float tpT;
  float track_pT;
  ttree->SetBranchAddress("photon_pT",&pT);
  ttree->SetBranchAddress("tphoton_pT",&tpT);

  vector<float> *allpT=NULL;
  allTree->SetBranchAddress("photon_pT",&allpT);

  TH1F *pTeffPlot = new TH1F("#frac{#it{p}^{T}}{#it{p}_{#it{truth}}^{T}}","",40,-2,2);
  TH2F *pTefffuncPlot = new TH2F("pT_resolution_to_truthpt","",40,1,35,40,-1.5,1.5);
  TH1F *converted_pTspec = new TH1F("converted_photon_truth_pT","",25,5,30);
  TH1F *all_pTspec = new TH1F("all_photon_truth_pT","",25,5,30);
  //TH1F *trackpTDist = new TH1F("truthpt","",40,0,35);
  pTeffPlot->Sumw2();
  converted_pTspec->Sumw2();
  all_pTspec->Sumw2();
  pTefffuncPlot->Sumw2();
  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    if(pT>0)pTeffPlot->Fill(pT/tpT);
    converted_pTspec->Fill(tpT);
    if(pT>0)pTefffuncPlot->Fill(tpT,pT/tpT);
    //trackpTDist->Fill(track_pT); 
  }
  for (int event = 0; event < allTree->GetEntries(); ++event)
  {
    allTree->GetEvent(event);
    for(auto i : *allpT){
      all_pTspec->Fill(i);
    }
  }
  TEfficiency* uni_rate = new TEfficiency(*converted_pTspec,*all_pTspec);
  pTeffPlot->Scale(1./ttree->GetEntries(),"width");
  pTefffuncPlot->Scale(1./ttree->GetEntries());
  TProfile* resProfile = pTefffuncPlot->ProfileX("func_prof",5,30);
  resProfile->Write();
  //trackpTDist->Scale(1./ttree->GetEntries(),"width");
  out_file->Write();
  ttree->ResetBranchAddresses();
  return uni_rate;
}

void compareDeta(TTree* signalTree, TTree* background){
  float detas,detab;
  signalTree->SetBranchAddress("track_deta",&detas);
  background->SetBranchAddress("track_deta",&detab);
  TH1F *detaS_plot = new TH1F("detaS","",3000,-.001,1);
  TH1F *detaB_plot = new TH1F("detaB","",3000,-.001,1);
  detaS_plot->Sumw2();
  detaB_plot->Sumw2();

  for (int i = 0; i < signalTree->GetEntries(); ++i)
  {
    signalTree->GetEvent(i);
    detaS_plot->Fill(detas);
  }
  for (int i = 0; i < background->GetEntries(); ++i)
  {
    background->GetEvent(i);
    detaB_plot->Fill(detab);
  }
  detaB_plot->Scale(1/detaB_plot->Integral());
  detaS_plot->Scale(1/detaS_plot->Integral());
  detaS_plot->Write();
  detaB_plot->Write();
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
  ttree->ResetBranchAddresses();
}

void makepTCaloGraph(string filename,TFile* outfile){
  ifstream caloFile;
  caloFile.open(filename.c_str());
  double x,y;
  string s;
  vector<double> xData, yData;
  /*if(!(caloFile >>x>>y)){
    cout<<"file error"<<endl;
    if(!caloFile.is_open()) cout<<"file not opened"<<endl;
    }*/
  while(caloFile >>x>>s>>y){
    xData.push_back(x);
    yData.push_back(y);
  }
  double *xArray, *yArray;
  xArray=&xData[0];
  yArray=&yData[0];
  TGraph *pTResCaloGraph = new TGraph(xData.size(),xArray,yArray);
  pTResCaloGraph->SetNameTitle("calopTRes","calopTRes");
  pTResCaloGraph->Sort();
  pTResCaloGraph->Write();
  outfile->Write();
}

TH1F* makePythiaSpec(TChain* ttree,TFile* out_file,string type=""){
  vector<float>* tpT= NULL;
  ttree->SetBranchAddress("photon_pT",&tpT);
  string title;
  if(!type.empty()){
    title=type;
    title+="_photon_truth_pT";
  }  
  else title="photon_truth_pT";
  TH1F *tpTspec = new TH1F(title.c_str(),"",20,5,25);
  tpTspec->Sumw2();
  cout<<"pythia tree with: "<<ttree->GetEntries()<<" entries"<<endl;
  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    for(auto i: *tpT){
      if(i>5)tpTspec->Fill(i);
    }
  }
  out_file->Write();
  ttree->ResetBranchAddresses();
  return tpTspec;
}

TH1F removeFirstBin(TH1F h){
  if(h.GetNbinsX()>1){
    string rname = h.GetName();
    rname+=to_string(replot++);
    TH1F r = TH1F(rname.c_str(),h.GetTitle(),h.GetNbinsX()-1,h.GetBinLowEdge(2),h.GetBinLowEdge(h.GetNbinsX()+1));
    r.Sumw2();
    for(unsigned i=1; i<r.GetNbinsX();++i){
      r.SetBinContent(i,h.GetBinContent(i+1));
      r.SetBinError(i,h.GetBinError(i+1));
    }
    return r;
  }
  else {
    return h;
  }
}

double ADtoP(double ad){
  cout<<"AD:"<<ad<<endl;
  double r=-1;
  if(ad<.2) r= 1-TMath::Exp(-13.436+101.14*ad-223.74*ad*ad);
  else if (ad<.34) r= 1-TMath::Exp(-8.31+42.79*ad-59.938*ad*ad);
  else if (ad<.6) r= 1-TMath::Exp(.917-4.279*ad-1.38*ad*ad);
  else r= 1-TMath::Exp(1.29-5.709*ad+.0186*ad*ad);
  cout<<"P:"<<r<<endl;
  return r;
}

float smartChi2(TH1F hard, TH1F soft){
  hard.Scale(1/hard.Integral());
  soft.Scale(1/soft.Integral());
  return soft.Chi2Test(&hard,"WW CHI2/NDF");
}

void chopHard(TH1F hard,TH1F soft){
  unsigned bins = soft.GetNbinsX();
  while(ADtoP(hard.AndersonDarlingTest(&soft))>.05&&bins>2){
    cout<<"Hard/Soft p="<<ADtoP(hard.AndersonDarlingTest(&soft))<<" n bins="<<hard.GetNbinsX()<<endl;
    cout<<"chi2 p="<<smartChi2(hard,soft)<<" n bins="<<hard.GetNbinsX()<<endl;
    hard=removeFirstBin(hard);
    soft=removeFirstBin(soft);
    bins--;
  }
  TH1F* temp = (TH1F*) hard.Clone("hard_chopped");
  temp->Write();
}

void calculateConversionRate(TEfficiency* rate, TH1F *pythia,TFile* out_file){
  TH1F* conversion_rate = (TH1F*)  pythia->Clone("rate");
  TH1* uni_rate = (TH1F*)rate->GetPassedHistogram()->Clone("uni_rate");
  uni_rate->Divide(rate->GetTotalHistogram());
  conversion_rate->Multiply(uni_rate);
  conversion_rate->Scale(1/pythia->Integral());
  out_file->Write();
}

double hardWeightFactor(TH1F* hard,TH1F* soft, unsigned matchBin, double* error){
  double hardTotal=hard->Integral(matchBin,hard->GetNbinsX());
  double softTotal=soft->Integral(matchBin,soft->GetNbinsX());
  double r= softTotal/hardTotal;
  *error = (softTotal+TMath::Power(softTotal,1./2))/hardTotal;
  cout<<soft->Integral(matchBin,soft->GetNbinsX())<<endl;
  cout<<hard->Integral(matchBin,soft->GetNbinsX())<<endl;
  return r;
}

TH1F* addSpec(TH1F* soft,TH1F* hard,TFile* file){
  TH1F* combined = new TH1F("combinedpythia","",soft->GetNbinsX(),soft->GetBinLowEdge(1),hard->GetBinLowEdge(hard->GetNbinsX()+1));
  unsigned matchBin = 10;//getMatchingBin(hard,soft); //hard coded by eye
  for (int i = 1; i < matchBin; ++i)
  {
    combined->SetBinContent(i,soft->GetBinContent(i));
    combined->SetBinError(i,soft->GetBinError(i));
  }
  double systematic;
  hard->Scale(hardWeightFactor(hard,soft,matchBin,&systematic));
  for (int i = matchBin; i < combined->GetNbinsX()+1; ++i)
  {
    combined->SetBinContent(i,hard->GetBinContent(i));
    combined->SetBinError(i,sqrt(hard->GetBinError(i)*hard->GetBinError(i)+systematic*systematic));
  }
  file->Write();
  return combined;
} 

void photonEff()
{
  TFile *out_file = new TFile("effplots.root","UPDATE");
  //string treePath = "/sphenix/user/vassalli/gammasample/truthconversionembededanalysis";
  string treeExtension = ".root";
  unsigned int nFiles=1000;
  string softPath = "/sphenix/user/vassalli/minBiasPythia/softana.root";
  //string hard0Path = "/sphenix/user/vassalli/minBiasPythia/hard0ana.root";
  string hard4Path = "/sphenix/user/vassalli/minBiasPythia/hard4ana.root";
  TChain *softTree = new TChain("photonTree");
  //TChain *hard0Tree = new TChain("photonTree");
  TChain *hard4Tree = new TChain("photonTree");
  softTree->Add(softPath.c_str());
  //hard0Tree->Add(hard0Path.c_str());
  hard4Tree->Add(hard4Path.c_str());
  /*TChain *ttree = handleFile(treePath,treeExtension,"cutTreeSignal",nFiles);
  TChain *pairBackTree = handleFile(treePath,treeExtension,"pairBackTree",nFiles);
  TChain *vtxBackTree = handleFile(treePath,treeExtension,"vtxBackTree",nFiles);
  TChain *vertexingTree = handleFile(treePath,treeExtension,"vtxingTree",nFiles);
  makephotonM(ttree,vtxBackTree,out_file);*/
  auto pythiaSpec=makePythiaSpec(softTree,out_file,"soft");
  //makePythiaSpec(hard0Tree,out_file,"hard0");
  auto hardSpec = makePythiaSpec(hard4Tree,out_file,"hard4");
  //chopHard(*hardSpec,*pythiaSpec);
  pythiaSpec = addSpec(pythiaSpec,hardSpec,out_file);
  //makepTDist(ttree,observations,out_file);
  /*makeVtxRes(ttree,out_file);
  makeVtxEff(ttree,out_file);
  makepTRes(ttree);
  compareDeta(ttree,pairBackTree);
  //testCuts(ttree,out_file);
  makepTCaloGraph("pTcalodata.csv",out_file);
  makeVtxR(ttree,vertexingTree,out_file);*/
  //makeRefitDist(ttree,out_file);
}
