#include "TFile.h"
#include "TH1F.h"

#include "RooRealVar.h"
#include "RooGaussian.h"
#include "RooVoigtian.h"
#include "RooDataHist.h"
#include "RooPlot.h"

#include "../util/binning.h"
#include "../util/DifferentialContainer.h"

void get_yield(TH1F* h_yield, int i, RooDataSet& ds, RooAddPdf model, RooRealVar& mass, RooRealVar& bkgfrac)
{
  model.fitTo(ds);

  double bkgfrac_fitted = bkgfrac.getVal();
  double bkgfrac_err = bkgfrac.getError();

  std::cout << "val " << bkgfrac_fitted << std::endl;
  std::cout << "err " << bkgfrac_err << std::endl;

  double nSignal = ds.sumEntries()*(1.-bkgfrac_fitted);
  double err_nSignal = ds.sumEntries()*bkgfrac_err;

  std::cout << "bin content " << nSignal << std::endl;
  std::cout << "bin error " << err_nSignal << std::endl;

  h_yield->SetBinContent(i,nSignal);
  h_yield->SetBinError(i,err_nSignal);

  std::string name = std::string(h_yield->GetName())+"_"+std::to_string(i);
  std::string title = std::string(h_yield->GetTitle())+" bin "+std::to_string(i);

  RooPlot* plot = mass.frame(RooFit::Title(title.c_str()));
  plot->SetName(name.c_str());
  ds.plotOn(plot);
  model.plotOn(plot);
  plot->Write();
}

void get_yield_allbins(TH1F* h_yield, HistogramInfo& hinfo, RooDataSet& ds, RooAddPdf model, RooRealVar& mass, RooRealVar& bkgfrac)
{
  for(int i=1; i<=h_yield->GetNbinsX(); i++)
  {
    std::cout << "bin " << i << " of " << h_yield->GetNbinsX() << std::endl;
    std::string selection = hinfo.get_bin_selection(std::string(ds.GetName())+"_"+hinfo.name,i);
    std::cout << "selection: " << selection << std::endl;

    RooDataSet ds_selected = static_cast<RooDataSet&>(*(ds.reduce({mass},selection.c_str())));

    get_yield(h_yield,i,ds_selected,model,mass,bkgfrac);
  }
}

