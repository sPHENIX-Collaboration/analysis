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

void makeMaps(TChain* ttree,TFile* out_file){
  float vtxR;
  float tvtxR;
  float vtx_phi;
  float tvtx_phi;
  ttree->SetBranchAddress("vtx_radius",&vtxR);
  ttree->SetBranchAddress("tvtx_radius",&tvtxR);
  ttree->SetBranchAddress("vtx_phi",&vtx_phi);
  ttree->SetBranchAddress("tvtx_phi",&tvtx_phi);

  TH2F *map = new TH2F("recoMap","",100,-30,30,100,-30,30);
  TH2F *tmap = new TH2F("truthMap","",100,-30,30,100,-30,30);
  map->Sumw2();
  tmap->Sumw2();

  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    map->Fill(vtxR*TMath::Cos(vtx_phi),vtxR*TMath::Sin(vtx_phi));
    tmap->Fill(tvtxR*TMath::Cos(tvtx_phi),tvtxR*TMath::Sin(tvtx_phi));
  }
  out_file->Write();
}

void makeDists(TChain *vtxTree, TChain *mainTree, TFile *outf){
  TH1F *tmap = new TH1F("truthDist","",60,0,30);
  TH1F *rmap = new TH1F("recoDist","",90,0,45);
  TH1F *cmap = new TH1F("correctedDist","",60,0,30);
  tmap->Sumw2();
  rmap->Sumw2();
  cmap->Sumw2();

  float tvtx,rvtx,cvtx;
  vtxTree->SetBranchAddress("vtx_radius",&rvtx);
  vtxTree->SetBranchAddress("tvtx_radius",&tvtx);
  mainTree->SetBranchAddress("vtx_radius",&cvtx);

  for (int event = 0; event < vtxTree->GetEntries(); ++event)
  {
    vtxTree->GetEvent(event);
    tmap->Fill(tvtx);
    rmap->Fill(rvtx);
  }
  for (int event = 0; event < mainTree->GetEntries(); ++event)
  {
    mainTree->GetEvent(event);
    cmap->Fill(cvtx);
  }
  cmap->Scale(1./mainTree->GetEntries());
  tmap->Scale(1./vtxTree->GetEntries());
  rmap->Scale(1./vtxTree->GetEntries());
  outf->Write();
}

void mapper()
{
  TFile *out_file = new TFile("maps.root","RECREATE");
  string treePath = "/sphenix/user/vassalli/gammasample/truthconversionembededanalysis";
  string treeExtension = ".root";
  unsigned int nFiles=1000;
  TChain *vtx_tree = handleFile(treePath,treeExtension,"vtxingTree",nFiles);
  TChain *main_tree = handleFile(treePath,treeExtension,"cutTreeSignal",nFiles);
  cout<<"mapping with "<<vtx_tree->GetEntries()<<" verticies"<<endl;
  makeMaps(main_tree,out_file);
//  makeDists(vtx_tree,main_tree,out_file);
}
