// ----------------------------------------------------------------------------
// TFiles and TTrees
// ----------------------------------------------------------------------------

TFile *f_1 = new TFile("/sphenix/user/gregtom3/data/Summer2018/ExclusiveReco_studies/latest_run/10x100_1M_dvmp.root","READ");
//TFile *f_2 = new TFile("/sphenix/user/gregtom3/data/Summer2018/ExclusiveReco_studies/latest_run/10x100_1M_dvmp.root");
//TFile *f_3 = new TFile("/sphenix/user/gregtom3/data/Summer2018/ExclusiveReco_studies/latest_run/18x275_1M_dvmp.root");


TTree *t_1_exclusive = (TTree*)f_1->Get("event_exclusive");
TTree *t_1_reco = (TTree*)f_1->Get("event_reco");
TTree *t_1_truth = (TTree*)f_1->Get("event_truth");

//TTree *t_2_exclusive = (TTree*)f_2->Get("event_exclusive");
//TTree *t_2_reco = (TTree*)f_2->Get("event_reco");
//TTree *t_2_truth = (TTree*)f_2->Get("event_truth");

//TTree *t_3_exclusive = (TTree*)f_3->Get("event_exclusive");
//TTree *t_3_reco = (TTree*)f_3->Get("event_reco");
//TTree *t_3_truth = (TTree*)f_3->Get("event_truth");

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
  TH1F * h1_inv_mass_10x100_123456 = (TH1F*)h1_inv_mass_base->Clone();
  TH1F * h1_inv_mass_10x100_1 = (TH1F*)h1_inv_mass_base->Clone();
  TH1F * h1_inv_mass_10x100_2 = (TH1F*)h1_inv_mass_base->Clone();
  TH1F * h1_inv_mass_10x100_3 = (TH1F*)h1_inv_mass_base->Clone();
  TH1F * h1_inv_mass_10x100_4 = (TH1F*)h1_inv_mass_base->Clone();
  TH1F * h1_inv_mass_10x100_5 = (TH1F*)h1_inv_mass_base->Clone();
  TH1F * h1_inv_mass_10x100_6 = (TH1F*)h1_inv_mass_base->Clone();
  // ----------------------------------------
  // Setting Axes Titles
  // ----------------------------------------
  h1_inv_mass_10x100_123456->GetXaxis()->SetTitle("Invariant Mass [GeV/c^{2}]");
  h1_inv_mass_10x100_123456->GetYaxis()->SetTitle("Counts");

  h1_inv_mass_10x100_1->GetXaxis()->SetTitle("Invariant Mass [GeV/c^{2}]");
  h1_inv_mass_10x100_1->GetYaxis()->SetTitle("Counts");

  h1_inv_mass_10x100_2->GetXaxis()->SetTitle("Invariant Mass [GeV/c^{2}]");
  h1_inv_mass_10x100_2->GetYaxis()->SetTitle("Counts");

  h1_inv_mass_10x100_3->GetXaxis()->SetTitle("Invariant Mass [GeV/c^{2}]");
  h1_inv_mass_10x100_3->GetYaxis()->SetTitle("Counts");

  h1_inv_mass_10x100_4->GetXaxis()->SetTitle("Invariant Mass [GeV/c^{2}]");
  h1_inv_mass_10x100_4->GetYaxis()->SetTitle("Counts");

  h1_inv_mass_10x100_5->GetXaxis()->SetTitle("Invariant Mass [GeV/c^{2}]");
  h1_inv_mass_10x100_5->GetYaxis()->SetTitle("Counts");

  h1_inv_mass_10x100_6->GetXaxis()->SetTitle("Invariant Mass [GeV/c^{2}]");
  h1_inv_mass_10x100_6->GetYaxis()->SetTitle("Counts");
  // ----------------------------------------
  // Filling Histograms and Saving Them (Individual)
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
      //fillHist(h1_inv_mass_10x100_123456, t_1_exclusive, types.at(idx_type));
      //hist_to_png(h1_inv_mass_10x100_123456,"10x100", types.at(idx_type));
    }
  

  // --------------------------------------------
  // Filling Histograms and Saving Them (Special)
  // --------------------------------------------
  fillHist(h1_inv_mass_10x100_1, t_1_exclusive, "reco_inv");
  fillHist(h1_inv_mass_10x100_2, t_1_exclusive, "true_inv");
  fillHist(h1_inv_mass_10x100_3, t_1_exclusive, "reco_inv_decay");
  fillHist(h1_inv_mass_10x100_4, t_1_exclusive, "reco_inv_scatter");
  fillHist(h1_inv_mass_10x100_5, t_1_exclusive, "true_inv_decay");
  fillHist(h1_inv_mass_10x100_6, t_1_exclusive, "true_inv_scatter");

  hist_to_png_special(h1_inv_mass_10x100_1,
		      h1_inv_mass_10x100_2,
		      h1_inv_mass_10x100_3,
		      h1_inv_mass_10x100_4,
		      h1_inv_mass_10x100_5,
		      h1_inv_mass_10x100_6, "10x100");
		      
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