void calculate_ratios_withRooFit()
{
  TFile* Ks_file = TFile::Open("../mass_histograms/Kshort_3runs.root");
  TFile* lambda_file = TFile::Open("../mass_histograms/Lambda_3runs.root");

  TTree* Ks_tree = (TTree*)Ks_file->Get("DecayTree");
  TTree* lambda_tree = (TTree*)lambda_file->Get("DecayTree");
/*
  TH1F* Ks_mass = (TH1F*)Ks_file->Get("Kshort_mass");
  TH1F* lambda_mass = (TH1F*)lambda_file->Get("lambda_mass");

  std::vector<DifferentialContainer> Ks_distributions =
  {
    DifferentialContainer(Ks_file,"K_S0",BinInfo::final_pt_bins),
    DifferentialContainer(Ks_file,"K_S0",BinInfo::final_rapidity_bins),
    DifferentialContainer(Ks_file,"K_S0",BinInfo::final_phi_bins),
    DifferentialContainer(Ks_file,"K_S0",BinInfo::final_ntrack_bins)
  };

  std::vector<DifferentialContainer> lambda_distributions =
  {
    DifferentialContainer(lambda_file,"Lambda0",BinInfo::final_pt_bins),
    DifferentialContainer(lambda_file,"Lambda0",BinInfo::final_rapidity_bins),
    DifferentialContainer(lambda_file,"Lambda0",BinInfo::final_phi_bins),
    DifferentialContainer(lambda_file,"Lambda0",BinInfo::final_ntrack_bins)
  };
*/
  const float maxval = 10.;

  std::vector<HistogramInfo> yield_hinfo =
  {
    BinInfo::final_pt_bins,
    BinInfo::final_eta_bins,
    BinInfo::final_rapidity_bins,
    BinInfo::final_phi_bins,
  };

  std::vector<TH1F*> Ks_yields;
  std::vector<TH1F*> lambda_yields;
  RooArgList Ks_args;
  RooArgList lambda_args;

  RooRealVar m_ks("K_S0_mass","K_S0_mass",0.45,0.55);
  RooRealVar m_lambda("Lambda0_mass","Lambda0_mass",1.1,1.13);

  std::vector<RooRealVar> Ks_vars;
  std::vector<RooRealVar> lambda_vars;

  Ks_args.add(m_ks);
  lambda_args.add(m_lambda);

  for(HistogramInfo& hinfo : yield_hinfo) 
  {
    Ks_yields.push_back(makeHistogram("Ks_yield","K_{S}^{0} yield",hinfo));
    lambda_yields.push_back(makeHistogram("Lambda_yield","#Lambda yield",hinfo));
    std::string Ks_branchname = "K_S0_"+hinfo.name;
    std::string lambda_branchname = "Lambda0_"+hinfo.name;
    std::cout << Ks_branchname << " " << lambda_branchname << std::endl;
    RooRealVar Ks_var(Ks_branchname.c_str(),Ks_branchname.c_str(),hinfo.bins.front(),hinfo.bins.back());
    RooRealVar lambda_var(lambda_branchname.c_str(),lambda_branchname.c_str(),hinfo.bins.front(),hinfo.bins.back());
    Ks_vars.push_back(Ks_var);
    lambda_vars.push_back(lambda_var);
  }

  for(int i=0; i<Ks_vars.size(); i++)
  {
    Ks_args.add(Ks_vars[i]);
    lambda_args.add(lambda_vars[i]);
  }

  Ks_args.Print();
  lambda_args.Print();

  RooDataSet Ks_ds("K_S0","K_S0",Ks_args,RooFit::Import(*Ks_tree));
  RooDataSet lambda_ds("Lambda0","Lambda0",lambda_args,RooFit::Import(*lambda_tree));

  // Kshort signal and background models
  RooRealVar ks_mean("ks_mean","mean",0.49,0.48,0.5);
  RooRealVar ks_width("ks_width","width",0.01,0.0001,0.2);
  RooGaussian ks_signal("ks_signal","signal",m_ks,ks_mean,ks_width);

  RooRealVar ks_lin_coef("ks_lin_coef","lin_coef",0.,0.,1.);
  RooRealVar ks_quad_coef("ks_quad_coef","quad_coef",0.,0.,1.);
  RooRealVar ks_cubic_coef("ks_cubic_coef","cubic_coef",0.,0.,1.);
  RooRealVar ks_quartic_coef("ks_quartic_coef","quartic_coef",0.,0.,1.);
  RooChebychev ks_bkg("ks_bkg","background",m_ks,RooArgList(ks_lin_coef,ks_quad_coef,ks_cubic_coef,ks_quartic_coef));

  //RooRealVar ks_lin_coef("ks_a0","a0",0.,-maxval,maxval);
  //RooRealVar ks_quad_coef("ks_a1","a1",0.,-maxval,maxval);
  //RooRealVar ks_cub_coef("ks_a2","a2",0.,-maxval,maxval);
  //RooPolynomial ks_bkg("ks_bkg","background",m_ks,RooArgList(ks_lin_coef,ks_quad_coef,ks_cub_coef));

  //RooRealVar ks_nsig("ks_nsig","nsignal",100.,0.,1e9);
  //RooRealVar ks_nbkg("ks_nbkg","nbkg",100.,0.,1e9);
  //RooAddPdf ks_massfit("ks_massfit","combined signal and background",RooArgList(ks_bkg,ks_signal),RooArgList(ks_nsig,ks_nbkg));

  RooRealVar ks_bkgfrac("ks_bkgfrac","background fraction",0.01,0.,1.);
  RooAddPdf ks_model("ks_model","combined signal and background",RooArgList(ks_bkg,ks_signal),ks_bkgfrac);

  // Lambda signal and background models
  RooRealVar lambda_mean("lambda_mean","mean",1.113,1.11,1.12);
  RooRealVar lambda_width("lambda_width","width",0.01,0.0001,0.2);
  RooGaussian lambda_signal("lambda_signal","signal",m_lambda,lambda_mean,lambda_width);

  RooRealVar lambda_lin_coef("lambda_lin_coef","lin_coef",0.,0.,1.);
  RooRealVar lambda_quad_coef("lambda_quad_coef","quad_coef",0.,0.,1.);
  RooRealVar lambda_cubic_coef("lambda_cubic_coef","cubic_coef",0.,0.,1.);
  RooRealVar lambda_quartic_coef("lambda_quartic_coef","quartic_coef",0.,0.,1.);
  RooChebychev lambda_bkg("lambda_bkg","background",m_lambda,RooArgList(lambda_lin_coef,lambda_quad_coef,lambda_cubic_coef));

  //RooRealVar lambda_lin_coef("lambda_a0","a0",1.,-maxval,maxval);
  //RooRealVar lambda_quad_coef("lambda_a1","a1",0.1,-maxval,maxval);
  //RooRealVar lambda_cub_coef("lambda_a2","a2",0.01,-maxval,maxval);
  //RooPolynomial lambda_bkg("lambda_bkg","background",m_lambda,RooArgList(lambda_lin_coef,lambda_quad_coef,lambda_cub_coef));

  //RooRealVar lambda_bkg_frac("lambda_bkg_frac","background fraction",0.5,0.,1.);
  //RooAddPdf lambda_massfit("lambda_massfit","combined signal and background",RooArgList(lambda_bkg,lambda_signal),lambda_bkg_frac);

  RooRealVar lambda_bkgfrac("lambda_bkgfrac","background fraction",0.1,0.,1.);
  RooAddPdf lambda_model("lambda_model","combined signal and background",RooArgList(lambda_bkg,lambda_signal),lambda_bkgfrac);
  /*
  // Import histograms as RooDataHists
  RooDataHist ks_hist("ks_hist","ks_hist",m_ks,RooFit::Import(*Ks_mass));
  RooDataHist lambda_hist("lambda_hist","lambda_hist",m_lambda,RooFit::Import(*lambda_mass));
  */

  TFile* fout = new TFile("fits.root","RECREATE");

  for(size_t i=0; i<yield_hinfo.size(); i++)
  {
    std::cout << "======= Differential " << Ks_yields[i]->GetName() << " =======" << std::endl;
    get_yield_allbins(Ks_yields[i],yield_hinfo[i],Ks_ds,ks_model,m_ks,ks_bkgfrac);
    std::cout << "======= Differential " << lambda_yields[i]->GetName() << " =======" << std::endl;
    get_yield_allbins(lambda_yields[i],yield_hinfo[i],lambda_ds,lambda_model,m_lambda,lambda_bkgfrac);
  }

  TH1F* lambdaKsratio = new TH1F("integrated_lambdaKs_ratio","Integrated #Lambda/K_{S}^{0} Ratio",1,0.,1.);

  std::vector<TH1F*> lambdaKs_diffratios;
  for(HistogramInfo& hinfo : yield_hinfo) lambdaKs_diffratios.push_back(makeHistogram("lambdaKsratio","#Lambda/K_{S}^{0} Ratio",hinfo));

  for(size_t i=0; i<yield_hinfo.size(); i++)
  {
    lambdaKs_diffratios[i]->Divide(lambda_yields[i],Ks_yields[i],1.,0.5);
  }

  for(size_t i=0; i<yield_hinfo.size(); i++)
  {
    //phi_yields[i]->Write();
    Ks_yields[i]->Write();
    lambda_yields[i]->Write();

    //phiKs_diffratios[i]->Write();
    lambdaKs_diffratios[i]->Write();
  }

/*
  // Do the fit
  ks_model.fitTo(ks_hist);
  lambda_model.fitTo(lambda_hist);

  TCanvas* ck = new TCanvas("ck","ck",600,600);
  RooPlot* ks_plot = m_ks.frame(RooFit::Title("Kshort mass"));
  ks_hist.plotOn(ks_plot);
  ks_model.plotOn(ks_plot);
  ks_plot->Draw();

  TCanvas* cl = new TCanvas("cl","cl",600,600);
  RooPlot* lambda_plot = m_lambda.frame(RooFit::Title("Lambda mass"));
  lambda_hist.plotOn(lambda_plot);
  lambda_model.plotOn(lambda_plot);
  lambda_plot->Draw();
*/
}
