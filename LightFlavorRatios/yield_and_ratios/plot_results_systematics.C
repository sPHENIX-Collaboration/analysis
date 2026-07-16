void plot_results_systematics()
{

  // get systematics from the component histograms
  //======================================  
  std::vector<std::string> fin;
  std::vector<std::string> hname;

  // daughter track pT cut systematic
  fin.push_back("/sphenix/tg/tg01/hf/gregoryottino/lightFlavorPpg16/analysis/LightFlavorRatios/systematic_uncertainty/SystematicDetermination_output_ptmin.root");
  hname.push_back("hDifference");

  // pT smearing systematic
  fin.push_back("/sphenix/tg/tg01/hf/gregoryottino/lightFlavorPpg16/analysis/LightFlavorRatios/systematic_uncertainty/SystematicDetermination_output_ptsmear.root");
  hname.push_back("hDifference");

  // cuts efficiency systematic
  fin.push_back("/sphenix/user/mjpeters/analysis/LightFlavorRatios/swimming_correction/LamdbaKsCutEfficiency_200MeV_hists.root");
  hname.push_back("cuteff_syserr_pT");

  // The peak extraction systematic
  fin.push_back("/sphenix/user/mjpeters/analysis/LightFlavorRatios/yield_and_ratios/fits.root");
  hname.push_back("ratio_fit_syserr_vspT");  

  int nsys = fin.size();
  std::cout << " there are " << fin.size() << " systematics input files " << std::endl;  

  TH1F *hsys[nsys];
  int nbinsmax = 0;
  int nfilemax = -1;
  
  for(int i=0;i<nsys;++i)
    {
      TFile *f = new TFile(fin[i].c_str());
      if(!f)
	{
	  std::cout << "Failed to open file named " << fin[i] << std::endl;
	  return;
	}
      f->GetObject(hname[i].c_str(), hsys[i]);
      if(!hsys[i])
	{
	  std::cout << "Failed to get histogram " << hname[i] << " from file " << fin[i] << std::endl;
	  return;
	}
      std::cout << " file " << i << " named " << fin[i] << " has histogram " << hname[i] << " with " << hsys[i]->GetNbinsX()  << " bins " << std::endl;
      if(hsys[i]->GetNbinsX() > nbinsmax)
	{
	  nbinsmax=hsys[i]->GetNbinsX();
	  nfilemax = i;
	}
    }
  std::cout << std::endl << " nbinsmax is " << nbinsmax << " for " << fin[nfilemax] << std::endl;

  double x[nbinsmax];
  for(int j = 0; j<nbinsmax; ++j)
    {
      x[j] = hsys[nfilemax]->GetBinCenter(j+1);      
    }

  double syscum[nbinsmax];
  for(int i = 0; i < nsys; ++i)  // histograms
    {
      for(int k=0; k < hsys[i]->GetNbinsX();++k)  // bins
	{
	  double xd=hsys[i]->GetBinLowEdge(k+1);
	  double xu=xd+hsys[i]->GetBinWidth(k+1);
	  double sys = hsys[i]->GetBinContent(k+1);
	  // decide which of the fine bins this goes into
	  for(int j=0; j<nbinsmax;++j)
	    {
	      if(x[j] > xd && x[j] < xu)
		{
		  syscum[k] += sys*sys;
		  std::cout << " file " << i << " file bin " << k << " fine bin " << j << " sys " << sys << " syscum " << syscum[k] << std::endl;
		}
	    }
	}		
    }

  // in addition we need to add a systematic for the tracking inefficiency correction to the ratio
  // we agreed on 3% flat in pT as a conservative estimate
  float track_inefficiency_percent = 0.03;
  for(int i=0;i<nbinsmax;++i)
    {
      syscum[i] += track_inefficiency_percent * track_inefficiency_percent;
      //  std::cout << " i " << i << " track_inefficiency_percent " << track_inefficiency_percent << " syscum is now " << syscum[i] << std::endl;
    }
  
  // get the ratios
  //===========  
  TFile* f = TFile::Open("/sphenix/user/mjpeters/analysis/LightFlavorRatios/yield_and_ratios/fits.root");
  if(!f)
    {
      std::cout << "Failed to open the file fits.root" << std::endl;
      return;
    }

  TH1F *ratio_hist_pt;
  f->GetObject("lambdaKsratio_vspT_lambdafeeddowncorrected_effcorrected_geoacceptancecorrected_cutefficiencycorrected",ratio_hist_pt);
  double plotmax = ratio_hist_pt->GetMaximum() * 1.3;

  ratio_hist_pt->SetMinimum(0);
  ratio_hist_pt->Draw();

  // convert ratio histogram to a TGraphMultiErrors
  // extract the arrays
  int nbins = ratio_hist_pt->GetNbinsX();
  float pt[nbins];
  float ptelo[nbins];
  float ptehi[nbins];
  float ratio[nbins];
  float ratiostat[nbins];
  for (int ipt=0;ipt<nbins;++ipt)
  {
    pt[ipt] = ratio_hist_pt->GetXaxis()->GetBinCenter(ipt+1);
    float ptlo = ratio_hist_pt->GetXaxis()->GetBinLowEdge(ipt+1);
    float ptw = ratio_hist_pt->GetXaxis()->GetBinWidth(ipt+1);
    ptelo[ipt] = pt[ipt] - ptlo;
    ptehi[ipt] = ptlo + ptw - pt[ipt];

    ratio[ipt] = ratio_hist_pt->GetBinContent(ipt+1);
    ratiostat[ipt] = ratio_hist_pt->GetBinError(ipt+1);
  }
  
  // full systematic errors
  //=================
  double esyslo[nbinsmax];
  double esyshi[nbinsmax];
  for(int i=0;i<nbinsmax;++i)
    {
      esyslo[i] = ratio[i]*sqrt(syscum[i]);
      esyshi[i] = ratio[i]*sqrt(syscum[i]);
      std::cout << " i " << i << " pT " << pt[i] << " ratio " << ratio[i] << " syscum " << syscum[i] << " sqrt(syscum) " << sqrt(syscum[i]) << " esyslo " << esyslo[i] << " esyshi " << esyshi[i] << std::endl;      
    }

  // plot the ratio vs pT
  //===============
  TCanvas *c=new TCanvas("c","corrected double ratio plot", 50, 50, 800,600);
  gPad->SetBottomMargin(0.12);
  gPad->SetLeftMargin(0.12);
  
TGraphMultiErrors *gr = new TGraphMultiErrors("gr","Ratio vs pT", nbins,pt,ratio,ptelo,ptehi,ratiostat,ratiostat);
 gr->SetName("ratio_pt_sys");
 gr->SetMinimum(0);
 gr->SetMaximum(plotmax);
 gr->SetMarkerStyle(20);
 gr->GetXaxis()->SetRangeUser(0,5.0);

 gr->AddYError(nbins,esyslo,esyshi);
 gr->SetMarkerStyle(22);
 gr->SetMarkerSize(1.5);
 gr->SetLineColor(kBlack);
 gr->GetAttLine(0)->SetLineColor(kBlack);
 gr->GetAttLine(1)->SetLineColor(kBlue);
 gr->GetAttFill(1)->SetFillStyle(0);

 gr->GetXaxis()->SetTitle("p_{T} (Gev)");
 gr->GetYaxis()->SetTitle("#Lambda^{0}/2K_{s}^0 (arbitrary units)");

 gr->Draw("APS ; Z ; 5 s=0.5");

 c->SaveAs("ratio_allsyst.pdf");

 TFile *fout = new TFile("temp_ratio_results_systematic.root","recreate");
 gr->Write();
 fout->Close();


 
}