void hist_to_png_special(TH1F * h1, TH1F * h2, TH1F *h3,
			 TH1F * h4, TH1F * h5, TH1F *h6,
			 TString type)
{
  //h1 reco
  //h2 true
  //h3 reco decay
  //h4 reco scatter
  //h5 true decay
  //h6 true scatter

  // Plot 1: Reco Inv. Mass Full
  TCanvas *cPNG_1 = new TCanvas("plot1","",700,500);
  TImage *img_1 = TImage::Create();
  auto legend_1 = new TLegend(0.6,0.75,0.9,0.9);
  h1->SetLineColor(kRed);
  h4->SetLineColor(kBlue);
  legend_1->SetHeader("Reco Decay Particles");
  legend_1->AddEntry(h1,"All e^{-}e^{+}","l");
  legend_1->AddEntry(h4,"Background e^{-}e^{+}","l");
  h1->Draw();
  h4->Draw("SAME");
  legend_1->Draw();
  gPad->RedrawAxis();
  gPad->SetLogy();
  img_1->FromPad(cPNG_1);
  img_1->WriteImage("plot1__"+type+"_dvmp_reco_inv_full.png");
  delete img_1;

  // Plot 2: True Inv. Mass Split
  TCanvas *cPNG_2 = new TCanvas("plot2","",700,500);
  TImage *img_2 = TImage::Create();
  h2->SetLineColor(kRed);
  h6->SetLineColor(kBlue);
  auto legend_2 = new TLegend(0.6,0.75,0.9,0.9);
  legend_2->SetHeader("True Decay Particles");
  legend_2->AddEntry(h2,"All e^{-}e^{+}","l");
  legend_2->AddEntry(h6,"Background e^{-}e^{+}","l");
  h2->Draw();
  h6->Draw("SAME");
  legend_2->Draw();
  gPad->RedrawAxis();
  gPad->SetLogy();
  img_2->FromPad(cPNG_2);
  img_2->WriteImage("plot2__"+type+"_dvmp_true_inv_full.png");
  delete img_2;

  // Plot 3: Combine plots 1 and 2
  TCanvas *cPNG_3 = new TCanvas("plot3","",1400,500);
  TImage *img_3 = TImage::Create();

  cPNG_3->Divide(2,1);
  cPNG_3->cd(1);
  h1->Draw();
  h4->Draw("same");
  legend_1->Draw();
  gPad->RedrawAxis();
  gPad->SetLogy();
  
  cPNG_3->cd(2);
  h2->Draw();
  h6->Draw("same");
  legend_2->Draw();
  gPad->RedrawAxis();
  gPad->SetLogy();
  
  img_3->FromPad(cPNG_3);
  img_3->WriteImage("plot3__"+type+"_dvmp_inv_full.png");
  delete img_3;
}
