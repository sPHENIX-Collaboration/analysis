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

void trackpT()
{
  float electron_pt[200];
  float positron_pt[200];
  float electron_reco_pt[200];
  float positron_reco_pt[200];
  double rVtx[200];
  int b_layer[20];
  int    truth_n;
  int  reco_n;
  int nVtx;


  string treePath = "/sphenix/user/vassalli/gammasample/fourembededonlineanalysis";
  string treeExtension = ".root";
  unsigned int nFiles=100;
  TChain *ttree = handleFile(treePath,treeExtension,"ttree",nFiles);
  ttree->SetBranchAddress("electron_pt",     &electron_pt    );
  ttree->SetBranchAddress("electron_reco_pt",&electron_reco_pt    );
  ttree->SetBranchAddress("positron_pt",     &positron_pt    );
  ttree->SetBranchAddress("positron_reco_pt",&positron_reco_pt    );
  ttree->SetBranchAddress("nTpair",          &truth_n  );
  ttree->SetBranchAddress("nRpair",          &reco_n  );
  ttree->SetBranchAddress("fLayer",          &b_layer  );
  ttree->SetBranchAddress("nVtx",          &nVtx  );
  ttree->SetBranchAddress("rVtx",          &rVtx  );
  

  string outfilename = "pTeffdists.root";
  TFile *out = new TFile(outfilename.c_str(),"RECREATE");

  TH1F *h_TepT = new TH1F("TepT","",100,0,30);
  TH1F *h_RepT = new TH1F("RepT","",100,0,30);

  TH1F *h_rvtx = new TH1F("rvtx","",100,0,30);
  TH1F *h_layer = new TH1F("layer","",24,-.5,23.5);

  h_TepT->GetXaxis()->SetTitle("pT");
  h_TepT->GetYaxis()->SetTitle("N");
  h_RepT->GetXaxis()->SetTitle("pT");
  h_RepT->GetYaxis()->SetTitle("N");

  h_rvtx->GetXaxis()->SetTitle("radius [cm]");
  h_rvtx->GetYaxis()->SetTitle("N");
  
  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    for (int i = 0; i < truth_n; ++i)
    {
      h_RepT->Fill(electron_reco_pt[i]);
      h_RepT->Fill(positron_reco_pt[i]);
    }
    for (int i = 0; i < nVtx; ++i)
    {
      h_TepT->Fill(electron_pt[i]);
      h_TepT->Fill(positron_pt[i]);
      h_rvtx->Fill(rVtx[i]);
    }
    for (int i = 0; i < reco_n; ++i)
    {
      h_layer->Fill(b_layer[i]);
    }
  }  
  out->Write();
  out->Close();
  delete ttree;
  delete out;
}
