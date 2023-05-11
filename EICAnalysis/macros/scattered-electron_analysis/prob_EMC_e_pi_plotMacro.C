#include <iostream>
#include <cstdlib>

#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"

TTree* loadTree(
		  TString FILE_NAME="",
		  TString TREE_NAME="",
		  const TString DIR_PATH = "/sphenix/user/gregtom3/data/Summer2018/ECAL_probability_studies/"
		)
{
  TFile *f = new TFile(DIR_PATH+FILE_NAME,"READ");
  TTree *t = (TTree*)f->Get(TREE_NAME);
  return t;
}

TH1F* fillHist(TH1F *THE_HIST, TTree *THE_TREE)
{
  std::vector<float> measured_energy;
  std::vector<float> measured_ptotal;
  std::vector<float> shower_probability; //integration of chi2 PDF, tells us likelihood of this specific cluster radius corresponds to that of an electron's
  std::vector<float>* measured_energy_pointer = &measured_energy;
  std::vector<float>* measured_ptotal_pointer = &measured_ptotal;
  std::vector<float>* shower_probability_pointer = &shower_probability;
  THE_TREE->SetBranchAddress("em_cluster_e",&measured_energy_pointer);
  THE_TREE->SetBranchAddress("em_track_ptotal",&measured_ptotal_pointer);
  THE_TREE->SetBranchAddress("em_cluster_prob",&shower_probability_pointer);
  
  Int_t nentries = Int_t(THE_TREE->GetEntries());
  for(Int_t entryInChain=0; entryInChain<nentries; entryInChain++)
    {
      Int_t entryInTree = THE_TREE->LoadTree(entryInChain);
      if (entryInTree < 0) break;
      THE_TREE->GetEntry(entryInChain);
      for(int i=0;i<shower_probability.size();i++)
	{
	  if(measured_energy[i]>0.5) //Cuts
	    THE_HIST->Fill(shower_probability[i]);
	}
    }
  THE_HIST->SetXTitle("P_{EM}");
  THE_HIST->SetYTitle("Counts");
  
  return THE_HIST;
}

