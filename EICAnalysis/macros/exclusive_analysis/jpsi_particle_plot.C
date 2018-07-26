// ----------------------------------------------------------------------------
// TFiles and TTrees
// ----------------------------------------------------------------------------

TFile *f_1 = new TFile("/sphenix/user/gregtom3/data/Summer2018/ExclusiveReco_studies/latest_run/10x100_10k_dvmp.root","READ");

TTree *t_1_exclusive = (TTree*)f_1->Get("event_exclusive");
TTree *t_1_reco = (TTree*)f_1->Get("event_reco");
TTree *t_1_truth = (TTree*)f_1->Get("event_truth");

// ----------------------------------------------------------------------------
// Main Code 
// ----------------------------------------------------------------------------

int jpsi_particle_plot()
{
  // ----------------------------------------
  // Creating Histograms
  // ----------------------------------------
  TH2F * h2_eta_phi_em_base = new TH2F("","",35,-4,4,35,-3.5,3.5);
  TH2F * h2_eta_phi_pr_base = new TH2F("","",35,-20,20,35,-3.5,3.5);
  TH2F * h2_mom_eta_em_base = new TH2F("","",35,-4,4,35,0,20);
  TH2F * h2_mom_eta_pr_base = new TH2F("","",35,-20,20,35,0,150);
  // ----------------------------------------
  // Setting Axes Titles
  // ----------------------------------------
  h2_eta_phi_em_base->GetXaxis()->SetTitle("#eta");
  h2_eta_phi_em_base->GetYaxis()->SetTitle("#phi");
  h2_eta_phi_pr_base->GetXaxis()->SetTitle("#eta");
  h2_eta_phi_pr_base->GetYaxis()->SetTitle("#phi");
  
  h2_mom_eta_em_base->GetXaxis()->SetTitle("#eta");
  h2_mom_eta_em_base->GetYaxis()->SetTitle("Momentum [GeV]");
  h2_mom_eta_pr_base->GetXaxis()->SetTitle("#eta");
  h2_mom_eta_pr_base->GetYaxis()->SetTitle("Momentum [GeV]");
  // ----------------------------------------
  // Filling Histograms and Saving Them
  // ----------------------------------------
  std::vector<TString> types_eta_phi_em_reco;
  std::vector<TString> types_eta_phi_em_true;
  std::vector<TString> types_eta_phi_pr;
  std::vector<TString> types_mom_eta_em_reco;
  std::vector<TString> types_mom_eta_em_true;
  std::vector<TString> types_mom_eta_pr;
  types_eta_phi_em_reco.push_back("reco_decay_positron_eta_phi");
  types_eta_phi_em_reco.push_back("reco_decay_electron_eta_phi");
  types_eta_phi_em_reco.push_back("reco_scattered_electron_eta_phi");
  types_mom_eta_em_reco.push_back("reco_decay_positron_mom_eta");
  types_mom_eta_em_reco.push_back("reco_decay_electron_mom_eta");
  types_mom_eta_em_reco.push_back("reco_scattered_electron_mom_eta");
  types_eta_phi_em_true.push_back("true_decay_positron_eta_phi");
  types_eta_phi_em_true.push_back("true_decay_electron_eta_phi");
  types_eta_phi_em_true.push_back("true_scattered_electron_eta_phi");
  types_eta_phi_pr.push_back("true_scattered_proton_eta_phi");
  types_mom_eta_em_true.push_back("true_decay_positron_mom_eta");
  types_mom_eta_em_true.push_back("true_decay_electron_mom_eta");
  types_mom_eta_em_true.push_back("true_scattered_electron_mom_eta");
  types_mom_eta_pr.push_back("true_scattered_proton_mom_eta");
  
  for(unsigned idx = 0 ; idx < types_eta_phi_em_reco.size() ; idx++)
    {
      fillHist(h2_eta_phi_em_base, t_1_reco, t_1_truth, types_eta_phi_em_reco.at(idx), true);
      hist_to_png(h2_eta_phi_em_base,"10x100", types_eta_phi_em_reco.at(idx));
    }

  for(unsigned idx = 0 ; idx < types_eta_phi_em_true.size() ; idx++)
    {
      fillHist(h2_eta_phi_em_base, t_1_reco, t_1_truth, types_eta_phi_em_true.at(idx), false);
      hist_to_png(h2_eta_phi_em_base,"10x100", types_eta_phi_em_true.at(idx));
    }

  for(unsigned idx = 0 ; idx < types_eta_phi_pr.size() ; idx++)
    {
      fillHist(h2_eta_phi_pr_base, t_1_reco, t_1_truth, types_eta_phi_pr.at(idx), false);
      hist_to_png(h2_eta_phi_pr_base,"10x100", types_eta_phi_pr.at(idx));
    }

  for(unsigned idx = 0 ; idx < types_mom_eta_em_reco.size() ; idx++)
    {
      fillHist(h2_mom_eta_em_base, t_1_reco, t_1_truth, types_mom_eta_em_reco.at(idx), true);
      hist_to_png(h2_mom_eta_em_base,"10x100", types_mom_eta_em_reco.at(idx));
    }

  for(unsigned idx = 0 ; idx < types_mom_eta_em_true.size() ; idx++)
    {
      fillHist(h2_mom_eta_em_base, t_1_reco, t_1_truth, types_mom_eta_em_true.at(idx), false);
      hist_to_png(h2_mom_eta_em_base,"10x100", types_mom_eta_em_true.at(idx));
    }


  for(unsigned idx = 0 ; idx < types_mom_eta_pr.size() ; idx++)
    {
      fillHist(h2_mom_eta_pr_base, t_1_reco, t_1_truth, types_mom_eta_pr.at(idx), false);
      hist_to_png(h2_mom_eta_pr_base,"10x100", types_mom_eta_pr.at(idx));
    }
  return 0;
}

