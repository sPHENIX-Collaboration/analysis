#include "TreeClass.C"

#include <vector>
// ----------------------------------------------------------------------------
// TFiles and TTrees
// ----------------------------------------------------------------------------

TFile *f_1 = new TFile("/sphenix/user/gregtom3/data/Summer2018/ExclusiveReco_studies/latest_run/10x100_10k_dvmp.root","READ");
//TFile *f_2 = new TFile("/sphenix/user/gregtom3/data/Summer2018/ExclusiveReco_studies/latest_run/10x150_10k_dvmp.root");
//TFile *f_3 = new TFile("/sphenix/user/gregtom3/data/Summer2018/ExclusiveReco_studies/latest_run/10x200_10k_dvmp.root");
//TFile *f_4 = new TFile("/sphenix/user/gregtom3/data/Summer2018/ExclusiveReco_studies/latest_run/10x250_10k_dvmp.root");
//TFile *f_5 = new TFile("/sphenix/user/gregtom3/data/Summer2018/ExclusiveReco_studies/latest_run/20x250_10k_dvmp.root");
//TFile *f_6 = new TFile("/sphenix/user/gregtom3/data/Summer2018/ExclusiveReco_studies/latest_run/18x275_10k_dvmp.root");

TTree *t_1_exclusive = (TTree*)f_1->Get("event_exclusive");
TTree *t_1_reco = (TTree*)f_1->Get("event_reco");
TTree *t_1_truth = (TTree*)f_1->Get("event_truth");

//TTree *t_2_exclusive = (TTree*)f_2->Get("event_exclusive");
//TTree *t_2_reco = (TTree*)f_2->Get("event_reco");
//TTree *t_2_truth = (TTree*)f_2->Get("event_truth");

//TTree *t_3_exclusive = (TTree*)f_3->Get("event_exclusive");
//TTree *t_3_reco = (TTree*)f_3->Get("event_reco");
//TTree *t_3_truth = (TTree*)f_3->Get("event_truth");

//TTree *t_4_exclusive = (TTree*)f_4->Get("event_exclusive");
//TTree *t_4_reco = (TTree*)f_4->Get("event_reco");
//TTree *t_4_truth = (TTree*)f_4->Get("event_truth");

//TTree *t_5_exclusive = (TTree*)f_5->Get("event_exclusive");
//TTree *t_5_reco = (TTree*)f_5->Get("event_reco");
//TTree *t_5_truth = (TTree*)f_5->Get("event_truth");

//TTree *t_6_exclusive = (TTree*)f_6->Get("event_exclusive");
//TTree *t_6_reco = (TTree*)f_6->Get("event_reco");
//TTree *t_6_truth = (TTree*)f_6->Get("event_truth");

// ----------------------------------------------------------------------------
// Main Code 
// ----------------------------------------------------------------------------

int jpsi_invariant_mass_plot_exclusiveReco()
{

  // ----------------------------------------
  // Load Sphenix Style for Histogram Plots
  // ----------------------------------------
  gROOT->LoadMacro("/sphenix/user/gregtom3/SBU/research/macros/macros/sPHENIXStyle/sPhenixStyle.C");
  SetsPhenixStyle();
  // ----------------------------------------
  // Creating Histogram Base
  // ----------------------------------------
  TH1F * h1_inv_mass_base = new TH1F("","",100,0,10);
  // ----------------------------------------
  // Creating Invariant Mass Histograms
  // ----------------------------------------
  TH1F * h1_inv_mass_10x100 = (TH1F*)h1_inv_mass_base->Clone();
  TH1F * h1_inv_mass_10x150 = (TH1F*)h1_inv_mass_base->Clone();
  TH1F * h1_inv_mass_10x200 = (TH1F*)h1_inv_mass_base->Clone();
  TH1F * h1_inv_mass_10x250 = (TH1F*)h1_inv_mass_base->Clone();
  TH1F * h1_inv_mass_20x250 = (TH1F*)h1_inv_mass_base->Clone();
  TH1F * h1_inv_mass_18x275 = (TH1F*)h1_inv_mass_base->Clone();
  // ----------------------------------------
  // Setting Axes Titles
  // ----------------------------------------
  h1_inv_mass_10x100->GetXaxis()->SetTitle("Invariant Mass [GeV/c^{2}]");
  h1_inv_mass_10x100->GetYaxis()->SetTitle("Counts");

  h1_inv_mass_10x150->GetXaxis()->SetTitle("Invariant Mass [GeV/c^{2}]");
  h1_inv_mass_10x150->GetYaxis()->SetTitle("Counts");
  
  h1_inv_mass_10x200->GetXaxis()->SetTitle("Invariant Mass [GeV/c^{2}]");
  h1_inv_mass_10x200->GetYaxis()->SetTitle("Counts");
  
  h1_inv_mass_10x250->GetXaxis()->SetTitle("Invariant Mass [GeV/c^{2}]");
  h1_inv_mass_10x250->GetYaxis()->SetTitle("Counts");
  
  h1_inv_mass_20x250->GetXaxis()->SetTitle("Invariant Mass [GeV/c^{2}]");
  h1_inv_mass_20x250->GetYaxis()->SetTitle("Counts");
  
  h1_inv_mass_18x275->GetXaxis()->SetTitle("Invariant Mass [GeV/c^{2}]");
  h1_inv_mass_18x275->GetYaxis()->SetTitle("Counts");
  // ----------------------------------------
  // Filling Histograms and Saving Them
  // ----------------------------------------
  std::vector<TString> types;
  types.push_back("reco_inv");
  types.push_back("true_inv");
  types.push_back("reco_inv_decay");
  types.push_back("reco_inv_scatter");
  types.push_back("true_inv_decay");
  types.push_back("true_inv_scatter");
  for(unsigned idx_type = 0 ; idx_type < types.size() ; idx_type++)
    {
      fillHist(h1_inv_mass_10x100, t_1_exclusive, types.at(idx_type));
      hist_to_png(h1_inv_mass_10x100,"10x100", types.at(idx_type));
      
    }
  
  return 0;
}

