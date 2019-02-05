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

void cluster2Plotter()
{
  float deta[200];
  float dphi[200];
  int    cluster_n;

  string treePath = "/sphenix/user/vassalli/gammasample/fourembededonlineanalysis";
  string treeExtension = ".root";
  unsigned int nFiles=100;
  TChain *ttree = handleFile(treePath,treeExtension,"ttree",nFiles);
  ttree->SetBranchAddress("clus_dphi",    &dphi    );
  ttree->SetBranchAddress("clus_deta",    &deta    );
  ttree->SetBranchAddress("nCluster",    &cluster_n  );

  string outfilename = "clus2plot.root";
  TFile *out = new TFile(outfilename.c_str(),"RECREATE");

  string plotname = "clus2plot";

  TH2F *h_2clusplot = new TH2F("clus2plot","",80,0,.05,80,0,.5); 
  TH1F *h_clusCount = new TH1F("cluscount","",2,0.5,2.5);
  h_2clusplot->SetStats(kFALSE);
  h_2clusplot->GetXaxis()->SetTitle("eta");
  h_2clusplot->GetYaxis()->SetTitle("phi");
  float meanEta=0;
  float meanPhi=0;
  long sum=0;
  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    h_clusCount->Fill(cluster_n);
    for (int i = 0; i < cluster_n&&cluster_n>1; ++i)
    {
      h_2clusplot->Fill(deta[i],dphi[i]);
      meanEta+=deta[i];
      meanPhi+=dphi[i];
    }
    sum+=cluster_n;
  }  
  meanEta/=sum;
  meanPhi/=sum;
  cout<<"Done with mean eta:"<<meanEta<<" and mean phi:"<<meanPhi<<'\n';
  out->Write();
  out->Close();
  delete ttree;
  delete out;
}