void histToPNG(TH1F* h_p, TH1F* h_e, char * title, char * saveFileName)
{
  TCanvas *cPNG = new TCanvas("cPNG",title); // 700 x 500 default
  TImage *img = TImage::Create();

  h_p->Draw();
  h_e->Draw("SAME");
  h_p->GetXaxis()->SetNdivisions(6,2,0,false);
  h_p->GetYaxis()->SetNdivisions(5,3,0,false);
  h_p->GetYaxis()->SetRangeUser(0,5000);
  auto legend = new TLegend(0.7,0.65,0.95,0.90,title);
  legend->AddEntry(h_p,"Pions","l");
  legend->AddEntry(h_e,"Electrons","l");
  legend->SetTextSize(0.05);
  legend->Draw();
  gPad->RedrawAxis();
  img->FromPad(cPNG);
  img->WriteImage(saveFileName);
  
  delete img;
  delete cPNG;

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
/                                 Main Code
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void prob_EMC_e_pi_plotMacro(
			     const TString DIR_PATH = "/sphenix/user/gregtom3/data/Summer2018/ECAL_probability_studies/"
                            )
{
  
  /* *
   * sPHENIX Style
   */
  
  gROOT->LoadMacro("/sphenix/user/gregtom3/SBU/research/macros/macros/sPHENIXStyle/sPhenixStyle.C");
  SetsPhenixStyle();
  gROOT->SetBatch(kTRUE);

  /* *
   * Base Histogram
   */
   
  TH1F *h_base = new TH1F("h_base","",25,0,1.2);
  TH1F *h_base_e = (TH1F*)h_base->Clone(); 
  TH1F *h_base_p = (TH1F*)h_base->Clone();

  h_base_e->SetLineColor(kRed);
  h_base_p->SetLineColor(kBlue);

  /* *
   * Loading histograms and trees for electrons and pions [1, 2, 5, 10, 20] GeV
   * CEMC Eta -0.5 0.5
   * EEMC Eta -3 -2
   * 10000 events per energy per particle per detector
   */

  /* CEMC */
  TH1F *h_P_1GeV_CEMC = (TH1F*)h_base_p->Clone();
  TH1F *h_P_2GeV_CEMC = (TH1F*)h_base_p->Clone();
  TH1F *h_P_5GeV_CEMC = (TH1F*)h_base_p->Clone();
  TH1F *h_P_10GeV_CEMC = (TH1F*)h_base_p->Clone();
  TH1F *h_P_20GeV_CEMC = (TH1F*)h_base_p->Clone();

  TH1F *h_E_1GeV_CEMC = (TH1F*)h_base_e->Clone();
  TH1F *h_E_2GeV_CEMC = (TH1F*)h_base_e->Clone();
  TH1F *h_E_5GeV_CEMC = (TH1F*)h_base_e->Clone();
  TH1F *h_E_10GeV_CEMC = (TH1F*)h_base_e->Clone();
  TH1F *h_E_20GeV_CEMC = (TH1F*)h_base_e->Clone();

  h_P_1GeV_CEMC->SetName("h_P_1GeV_CEMC");
  h_P_2GeV_CEMC->SetName("h_P_2GeV_CEMC");
  h_P_5GeV_CEMC->SetName("h_P_5GeV_CEMC");
  h_P_10GeV_CEMC->SetName("h_P_10GeV_CEMC");
  h_P_20GeV_CEMC->SetName("h_P_20GeV_CEMC");

  h_E_1GeV_CEMC->SetName("h_E_1GeV_CEMC");
  h_E_2GeV_CEMC->SetName("h_E_2GeV_CEMC");
  h_E_5GeV_CEMC->SetName("h_E_5GeV_CEMC");
  h_E_10GeV_CEMC->SetName("h_E_10GeV_CEMC");
  h_E_20GeV_CEMC->SetName("h_E_20GeV_CEMC");

  TTree *t_P_1GeV_CEMC = loadTree("Pions/Pions1C.root", "event_cluster");
  TTree *t_P_2GeV_CEMC = loadTree("Pions/Pions2C.root", "event_cluster");
  TTree *t_P_5GeV_CEMC = loadTree("Pions/Pions5C.root", "event_cluster");
  TTree *t_P_10GeV_CEMC = loadTree("Pions/Pions10C.root", "event_cluster");
  TTree *t_P_20GeV_CEMC = loadTree("Pions/Pions20C.root", "event_cluster");
  
  TTree *t_E_1GeV_CEMC = loadTree("Electrons/Electrons1C.root", "event_cluster");
  TTree *t_E_2GeV_CEMC = loadTree("Electrons/Electrons2C.root", "event_cluster");
  TTree *t_E_5GeV_CEMC = loadTree("Electrons/Electrons5C.root", "event_cluster");
  TTree *t_E_10GeV_CEMC = loadTree("Electrons/Electrons10C.root", "event_cluster");
  TTree *t_E_20GeV_CEMC = loadTree("Electrons/Electrons20C.root", "event_cluster");

  /* EEMC */
  TH1F *h_P_1GeV_EEMC = (TH1F*)h_base_p->Clone();
  TH1F *h_P_2GeV_EEMC = (TH1F*)h_base_p->Clone();
  TH1F *h_P_5GeV_EEMC = (TH1F*)h_base_p->Clone();
  TH1F *h_P_10GeV_EEMC = (TH1F*)h_base_p->Clone();
  TH1F *h_P_20GeV_EEMC = (TH1F*)h_base_p->Clone();

  TH1F *h_E_1GeV_EEMC = (TH1F*)h_base_e->Clone();
  TH1F *h_E_2GeV_EEMC = (TH1F*)h_base_e->Clone();
  TH1F *h_E_5GeV_EEMC = (TH1F*)h_base_e->Clone();
  TH1F *h_E_10GeV_EEMC = (TH1F*)h_base_e->Clone();
  TH1F *h_E_20GeV_EEMC = (TH1F*)h_base_e->Clone();

  h_P_1GeV_EEMC->SetName("h_P_1GeV_EEMC");
  h_P_2GeV_EEMC->SetName("h_P_2GeV_EEMC");
  h_P_5GeV_EEMC->SetName("h_P_5GeV_EEMC");
  h_P_10GeV_EEMC->SetName("h_P_10GeV_EEMC");
  h_P_20GeV_EEMC->SetName("h_P_20GeV_EEMC");

  h_E_1GeV_EEMC->SetName("h_E_1GeV_EEMC");
  h_E_2GeV_EEMC->SetName("h_E_2GeV_EEMC");
  h_E_5GeV_EEMC->SetName("h_E_5GeV_EEMC");
  h_E_10GeV_EEMC->SetName("h_E_10GeV_EEMC");
  h_E_20GeV_EEMC->SetName("h_E_20GeV_EEMC");

  TTree *t_P_1GeV_EEMC = loadTree("Pions/Pions1E.root", "event_cluster");
  TTree *t_P_2GeV_EEMC = loadTree("Pions/Pions2E.root", "event_cluster");
  TTree *t_P_5GeV_EEMC = loadTree("Pions/Pions5E.root", "event_cluster");
  TTree *t_P_10GeV_EEMC = loadTree("Pions/Pions10E.root", "event_cluster");
  TTree *t_P_20GeV_EEMC = loadTree("Pions/Pions20E.root", "event_cluster");
  
  TTree *t_E_1GeV_EEMC = loadTree("Electrons/Electrons1E.root", "event_cluster");
  TTree *t_E_2GeV_EEMC = loadTree("Electrons/Electrons2E.root", "event_cluster");
  TTree *t_E_5GeV_EEMC = loadTree("Electrons/Electrons5E.root", "event_cluster");
  TTree *t_E_10GeV_EEMC = loadTree("Electrons/Electrons10E.root", "event_cluster");
  TTree *t_E_20GeV_EEMC = loadTree("Electrons/Electrons20E.root", "event_cluster");

  /* *
   * Filling Histograms
   */
  
  /* Pions CEMC */
  h_P_1GeV_CEMC=fillHist(h_P_1GeV_CEMC,t_P_1GeV_CEMC);
  h_P_2GeV_CEMC=fillHist(h_P_2GeV_CEMC,t_P_2GeV_CEMC);
  h_P_5GeV_CEMC=fillHist(h_P_5GeV_CEMC,t_P_5GeV_CEMC);
  h_P_10GeV_CEMC=fillHist(h_P_10GeV_CEMC,t_P_10GeV_CEMC);
  h_P_20GeV_CEMC=fillHist(h_P_20GeV_CEMC,t_P_20GeV_CEMC);
  /* Pions EEMC */
  //Lots of Bugs currently with event_cluster info from EEMC
  /*h_P_1GeV_EEMC=fillHist(h_P_1GeV_EEMC,t_P_1GeV_EEMC);
  h_P_2GeV_EEMC=fillHist(h_P_2GeV_EEMC,t_P_2GeV_EEMC);
  h_P_5GeV_EEMC=fillHist(h_P_5GeV_EEMC,t_P_5GeV_EEMC);
  h_P_10GeV_EEMC=fillHist(h_P_10GeV_EEMC,t_P_10GeV_EEMC);
  h_P_20GeV_EEMC=fillHist(h_P_20GeV_EEMC,t_P_20GeV_EEMC);*/


  /* Electrons CEMC */
  h_E_1GeV_CEMC=fillHist(h_E_1GeV_CEMC,t_E_1GeV_CEMC);
  h_E_2GeV_CEMC=fillHist(h_E_2GeV_CEMC,t_E_2GeV_CEMC);
  h_E_5GeV_CEMC=fillHist(h_E_5GeV_CEMC,t_E_5GeV_CEMC);
  h_E_10GeV_CEMC=fillHist(h_E_10GeV_CEMC,t_E_10GeV_CEMC);
  h_E_20GeV_CEMC=fillHist(h_E_20GeV_CEMC,t_E_20GeV_CEMC);
  /* Electrons EEMC */
  /*h_E_1GeV_EEMC=fillHist(h_E_1GeV_EEMC,t_E_1GeV_EEMC);
  h_E_2GeV_EEMC=fillHist(h_E_2GeV_EEMC,t_E_2GeV_EEMC);
  h_E_5GeV_EEMC=fillHist(h_E_5GeV_EEMC,t_E_5GeV_EEMC);
  h_E_10GeV_EEMC=fillHist(h_E_10GeV_EEMC,t_E_10GeV_EEMC);
  h_E_20GeV_EEMC=fillHist(h_E_20GeV_EEMC,t_E_20GeV_EEMC);*/
  
  /* *
   * Drawing Histograms and Saving Them
   */
  
  /* CEMC */
  histToPNG(h_P_1GeV_CEMC,h_E_1GeV_CEMC,"1GeV CEMC","prob_e_pi_1GeV_CEMC.png");
  histToPNG(h_P_2GeV_CEMC,h_E_2GeV_CEMC,"2GeV CEMC","prob_e_pi_2GeV_CEMC.png");
  histToPNG(h_P_5GeV_CEMC,h_E_5GeV_CEMC,"5GeV CEMC","prob_e_pi_5GeV_CEMC.png");
  histToPNG(h_P_10GeV_CEMC,h_E_10GeV_CEMC,"10GeV CEMC","prob_e_pi_10GeV_CEMC.png");
  histToPNG(h_P_20GeV_CEMC,h_E_20GeV_CEMC,"20GeV CEMC","prob_e_pi_20GeV_CEMC.png");
 /* EEMC */
  /*histToPNG(h_P_1GeV_EEMC,h_E_1GeV_EEMC,"1GeV EEMC","prob_e_pi_1GeV_EEMC.png");
  histToPNG(h_P_2GeV_EEMC,h_E_2GeV_EEMC,"2GeV EEMC","prob_e_pi_2GeV_EEMC.png");
  histToPNG(h_P_5GeV_EEMC,h_E_5GeV_EEMC,"5GeV EEMC","prob_e_pi_5GeV_EEMC.png");
  histToPNG(h_P_10GeV_EEMC,h_E_10GeV_EEMC,"10GeV EEMC","prob_e_pi_10GeV_EEMC.png");
  histToPNG(h_P_20GeV_EEMC,h_E_20GeV_EEMC,"20GeV EEMC","prob_e_pi_20GeV_EEMC.png");*/
}