void fillHist(TH1F * h, TTree *t, TString type_TString)
{
  std::vector<float> reco_inv;
  std::vector<float> true_inv;
  std::vector<float> reco_inv_decay;
  std::vector<float> reco_inv_scatter;
  std::vector<float> true_inv_decay;
  std::vector<float> true_inv_scatter;

  std::vector<float>* reco_inv_pointer = &reco_inv;
  std::vector<float>* true_inv_pointer = &true_inv;
  std::vector<float>* reco_inv_decay_pointer = &reco_inv_decay;
  std::vector<float>* reco_inv_scatter_pointer = &reco_inv_scatter;
  std::vector<float>* true_inv_decay_pointer = &true_inv_decay;
  std::vector<float>* true_inv_scatter_pointer = &true_inv_scatter;

  t->SetBranchAddress("reco_inv",&reco_inv_pointer);
  t->SetBranchAddress("true_inv",&true_inv_pointer);
  t->SetBranchAddress("reco_inv_decay",&reco_inv_decay_pointer);
  t->SetBranchAddress("reco_inv_scatter",&reco_inv_scatter_pointer);
  t->SetBranchAddress("true_inv_decay",&true_inv_decay_pointer);
  t->SetBranchAddress("true_inv_scatter",&true_inv_scatter_pointer);
  
  h->Reset();
  Int_t nentries = Int_t(t->GetEntries());
  char * type = type_TString.Data();
  for(Int_t entryInChain=0; entryInChain<nentries; entryInChain++)
    {
      Int_t entryInTree = t->LoadTree(entryInChain);
      if (entryInTree < 0) break;
      t->GetEntry(entryInChain);
      if(strcmp(type,"reco_inv")==0)
	{
	  for(unsigned i = 0 ; i<reco_inv.size() ; i++)
	    h->Fill(reco_inv.at(i));
	}
      else if(strcmp(type,"true_inv")==0)
	{
	   for(unsigned i = 0 ; i<true_inv.size() ; i++)
	    h->Fill(true_inv.at(i));
	}
      else if(strcmp(type,"reco_inv_decay")==0)
	{
	   for(unsigned i = 0 ; i<reco_inv_decay.size() ; i++)
	    h->Fill(reco_inv_decay.at(i));
	}
      else if(strcmp(type,"reco_inv_scatter")==0)
	{
	   for(unsigned i = 0 ; i<reco_inv_scatter.size() ; i++)
	    h->Fill(reco_inv_scatter.at(i));
	}
      else if(strcmp(type,"true_inv_decay")==0)
	{
	   for(unsigned i = 0 ; i<true_inv_decay.size() ; i++)
	    h->Fill(true_inv_decay.at(i));
	}
      else if(strcmp(type,"true_inv_scatter")==0)
	{
	   for(unsigned i = 0 ; i<true_inv_scatter.size() ; i++)
	    h->Fill(true_inv_scatter.at(i));
	}
    }
}

void hist_to_png(TH1F * h, TString saveTitle, TString type)
{
  saveTitle = type + "_" + saveTitle+".png";
  TCanvas *cPNG = new TCanvas(saveTitle,"",700,500);
  TImage *img = TImage::Create();
  h->Draw();
  img->FromPad(cPNG);
  img->WriteImage(saveTitle);
  delete img;
}