void fillHist(TH2F * h, TTree *t_reco, TTree *t_truth, TString type_TString, bool reco)
{
  std::vector<float> reco_eta;
  std::vector<float> true_eta;
  std::vector<float> reco_phi;
  std::vector<float> true_phi;
  std::vector<float> reco_ptotal;
  std::vector<float> true_ptotal;
  std::vector<int> reco_charge;
  std::vector<int> pid;
  std::vector<bool> reco_is_scattered_lepton;
  std::vector<bool> is_scattered_lepton;
  std::vector<float> reco_cluster_e;

  std::vector<float>* reco_eta_pointer = &reco_eta;
  std::vector<float>* true_eta_pointer = &true_eta;
  std::vector<float>* reco_phi_pointer = &reco_phi;
  std::vector<float>* true_phi_pointer = &true_phi;
  std::vector<float>* reco_ptotal_pointer = &reco_ptotal;
  std::vector<float>* true_ptotal_pointer = &true_ptotal;
  std::vector<int>* reco_charge_pointer = &reco_charge;
  std::vector<int>* pid_pointer = &pid;
  std::vector<bool>* reco_is_scattered_lepton_pointer = &reco_is_scattered_lepton;
  std::vector<bool>* is_scattered_lepton_pointer = &is_scattered_lepton;
  std::vector<float>* reco_cluster_e_pointer = &reco_cluster_e;

  h->Reset();
  if(reco)
    {
      t_reco->SetBranchAddress("eta",&reco_eta_pointer);
      t_reco->SetBranchAddress("phi",&reco_phi_pointer);
      t_reco->SetBranchAddress("ptotal",&reco_ptotal_pointer);
      t_reco->SetBranchAddress("charge",&reco_charge_pointer);
      t_reco->SetBranchAddress("is_scattered_lepton",&reco_is_scattered_lepton_pointer);
      t_reco->SetBranchAddress("cluster_e",&reco_cluster_e_pointer);
      Int_t nentries = Int_t(t_reco->GetEntries());
      char * type = type_TString.Data();
      for(Int_t entryInChain=0; entryInChain<nentries; entryInChain++)
	{
	  Int_t entryInTree = t_reco->LoadTree(entryInChain);
	  if (entryInTree < 0) break;
	  t_reco->GetEntry(entryInChain);
	  unsigned size_reco = reco_eta.size();
	  if(strcmp(type,"reco_decay_positron_eta_phi")==0)
	    {
	      for(unsigned i = 0 ; i < size_reco ; i++)
		{
		  if(reco_charge.at(i)==1)
		      h->Fill(reco_eta.at(i),reco_phi.at(i));
		}
	    }
	  else if(strcmp(type,"reco_decay_electron_eta_phi")==0)
	    {
	      for(unsigned i = 0 ; i < size_reco ; i++)
		{
		  if(reco_charge.at(i)==-1)
		    {
		      if(!reco_is_scattered_lepton.at(i))
			h->Fill(reco_eta.at(i),reco_phi.at(i));
		    }
		}
	    }
	  else if(strcmp(type,"reco_scattered_electron_eta_phi")==0)
	    {
	      for(unsigned i = 0 ; i < size_reco ; i++)
		{
		  if(reco_charge.at(i)==-1)
		    {
		      if(reco_is_scattered_lepton.at(i))
			h->Fill(reco_eta.at(i),reco_phi.at(i));		    
		    }
		}
	    }
	  else if(strcmp(type,"reco_decay_positron_mom_eta")==0)
	    {
	      for(unsigned i = 0 ; i < size_reco ; i++)
		{
		  if(reco_charge.at(i)==1)
		    h->Fill(reco_eta.at(i),reco_ptotal.at(i));
		}
	    }
	  else if(strcmp(type,"reco_decay_electron_mom_eta")==0)
	    {
	      for(unsigned i = 0 ; i < size_reco ; i++)
		{
		  if(reco_charge.at(i)==-1)
		    {
		      if(!reco_is_scattered_lepton.at(i))
			h->Fill(reco_eta.at(i),reco_ptotal.at(i));
		    }
		}
	    }
	  else if(strcmp(type,"reco_scattered_electron_mom_eta")==0)
	    {
	      for(unsigned i = 0 ; i < size_reco ; i++)
		{
		  if(reco_charge.at(i)==-1)
		    {
		      if(reco_is_scattered_lepton.at(i))
			h->Fill(reco_eta.at(i),reco_ptotal.at(i));
		    }
		}
	    }
	}
    }
  else
    {
      t_truth->SetBranchAddress("geta",&true_eta_pointer);
      t_truth->SetBranchAddress("gphi",&true_phi_pointer);
      t_truth->SetBranchAddress("gptotal",&true_ptotal_pointer);
      t_truth->SetBranchAddress("pid",&pid_pointer);
      t_truth->SetBranchAddress("is_scattered_lepton",&is_scattered_lepton_pointer);
      Int_t nentries = Int_t(t_truth->GetEntries());
      char * type = type_TString.Data();
      for(Int_t entryInChain=0; entryInChain<nentries; entryInChain++)
	{
	  Int_t entryInTree = t_truth->LoadTree(entryInChain);
	  if (entryInTree < 0) break;
	  t_truth->GetEntry(entryInChain);
	  unsigned size_truth = true_eta.size();
	  if(strcmp(type,"true_decay_positron_eta_phi")==0)
	    {
	      for(unsigned i = 0 ; i < size_truth ; i++)
		{
		  if(pid.at(i)==-11)
		    h->Fill(true_eta.at(i),true_phi.at(i));
		}
	    }
	  else if(strcmp(type,"true_decay_electron_eta_phi")==0)
	    {
	      for(unsigned i = 0 ; i < size_truth ; i++)
		{
		  if(pid.at(i)==11)
		    {
		      if(!is_scattered_lepton.at(i))
			h->Fill(true_eta.at(i),true_phi.at(i));
		    }
		}
	    }
	  else if(strcmp(type,"true_scattered_electron_eta_phi")==0)
	    {
	      for(unsigned i = 0 ; i < size_truth; i++)
		{
		  if(pid.at(i)==11)
		    {
		      if(is_scattered_lepton.at(i))
			h->Fill(true_eta.at(i),true_phi.at(i));
		    }
		}
	    }
	  else if(strcmp(type,"true_decay_positron_mom_eta")==0)
	    {
	      for(unsigned i = 0 ; i < size_truth ; i++)
		{
		  if(pid.at(i)=-11)
		    h->Fill(true_eta.at(i),true_ptotal.at(i));
		}
	    }
	  else if(strcmp(type,"true_decay_electron_mom_eta")==0)
	    {
	      for(unsigned i = 0 ; i < size_truth ; i++)
		{
		  if(pid.at(i)=11)
		    {
		      if(!is_scattered_lepton.at(i))
			h->Fill(true_eta.at(i),true_ptotal.at(i));
		    }
		}
	    }
	  else if(strcmp(type,"true_scattered_electron_mom_eta")==0)
	    {
	      for(unsigned i = 0 ; i < size_truth ; i++)
		{
		  if(pid.at(i)=11)
		    {
		      if(is_scattered_lepton.at(i))
			h->Fill(true_eta.at(i),true_ptotal.at(i));
		    }
		}
	    }
	  else if(strcmp(type,"true_scattered_proton_mom_eta")==0)
	    {
	      for(unsigned i = 0 ; i < size_truth ; i++)
		{
		  if(pid.at(i)=2212)
		    h->Fill(true_eta.at(i),true_ptotal.at(i));
		}
	    }
	  else if(strcmp(type,"true_scattered_proton_eta_phi")==0)
	    {
	      for(unsigned i = 0 ; i < size_truth ; i++)
		{
		  if(pid.at(i)=2212)
		    h->Fill(true_eta.at(i),true_phi.at(i));
		}
	    }
	}
    }
}















void hist_to_png(TH2F * h, TString saveTitle, TString type)
{
  saveTitle = type + "_" + saveTitle+".png";
  TCanvas *cPNG = new TCanvas(saveTitle,"",700,500);
  TImage *img = TImage::Create();

  h->Draw("colz");

  img->FromPad(cPNG);
  img->WriteImage(saveTitle);
  delete img;
}
