#ifndef RESONANCE_RATIO_H
#define RESONANCE_RATIO_H

#include "TFile.h"
#include "TH1F.h"

#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooPlot.h"

#include <phool/PHRandomSeed.h>
#include <gsl/gsl_rng.h>

#include "../config/binning.h"
#include "../config/cuts.h"
#include "../util/HistogramTools.h"
#include "../util/DifferentialContainer.h"
#include "../corrections/CorrectionHistogram1D.h"
#include "linearSidebandFit.h"

#include "SidebandFitter.h"

class ResonanceRatio
{
  public:
  ResonanceRatio(int numerator_pdgid, std::string numerator_name, std::string numerator_title,
                 int denominator_pdgid, std::string denominator_name, std::string denominator_title,
                 const HistogramInfo& numerator_massbins, const HistogramInfo& denominator_massbins,
                 const CutSettings& numerator_cuts, const CutSettings& denominator_cuts,
                 TFile* outf, std::string rationame, std::string ratiotitle, float scalefactor, bool blind,
                 std::vector<HistogramInfo> variables, std::vector<std::vector<std::shared_ptr<CorrectionHistogram1D>>> corrections)
  : _numerator_pdgid(numerator_pdgid), _numerator_name(numerator_name), _numerator_title(numerator_title),
    _denominator_pdgid(denominator_pdgid), _denominator_name(denominator_name), _denominator_title(denominator_title),
    _numerator_massbins(numerator_massbins),_denominator_massbins(denominator_massbins),
    _numerator_cuts(numerator_cuts),_denominator_cuts(denominator_cuts),
    outfile(outf), _rationame(rationame), _ratiotitle(ratiotitle), _scalefactor(scalefactor), 
    _blind(blind), _variables(variables), _corrections(corrections)
  {
    setup_yield_histograms();
  }

  // main analysis functions
  void calculate_ratios_unbinned(RooAbsData* numerator_data, RooAbsData* denominator_data);
  void calculate_ratios_binned(TH1F* integrated_numerator_data, std::vector<DifferentialContainer>& diff_numerator_data,
                               TH1F* integrated_denominator_data, std::vector<DifferentialContainer>& diff_denominator_data);

  // yield histograms
  TH1F* numerator_integrated_yield;
  TH1F* denominator_integrated_yield;
  std::vector<TH1F*> numerator_diff_yields;
  std::vector<TH1F*> denominator_diff_yields;

  TH1F* numerator_integrated_fit_syserr;
  TH1F* denominator_integrated_fit_syserr;
  std::vector<TH1F*> numerator_fit_syserr;
  std::vector<TH1F*> denominator_fit_syserr;
  std::vector<TH1F*> ratio_fit_syserr;

  // ratio histograms
  std::vector<TH1F*> integrated_ratio_w_corrections;
  std::vector<std::vector<TH1F*>> diff_ratios_w_corrections;
  // output file
  TFile* outfile;

  protected:
  // workflow functions
  void setup_yield_histograms();
  void get_yield(TH1F* h_yield, int i, RooAbsData* ds, RooRealVar* mass, int pdgid, const CutSettings& cuts, const HistogramInfo& massbins);
  void get_yield_constfit(TH1F* h_yield, TH1F* h_err, int i, RooAbsData* ds, RooRealVar* mass, const CutSettings& cuts, const HistogramInfo& massbins);
  void get_yield_linearsideband(TH1F* h_yield, int i, RooAbsData* ds, RooRealVar* mass, const CutSettings& cuts, const HistogramInfo& massbins, bool use_threshold_background);
  void get_yield_fitterclass(TH1F* h_yield, TH1F* h_err, int i, RooAbsData* ds, RooRealVar* mass, int pdgid, const CutSettings& cuts, const HistogramInfo& massbins);
  void get_diff_yield_unbinned(TH1F* h_yield, TH1F* h_err, const HistogramInfo& diff_variable, RooAbsData* data, int pdgid, const CutSettings& cuts, const HistogramInfo& massbins);
  void get_diff_yield_binned(TH1F* h_yield, TH1F* h_err, const HistogramInfo& diff_variable, DifferentialContainer& data, int pdgid, const CutSettings& cuts, const HistogramInfo& massbins);
  std::string get_corrected_title(std::string current_title, std::string correction_title);
  void calculate_ratios();
  void save_results();

  // fit models
  int _numerator_pdgid;
  int _denominator_pdgid;
  std::string _numerator_name;
  std::string _denominator_name;
  std::string _numerator_title;
  std::string _denominator_title;
  CutSettings _numerator_cuts;
  CutSettings _denominator_cuts;
  HistogramInfo _numerator_massbins;
  HistogramInfo _denominator_massbins;
  // differential variables and corresponding slates of corrections to apply
  std::vector<HistogramInfo> _variables;
  std::vector<std::vector<std::shared_ptr<CorrectionHistogram1D>>> _corrections;
  // naming, scale factor, and blind settings
  std::string _rationame;
  std::string _ratiotitle;
  float _scalefactor;
  bool _blind;
};

void ResonanceRatio::setup_yield_histograms()
{
  for(HistogramInfo& hinfo : _variables)
  {
    numerator_diff_yields.push_back(makeHistogram(_numerator_name+"_yield",_numerator_title+" yield",hinfo));
    denominator_diff_yields.push_back(makeHistogram(_denominator_name+"_yield",_denominator_title+" yield",hinfo));
    numerator_fit_syserr.push_back(makeHistogram(_numerator_name+"_fit_syserr",_numerator_title+" systematic error from mass fit",hinfo));
    denominator_fit_syserr.push_back(makeHistogram(_denominator_name+"_fit_syserr",_denominator_title+" systematic error from mass fit",hinfo));
    ratio_fit_syserr.push_back(makeHistogram("ratio_fit_syserr","systematic error from mass fit procedure",hinfo));
  }

  numerator_integrated_yield = new TH1F(("all_"+_numerator_name+"_yield").c_str(),("All "+_numerator_title+" yield").c_str(),1,0.,1.);
  denominator_integrated_yield = new TH1F(("all_"+_denominator_name+"_yield").c_str(),("All "+_denominator_title+" yield").c_str(),1,0.,1.);

  numerator_integrated_fit_syserr = new TH1F(("all_"+_numerator_name+"_fit_syserr").c_str(),("All "+_numerator_title+" fit systematic error").c_str(),1,0.,1.);
  denominator_integrated_fit_syserr = new TH1F(("all_"+_denominator_name+"_fit_syserr").c_str(),("All "+_denominator_title+" fit systematic error").c_str(),1,0.,1.);

  numerator_integrated_yield->SetTitle((_numerator_massbins.title+";"+_numerator_massbins.axis_label+";Candidates").c_str());
  denominator_integrated_yield->SetTitle((_denominator_massbins.title+";"+_denominator_massbins.axis_label+";Candidates").c_str());
}

void ResonanceRatio::get_yield_constfit(TH1F* h_yield, TH1F* h_err, int i, RooAbsData* ds, RooRealVar* mass, const CutSettings& cuts, const HistogramInfo& massbins)
{
  TH1* ds_h = ds->createHistogram((std::string(h_yield->GetName())+"_"+std::to_string(i)+"_h").c_str(),*mass);
  ds_h->Write();

  RooRealVar const_leftsideband("const_leftsideband","left sideband level",ds->sumEntries(),0.,1e12);
  RooRealVar const_rightsideband("const_rightsideband","right sideband level",ds->sumEntries(),0.,1e12);

  mass->setRange("full",mass->getMin(),mass->getMax());
  mass->setRange("left_sideband",cuts.left_sideband.first,cuts.left_sideband.second);
  mass->setRange("right_sideband",cuts.right_sideband.first,cuts.right_sideband.second);

  RooConstVar dummy_parameter("dummy_par","dummy parameter",1.);

  RooPolynomial uniform_left("uniform_left","uniform left",*mass,RooArgList(dummy_parameter));
  RooPolynomial uniform_right("uniform_right","uniform right",*mass,RooArgList(dummy_parameter));

  RooExtendPdf leftside_model("leftside_model","leftside model",uniform_left,const_leftsideband,"left_sideband");
  RooExtendPdf rightside_model("rightside_model","rightside model",uniform_right,const_rightsideband,"right_sideband");

  leftside_model.fitTo(*ds,RooFit::Range("left_sideband"));
  rightside_model.fitTo(*ds,RooFit::Range("right_sideband"));

  double signal_region_width = cuts.right_sideband.first - cuts.left_sideband.second;
  double left_sideband_width = cuts.left_sideband.second - cuts.left_sideband.first;
  double right_sideband_width = cuts.right_sideband.second - cuts.right_sideband.first;

  double yield = ds->sumEntries() - (const_leftsideband.getVal() + const_rightsideband.getVal())/2.;
  double yield_err = sqrt(ds->sumEntriesW2() + (pow(const_leftsideband.getError(),2.) + pow(const_rightsideband.getError(),2.))/2.);

  std::cout << "total integral: " << ds->sumEntries() << std::endl;

  std::cout << "from fit:" << std::endl;
  std::cout << "left sideband: " << const_leftsideband.getVal() << " +- " << const_leftsideband.getError() << std::endl;
  //std::cout << "bkg yield from left sideband: " << const_leftsideband.getVal()*mass_window_width << " +- " << const_leftsideband.getError()*mass_window_width << std::endl;
  std::cout << "right sideband: " << const_rightsideband.getVal() << " +- " << const_rightsideband.getError() << std::endl;
  //std::cout << "bkg yield from right sideband: " << const_rightsideband.getVal()*mass_window_width << " +- " << const_rightsideband.getError()*mass_window_width << std::endl;
  std::cout << "signal yield: " << yield << " +- " << yield_err << std::endl;

  std::string signalregion_cut = cuts.mother_name + "_mass > " + std::to_string(cuts.left_sideband.second) + " && " + cuts.mother_name + "_mass < " + std::to_string(cuts.right_sideband.first);
  std::string leftsideband_cut = cuts.mother_name + "_mass > " + std::to_string(cuts.left_sideband.first) + " && " + cuts.mother_name + "_mass < " + std::to_string(cuts.left_sideband.second);
  std::string rightsideband_cut = cuts.mother_name + "_mass > " + std::to_string(cuts.right_sideband.first) + " && " + cuts.mother_name + "_mass < " + std::to_string(cuts.right_sideband.second);

  // N = T - (LS/WL + RS/WR)/2.*WS
  // sigma_N^2 = sigma_T^2 + (WS/(2*WL))^2 sigma_LS^2 + (WS/(2*WR))^2 sigma_RS^2
  // poisson process so sigma_T = sqrt(T), sigma_LS = sqrt(LS), sigma_RS = sqrt(RS)
  // sigma_N^2 = T + WS^2/4 (LS/WL^2 + RS/WR^2)

  double total_signalregion_integral = ds->sumEntries(signalregion_cut.c_str());
  double leftsideband_const_fromintegral = ds->sumEntries(leftsideband_cut.c_str());
  double rightsideband_const_fromintegral = ds->sumEntries(rightsideband_cut.c_str());
  double yield_fromintegral = total_signalregion_integral - (leftsideband_const_fromintegral/left_sideband_width + rightsideband_const_fromintegral/right_sideband_width)/2.*signal_region_width;
  double yield_fromintegral_err = sqrt(total_signalregion_integral + (leftsideband_const_fromintegral/pow(left_sideband_width,2.) + rightsideband_const_fromintegral/pow(right_sideband_width,2.))/4.*pow(signal_region_width,2.));

  std::cout << "from integral:" << std::endl;
  std::cout << "signal region integral: " << total_signalregion_integral << " +- " << sqrt(total_signalregion_integral) << std::endl;
  std::cout << "left sideband: " << leftsideband_const_fromintegral*signal_region_width/left_sideband_width << " +- " << sqrt(leftsideband_const_fromintegral)*signal_region_width/left_sideband_width << std::endl;
  std::cout << "right sideband: " << rightsideband_const_fromintegral*signal_region_width/right_sideband_width << " +- " << sqrt(rightsideband_const_fromintegral)*signal_region_width/right_sideband_width << std::endl;
  std::cout << "signal yield: " << yield_fromintegral << " +- " << yield_fromintegral_err << std::endl;

  double syserr = fabs(leftsideband_const_fromintegral/left_sideband_width-rightsideband_const_fromintegral/right_sideband_width)/2.*signal_region_width;

  std::cout << "sys_err from fit: " << syserr << std::endl;

  h_yield->SetBinContent(i,yield_fromintegral);
  h_yield->SetBinError(i,yield_fromintegral_err);

  h_err->SetBinContent(i,syserr/yield_fromintegral);

  std::string name;
  if(i>=0) name = std::string(h_yield->GetName())+"_rooplot_"+std::to_string(i);
  else name = std::string(h_yield->GetName())+"_rooplot";
  std::string title;
  if(i>=0) title = std::string(h_yield->GetTitle())+" bin "+std::to_string(i);
  else title = std::string(h_yield->GetTitle());

  RooPlot* plot = mass->frame(RooFit::Title(title.c_str()));
  plot->SetName(name.c_str());
  ds->plotOn(plot);
  leftside_model.plotOn(plot,RooFit::DrawOption("L"),RooFit::Range("left_sideband"));
  rightside_model.plotOn(plot,RooFit::DrawOption("L"),RooFit::Range("right_sideband"));
  plot->Write();
}

void ResonanceRatio::get_yield_linearsideband(TH1F* h_yield, int i, RooAbsData* ds, RooRealVar* mass, const CutSettings& cuts, const HistogramInfo& massbins, bool use_threshold_background=false)
{
/*
  model.mass->setRange("reco_window",model.left_sideband.first,model.right_sideband.second);
  model.mass->setRange("signal",model.left_sideband.second,model.right_sideband.first);
  model.mass->setRange("left_sideband",model.left_sideband.first,model.left_sideband.second);
  model.mass->setRange("right_sideband",model.right_sideband.first,model.right_sideband.second);

  RooRealVar p0("p0","p0",-10.,10.);
  std::string bkgname = "linear_bkg"+std::string(h_yield->GetName())+std::to_string(i);
  RooChebychev linear_bkg(bkgname.c_str(),"linear_bkg",*model.mass,RooArgList(p0));
  RooRealVar N("N","N",0.,ds->sumEntries());
  RooExtendPdf linear_bkg_ext((bkgname+"_extend").c_str(),"linear_bkg_extend",linear_bkg,N);

  RooFitResult* roofitres = linear_bkg_ext.fitTo(*ds,RooFit::Extended(true),RooFit::Range("left_sideband,right_sideband"),RooFit::Save(true));

  RooAbsReal* bkg_insignal = linear_bkg_ext.createIntegral(*model.mass,RooFit::NormSet(*model.mass),RooFit::Range("signal"));
  double total = ds->reduce(RooFit::CutRange("signal"))->sumEntries();
  double bkg_val = bkg_insignal->getVal()*total;
  double bkg_err = bkg_insignal->getPropagatedError(*roofitres)*total;

  std::cout << "bkg from roofit: " << bkg_val << " +- " << bkg_err << std::endl;
*/

  TH1* ds_h = ds->createHistogram((std::string(h_yield->GetName())+std::to_string(i)).c_str(),*mass);
  double binwidth = ds_h->GetBinWidth(1);
  double min_mass = ds_h->GetXaxis()->GetXmin();
  double max_mass = ds_h->GetXaxis()->GetXmax();
  TF1* sideband_fit;
  TFitResultPtr fitres;
  TF1* full_background;
  if(use_threshold_background)
  {
    double daughter_pt_cutoff = 0.2;
    LinearSidebandThresholdFast background_fit(cuts.mother_name,cuts.track_1_min_pT,i,cuts.left_sideband,cuts.right_sideband);
    sideband_fit = new TF1("sideband_threshold",background_fit,min_mass,max_mass,3);
    sideband_fit->FixParameter(2,0.25);
    fitres = ds_h->Fit(sideband_fit,"RLES");
    background_fit.restrict_to_sidebands = false;
    full_background = sideband_fit;
  }
  else
  {
    sideband_fit = linear_sideband_TF1(min_mass,max_mass,cuts.left_sideband,cuts.right_sideband);
    fitres = ds_h->Fit(sideband_fit,"RLMS");
    full_background = linear_background(sideband_fit);
  }

  double n_background = sideband_fit->Integral(min_mass,max_mass)/binwidth;
  double n_background_err = sideband_fit->IntegralError(min_mass,max_mass,fitres->GetParams(), fitres->GetCovarianceMatrix().GetMatrixArray())/binwidth;

//  std::string signalregion_cut = model.name + "_mass > " + std::to_string(model.left_sideband.second) + " && " + model.name + "_mass < " + std::to_string(model.right_sideband.first);

  double total_counts = ds->sumEntries();
  double total_counts_err = sqrt(total_counts);

  double n_signal = total_counts - n_background;
  double n_signal_err = sqrt(pow(total_counts_err,2)+pow(n_background_err,2));

  std::cout << "total: " << total_counts << " +- " << total_counts_err << std::endl;
  std::cout << "background: " << n_background << " +- " << n_background_err << std::endl;
  std::cout << "signal yield: " << n_signal << " +- " << n_signal_err << std::endl;

  h_yield->SetBinContent(i,n_signal);
  h_yield->SetBinError(i,n_signal_err);

  std::string name;
  if(i>=0) name = std::string(h_yield->GetName())+"_rooplot_"+std::to_string(i);
  else name = std::string(h_yield->GetName())+"_rooplot";
  std::string title;
  if(i>=0) title = std::string(h_yield->GetTitle())+" bin "+std::to_string(i);
  else title = std::string(h_yield->GetTitle());
/*
  RooPlot* plot = model.mass->frame(RooFit::Title(title.c_str()));
  plot->SetName(name.c_str());
  ds->plotOn(plot);
*/

  outfile->cd();
  ds_h->Write();
}

void ResonanceRatio::get_yield_fitterclass(TH1F* h_yield, TH1F* h_err, int i, RooAbsData* ds, RooRealVar* mass, int pdgid, const CutSettings& cuts, const HistogramInfo& massbins)
{
  std::string nametag = std::string(h_yield->GetName())+std::to_string(i);
  SidebandFitter fitter(nametag,pdgid,cuts.left_sideband,cuts.right_sideband,massbins.bins.size()-1);

  fitter.fit_all_orders_toN(ds,mass,cuts.max_background_order);

  double nsignal = fitter.get_best_nsignal();
  double nsignal_err = fitter.get_best_nsignal_err();
  double nbkg = fitter.get_best_nbkg();
  double nbkg_err = fitter.get_best_nbkg_err();
  double yield = nsignal;
  double yield_err = nsignal_err;

  double syserr = fitter.get_yield_syserr();

  std::cout << "nsignal val " << nsignal << std::endl;
  std::cout << "nsignal err " << nsignal_err << std::endl;
  std::cout << "nbkg " << nbkg << std::endl;
  std::cout << "nbkg err " << nbkg_err << std::endl;
  std::cout << "sum entries " << ds->sumEntries() << std::endl;
  std::cout << "yield " << yield << std::endl;
  std::cout << "yield extraction sys. err: " << syserr*100. << "%" << std::endl;

  h_yield->SetBinContent(i,yield);
  h_yield->SetBinError(i,yield_err);

  h_err->SetBinContent(i,syserr);

  std::string name;
  if(i>=0) name = std::string(h_yield->GetName())+"_"+std::to_string(i);
  else name = std::string(h_yield->GetName())+"_";
  std::string title;
  if(i>=0) title = std::string(h_yield->GetTitle())+" bin "+std::to_string(i);
  else title = std::string(h_yield->GetTitle());

  RooPlot* plot = fitter.plot_best_fit(ds,mass);
  plot->SetName(name.c_str());
  plot->SetTitle(title.c_str());
  plot->Write();
}

void ResonanceRatio::get_yield(TH1F* h_yield, int i, RooAbsData* ds, RooRealVar* mass, int pdgid, const CutSettings& cuts, const HistogramInfo& massbins)
{
  mass->setRange("signal",cuts.left_sideband.second,cuts.right_sideband.first);
  mass->setRange("left_sideband",cuts.left_sideband.first,cuts.left_sideband.second);
  mass->setRange("right_sideband",cuts.right_sideband.first,cuts.right_sideband.second);

  double pdg_mass = TDatabasePDG::Instance()->GetParticle(pdgid)->Mass();

  TH1* ds_h = ds->createHistogram((std::string(h_yield->GetName())+std::to_string(i)).c_str(),*mass);

  std::vector<RooRealVar> sideband_parameters;
  sideband_parameters.emplace_back("q1","q1",0.,-10.,10.);
  sideband_parameters.emplace_back("q2","q2",0.,-10.,10.);
  sideband_parameters.emplace_back("q3","q4",0.,-10.,10.);
  sideband_parameters.emplace_back("q4","q4",0.,-10.,10.);
  //sideband_parameters.emplace_back("q5","q5",0.,-10.,10.);
  //sideband_parameters.emplace_back("q6","q6",0.,-1.,1.);
  //sideband_parameters.emplace_back("q7","q7",0.,-1.,1.);
  //sideband_parameters.emplace_back("q8","q8",0.,-1.,1.);
  //sideband_parameters.emplace_back("q9","q9",0.,0.,1.);
  //sideband_parameters.emplace_back("q10","q10",0.5,0.,1.);
  //sideband_parameters.emplace_back("q11","q11",0.5,0.,1.);
  //sideband_parameters.emplace_back("q12","q12",0.5,0.,1.);

/*
  std::vector<RooRealVar> sideband_parameters;
  sideband_parameters.push_back(*model.mass);
  sideband_parameters.emplace_back("sb_logistic_height","logistic_height",0.,-1e6,1e6);
  sideband_parameters.emplace_back("sb_logistic_midpoint","logistic_midpoint",pdg_mass,pdg_mass/2.,pdg_mass*2.);
  sideband_parameters.emplace_back("sb_logistic_width","logistic_width",0.01,1e-4,1e6);
  sideband_parameters.emplace_back("sb_linear_slope","linear_slope",0.,-1e3,1e3);
*/
  std::string sidebandname = "sideband"+std::string(h_yield->GetName())+std::to_string(i);

  //RooGenericPdf sideband(sidebandname.c_str(),sidebandname.c_str(),"x[1]/(1+exp(-x[3]*(x[0]-x[2])))+1.+x[0]*x[4]",RooArgList(sideband_parameters.begin(),sideband_parameters.end()));

  RooChebychev sideband(sidebandname.c_str(),sidebandname.c_str(),*mass,RooArgList(sideband_parameters.begin(),sideband_parameters.end()));
  sideband.fitTo(*ds,RooFit::Range("left_sideband,right_sideband"),RooFit::Minimizer("Minuit2","minimize"),RooFit::Offset(true),RooFit::Strategy(2));

  double signal_window_width = cuts.right_sideband.first - cuts.left_sideband.second;

  RooRealVar mean("mean","mean",pdg_mass,(pdg_mass-0.5*signal_window_width),(pdg_mass+0.5*signal_window_width));
  RooRealVar width1("width1","width1",0.2*signal_window_width,0.01*signal_window_width,1.*signal_window_width);
  RooRealVar width2("width2","width2",0.2*signal_window_width,0.01*signal_window_width,1.*signal_window_width);
  RooRealVar width3("width3","width3",0.2*signal_window_width,0.01*signal_window_width,1.*signal_window_width);

  std::string tag = std::string(h_yield->GetName())+std::to_string(i);
  std::string signalname = "signal"+std::string(h_yield->GetName())+std::to_string(i);

  RooGaussian gaus1(("gaus1"+tag).c_str(),"gaus1",*mass,mean,width1);
  RooGaussian gaus2(("gaus2"+tag).c_str(),"gaus2",*mass,mean,width2);
  RooGaussian gaus3(("gaus3"+tag).c_str(),"gaus3",*mass,mean,width3);

  RooRealVar gaus2_frac(("gaus2_frac"+tag).c_str(),"gaus2_frac",0.01,0.,1.);
  RooRealVar gaus3_frac(("gaus3_frac"+tag).c_str(),"gaus3_frac",0.01,0.,1.);

  RooAddPdf signal(signalname.c_str(),signalname.c_str(),RooArgList(gaus1,gaus2),RooArgList(gaus2_frac));

  std::vector<RooRealVar> background_parameters;
/*
  // adjust RooBernstein coefficients in a way that keeps the normalization intact
  double mass_window_width = model.right_sideband.second - model.left_sideband.first;
  int highest_j_before_signal_region = floor((model.left_sideband.second-model.left_sideband.first)/mass_window_width*sideband_parameters.size());
  int lowest_j_after_signal_region = ceil((model.right_sideband.first-model.left_sideband.second)/mass_window_width*sideband_parameters.size());
  std::cout << "signal region starts after j = " << highest_j_before_signal_region << " and ends before j = " << lowest_j_after_signal_region << std::endl;

  double old_normalization = 0.;
  double new_normalization = 0.;
*/
  for(int j=0;j<sideband_parameters.size();j++)
  {
    std::string name = "k"+std::to_string(j+1);
    double val = sideband_parameters[j].getVal();
    //old_normalization += val;
    //if(j>highest_j_before_signal_region && j<lowest_j_after_signal_region)
    //{
    //  val = 1./2. * (sideband_parameters[highest_j_before_signal_region].getVal() + sideband_parameters[lowest_j_after_signal_region].getVal());
    //}
    //new_normalization += val;
    background_parameters.emplace_back(name.c_str(),name.c_str(),val,(val-0.1),(val+0.1));
    background_parameters[j].setError(sideband_parameters[j].getError());
    background_parameters[j].setConstant();
  }
/*
  for(int j=0;j<sideband_parameters.size();j++)
  {
    double new_val = background_parameters[j].getVal()*(old_normalization/new_normalization);
    background_parameters[j].setVal(new_val);
  }

  std::cout << "new parameters: " << std::endl;
  for(int j=0;j<background_parameters.size();j++)
  {
    std::cout << background_parameters[j].GetName() << " = " << background_parameters[j].getVal() << std::endl;
  }
*/
/*
  background_parameters.emplace_back("k1","k1",0,-1.,1.);
  background_parameters.emplace_back("k2","k2",0.,-1.,1.);
  background_parameters.emplace_back("k3","k3",0.,-1.,1.);
  background_parameters.emplace_back("k4","k4",0.,-1.,1.);
  background_parameters.emplace_back("k5","k5",0.,-1.,1.);
  background_parameters.emplace_back("k6","k6",0.,-1.,1.);
  background_parameters.emplace_back("k7","k7",0.,-1.,1.);
  background_parameters.emplace_back("k8","k8",0.,-1.,1.);
*/
/*
  std::vector<RooRealVar> background_parameters;
  background_parameters.push_back(*model.mass);
  background_parameters.emplace_back("logistic_height","logistic_height",0.,-1e6,1e6);
  background_parameters.emplace_back("logistic_midpoint","logistic_midpoint",pdg_mass,pdg_mass/2.,pdg_mass*2.);
  background_parameters.emplace_back("logistic_width","logistic_width",0.01,1e-4,1e6);
  background_parameters.emplace_back("linear_slope","linear_slope",0.,-1e3,1e3);

  for(int j=0;j<background_parameters.size();j++)
  {
    background_parameters[j].setVal(sideband_parameters[j].getVal());
    background_parameters[j].setError(sideband_parameters[j].getError());
    background_parameters[j].setConstant();
  }
*/
  std::string backgroundname = "background"+std::string(h_yield->GetName())+std::to_string(i);

  //RooGenericPdf background(backgroundname.c_str(),backgroundname.c_str(),"x[1]/(1+exp(-x[3]*(x[0]-x[2])))+1.+x[0]*x[4]",RooArgList(background_parameters.begin(),background_parameters.end()));

  RooChebychev background(backgroundname.c_str(),backgroundname.c_str(),*mass,RooArgList(background_parameters.begin(),background_parameters.end()));

  RooRealVar n_signal(("nsignal_"+tag).c_str(),"nsignal",0.9*ds->sumEntries(),0.,ds->sumEntries());
  RooRealVar n_bkg(("nbkg_"+tag).c_str(),"nbkg",0.1*ds->sumEntries(),0.,ds->sumEntries());

  RooAddPdf full_fit(("full_fit"+tag).c_str(),"full_fit",RooArgList(signal,background),RooArgList(n_signal,n_bkg));
  RooFitResult* full_result = full_fit.fitTo(*ds,RooFit::Minimizer("Minuit2","minimize"),RooFit::InitialHesse(true),RooFit::Strategy(2),RooFit::Offset(true),RooFit::Minos(true),RooFit::Save(true));
/*
  // first, fit background to sidebands
  model.background_function->fitTo(*ds,RooFit::Range("left_sideband,right_sideband"),RooFit::Offset(true));

  // then, hold the background parameters constant and fit over the signal region
  for(auto& param : *(model.background_function->getParameters(*ds)))
  {
    param->setAttribute("Constant",true);
  }

  model.generate_fitFunction();
  model.fit_function->fitTo(*ds,RooFit::Range("signal"));

  double nsignal = model.n_signal->getVal();
  double nsignal_err = model.n_signal->getError();

  double nbkg = model.n_background->getVal();
  double nbkg_err = model.n_background->getError();

  double yield = ds->sumEntries() - nbkg;
  double yield_err = nbkg_err;
*/

  double nsignal = n_signal.getVal();
  double nsignal_err = n_signal.getPropagatedError(*full_result,*mass);
  double nbkg = n_bkg.getVal();
  double nbkg_err = n_bkg.getPropagatedError(*full_result,*mass);
  double yield = n_signal.getVal();
  double yield_err = n_signal.getPropagatedError(*full_result,*mass);
 
  std::cout << "nsignal val " << nsignal << std::endl;
  std::cout << "nsignal err " << nsignal_err << std::endl;
  std::cout << "nbkg " << nbkg << std::endl;
  std::cout << "nbkg err " << nbkg_err << std::endl;
  std::cout << "sum entries " << ds->sumEntries() << std::endl;
  std::cout << "yield " << yield << std::endl;

  h_yield->SetBinContent(i,yield);
  h_yield->SetBinError(i,yield_err);

  std::string name;
  if(i>=0) name = std::string(h_yield->GetName())+"_"+std::to_string(i);
  else name = std::string(h_yield->GetName())+"_";
  std::string title;
  if(i>=0) title = std::string(h_yield->GetTitle())+" bin "+std::to_string(i);
  else title = std::string(h_yield->GetTitle());

  RooPlot* plot = mass->frame(RooFit::Title(title.c_str()));
  plot->SetName(name.c_str());
  ds->plotOn(plot);
  full_fit.plotOn(plot,RooFit::Components(background.GetName()),RooFit::DrawOption("FL"),RooFit::LineStyle(kDashed),RooFit::FillColor(kGray),RooFit::MoveToBack());
  full_fit.plotOn(plot,RooFit::DrawOption("FL"),RooFit::FillColor(kAzure+1),RooFit::MoveToBack());
  plot->Write();
}

void ResonanceRatio::get_diff_yield_unbinned(TH1F* h_yield, TH1F* h_err, const HistogramInfo& diff_variable, RooAbsData* data, int pdgid, const CutSettings& cuts, const HistogramInfo& massbins)
{
  for(int i=1; i<=h_yield->GetNbinsX(); i++)
  {
    std::cout << "==================================================================================" << std::endl;
    std::cout << cuts.mother_name << " " << diff_variable.name << " bin " << i << " of " << h_yield->GetNbinsX() << std::endl;

    std::string selection = diff_variable.get_bin_selection(std::string(data->GetName())+"_"+diff_variable.name,i);
    std::cout << "selection: " << selection << std::endl;

    RooRealVar mass(("diffvar_mass"+diff_variable.name+"_"+std::to_string(i)).c_str(),"mass",massbins.bins.front(),massbins.bins.back());
    RooDataSet* ds_selected = (RooDataSet*)data->reduce(RooArgList(mass),selection.c_str());

    bool use_threshold_background = false;//(hinfo.name == "pT");

    //get_yield_linearsideband(h_yield,i,ds_selected,model,use_threshold_background);
    get_yield_fitterclass(h_yield,h_err,i,ds_selected,&mass,pdgid,cuts,massbins);
  }
}

void ResonanceRatio::get_diff_yield_binned(TH1F* h_yield, TH1F* h_err, const HistogramInfo& diff_variable, DifferentialContainer& data, int pdgid, const CutSettings& cuts, const HistogramInfo& massbins)
{
  for(int i=1; i<=h_yield->GetNbinsX(); i++)
  {
    std::cout << "==================================================================================" << std::endl;
    std::cout << cuts.mother_name << " " << diff_variable.name << " bin " << i << " of " << h_yield->GetNbinsX() << std::endl;
    RooRealVar mass(("diffvar_mass"+diff_variable.name+"_"+std::to_string(i)).c_str(),"mass",massbins.bins.front(),massbins.bins.back());
    RooDataHist dh("binned_massfit","binned_massfit",mass,RooFit::Import(*(data.hists[i])));
    bool use_threshold_background = false;//(hinfo.name == "pT");
    //get_yield_linearsideband(h_yield,i,&dh,model,use_threshold_background);
    get_yield_fitterclass(h_yield,h_err,i,&dh,&mass,pdgid,cuts,massbins);
  }
}

std::string ResonanceRatio::get_corrected_title(std::string current_title,std::string correction_title)
{
  std::string new_title;
  int semicolon_pos = current_title.find(";");
  if(semicolon_pos<current_title.length())
  {
    std::string main_title = current_title.substr(0,current_title.find(";"));
    std::string axis_labels = current_title.substr(current_title.find(";"),current_title.length());
    std::string new_main_title = main_title + ", " + correction_title + " corrected";
    new_title = new_main_title + axis_labels;
  }
  else
  {
    new_title = current_title + ", " + correction_title + " corrected";
  }
  return new_title;
}

void ResonanceRatio::calculate_ratios()
{
  // calculate integrated ratio and apply corrections

  TH1F* integrated_ratio = new TH1F("integrated_ratio",("Integrated "+_rationame).c_str(),1,0.,1.);
  float integrated_ratio_val = numerator_integrated_yield->GetBinContent(1)/denominator_integrated_yield->GetBinContent(1)*_scalefactor;
  float integrated_ratio_err = integrated_ratio_val * sqrt(pow(numerator_integrated_yield->GetBinError(1)/numerator_integrated_yield->GetBinContent(1),2.)+
                                                           pow(denominator_integrated_yield->GetBinError(1)/denominator_integrated_yield->GetBinContent(1),2.));
  integrated_ratio->SetBinContent(1,integrated_ratio_val);
  integrated_ratio->SetBinError(1,integrated_ratio_err);

  integrated_ratio_w_corrections.push_back(integrated_ratio);

  for(std::shared_ptr<CorrectionHistogram1D> correction : _corrections.at(0)) // retrieve one set of corrections to integrate over
  {
    TH1F* current_h = integrated_ratio_w_corrections.back();
    std::string name = std::string(current_h->GetName()) + "_" + correction->name + "corrected";

    std::string current_title = std::string(current_h->GetTitle());
    std::string new_title = get_corrected_title(current_title,correction->title);

    TH1F* new_h = (TH1F*)current_h->Clone(name.c_str());
    new_h->SetTitle(new_title.c_str());

    correction->apply_correction(correction->get_xmin(),correction->get_xmax(),new_h,1);

    integrated_ratio_w_corrections.push_back(new_h); 
  }

  // calculate differential ratios

  for(int ivar=0; ivar<_variables.size(); ivar++)
  {
    TH1F* diffratio = makeHistogram(_rationame,_ratiotitle,_variables[ivar]);
    for(int ibin=1; ibin<=diffratio->GetNbinsX(); ibin++)
    {
      float ratio_val = numerator_diff_yields[ivar]->GetBinContent(ibin) * _scalefactor / denominator_diff_yields[ivar]->GetBinContent(ibin);
      float ratio_err = ratio_val * sqrt(pow(numerator_diff_yields[ivar]->GetBinError(ibin)/numerator_diff_yields[ivar]->GetBinContent(ibin),2.)+pow(denominator_diff_yields[ivar]->GetBinError(ibin)/denominator_diff_yields[ivar]->GetBinContent(ibin),2.));
      diffratio->SetBinContent(ibin,ratio_val);
      diffratio->SetBinError(ibin,ratio_err);
    }
    diff_ratios_w_corrections.push_back({diffratio});
  }

  // apply corrections to differential ratios

  for(int ivar=0; ivar<_variables.size(); ivar++)
  {
    for(int icorr=0; icorr<_corrections[ivar].size(); icorr++)
    {
      TH1F* current_h = diff_ratios_w_corrections[ivar][icorr];
      std::shared_ptr<CorrectionHistogram1D> correction = _corrections[ivar][icorr];
      std::string name = std::string(current_h->GetName()) + "_" + correction->name + "corrected";

      // transforming title is a bit complicated due to axis labels being in the way
      std::string current_title = std::string(current_h->GetTitle());
      std::string new_title = get_corrected_title(current_title,correction->title);

      TH1F* new_h = (TH1F*)current_h->Clone(name.c_str());
      new_h->SetTitle(new_title.c_str());

      for(int ibin=1; ibin<=new_h->GetNbinsX(); ibin++)
      {
        float xlow = new_h->GetBinLowEdge(ibin);
        float xhigh = xlow + new_h->GetBinWidth(ibin);
        std::cout << "ivar " << ivar << " icorr " << icorr << " ibin " << ibin << std::endl;
        std::cout << "before correction: " << new_h->GetBinContent(ibin) << " +- " << new_h->GetBinError(ibin) << std::endl;
        correction->apply_correction(xlow,xhigh,new_h,ibin);
        std::cout << "after " << correction->title << " correction: " << new_h->GetBinContent(ibin) << " +- " << new_h->GetBinError(ibin) << std::endl;
      }
      diff_ratios_w_corrections[ivar].push_back(new_h);
    }
  }

  // calculate systematic error on ratio from mass peak fits

  for(int i=0;i<ratio_fit_syserr.size();i++)
  {
    for(int bin=1;bin<=ratio_fit_syserr[i]->GetNbinsX();bin++)
    {
      double numerator_relative_err = numerator_fit_syserr[i]->GetBinContent(bin);
      double denominator_relative_err = denominator_fit_syserr[i]->GetBinContent(bin);
      ratio_fit_syserr[i]->SetBinContent(bin,_scalefactor * sqrt(pow(numerator_relative_err,2.)+pow(denominator_relative_err,2.)));
    }
  }

  // apply blinding

  if (_blind)
  {
    const uint seed = PHRandomSeed();
    std::unique_ptr<gsl_rng> m_rng;
    m_rng.reset(gsl_rng_alloc(gsl_rng_mt19937));
    gsl_rng_set(m_rng.get(), seed);
    float blind_par = 9.9*gsl_rng_uniform_pos(m_rng.get())+0.1; //Take a value anywhere between 0.1 and 10.

    for(TH1F* h : integrated_ratio_w_corrections)
    {
      float content = h->GetBinContent(1);
      h->SetBinContent(1,content * blind_par);
      h->SetBinError(1,h->GetBinError(1) * blind_par);
    }

    for(std::vector<TH1F*>& v_corr : diff_ratios_w_corrections)
    {
      for(TH1F* h : v_corr)
      {
        for(int ibin=0; ibin<=h->GetNbinsX(); ibin++)
        {
          float content = h->GetBinContent(ibin);
          h->SetBinContent(ibin,content * blind_par);
          h->SetBinError(ibin,h->GetBinError(ibin) * blind_par);
        }
      }
    }
  }
}

void ResonanceRatio::save_results()
{
  // Save everything to file

  outfile->cd();

  for(TH1F* h : numerator_diff_yields)
  {
    //h->Scale(1.,"width");
    h->Write();
  }

  for(TH1F* h : denominator_diff_yields)
  {
    //h->Scale(1.,"width");
    h->Write();
  }

  for(TH1F* h : numerator_fit_syserr)
  {
    h->Write();
  }

  for(TH1F* h : denominator_fit_syserr)
  {
    h->Write();
  }

  for(TH1F* h : ratio_fit_syserr)
  {
    h->Write();
  }

  for(TH1F* h : integrated_ratio_w_corrections)
  {
    h->Write();
  }

  for(std::vector<TH1F*>& v_corr : diff_ratios_w_corrections)
  {
    for(TH1F* h : v_corr)
    {
      h->Write();
    }
  }
}

void ResonanceRatio::calculate_ratios_unbinned(RooAbsData* numerator_data, RooAbsData* denominator_data)
{
  //get_yield_linearsideband(numerator_integrated_yield,-1,numerator_data,_numerator_model);
  //get_yield_linearsideband(denominator_integrated_yield,-1,denominator_data,_denominator_model);

  RooRealVar numerator_integrated_mass("numerator_integrated_mass","numerator integrated mass",_numerator_massbins.bins.front(),_numerator_massbins.bins.back());
  RooRealVar denominator_integrated_mass("denominator_integrated_mass","denominator integrated mass",_denominator_massbins.bins.front(),_denominator_massbins.bins.back());
  
  get_yield_fitterclass(numerator_integrated_yield,numerator_integrated_fit_syserr,-1,numerator_data,&numerator_integrated_mass,_numerator_pdgid,_numerator_cuts,_numerator_massbins);
  get_yield_fitterclass(denominator_integrated_yield,denominator_integrated_fit_syserr,-1,denominator_data,&denominator_integrated_mass,_denominator_pdgid,_denominator_cuts,_denominator_massbins);

  // extract differential yields

  for(size_t i=0; i<_variables.size(); i++)
  {
    std::cout << "======= Differential " << numerator_diff_yields[i]->GetName() << " =======" << std::endl;
    get_diff_yield_unbinned(numerator_diff_yields[i],numerator_fit_syserr[i],_variables[i],numerator_data,_numerator_pdgid,_numerator_cuts,_numerator_massbins);
    std::cout << "======= Differential " << denominator_diff_yields[i]->GetName() << " =======" << std::endl;
    get_diff_yield_unbinned(denominator_diff_yields[i],denominator_fit_syserr[i],_variables[i],denominator_data,_denominator_pdgid,_denominator_cuts,_denominator_massbins);
  }

  calculate_ratios();

  save_results();
}

void ResonanceRatio::calculate_ratios_binned(TH1F* integrated_numerator_data, std::vector<DifferentialContainer>& diff_numerator_data, 
                                             TH1F* integrated_denominator_data, std::vector<DifferentialContainer>& diff_denominator_data)
{
  RooRealVar numerator_mass("numerator_mass","numerator_mass",_numerator_massbins.bins.front(),_numerator_massbins.bins.back());
  RooRealVar denominator_mass("denominator_mass","denominator_mass",_denominator_massbins.bins.front(),_denominator_massbins.bins.back());

  RooDataHist integrated_numerator_dh("integrated_numerator_dh","integrated_numerator_dh",RooArgList(numerator_mass),RooFit::Import(*integrated_numerator_data));
  RooDataHist integrated_denominator_dh("integrated_denominator_dh","integrated_denominator_dh",RooArgList(denominator_mass),RooFit::Import(*integrated_denominator_data));

  get_yield_fitterclass(numerator_integrated_yield,numerator_integrated_fit_syserr,-1,&integrated_numerator_dh,&numerator_mass,_numerator_pdgid,_numerator_cuts,_numerator_massbins);
  get_yield_fitterclass(denominator_integrated_yield,denominator_integrated_fit_syserr,-1,&integrated_denominator_dh,&denominator_mass,_denominator_pdgid,_denominator_cuts,_denominator_massbins);

  for(int i=0; i<_variables.size(); i++)
  {
    get_diff_yield_binned(numerator_diff_yields[i],numerator_fit_syserr[i],_variables[i],diff_numerator_data[i],_numerator_pdgid,_numerator_cuts,_numerator_massbins);
    get_diff_yield_binned(denominator_diff_yields[i],denominator_fit_syserr[i],_variables[i],diff_denominator_data[i],_denominator_pdgid,_denominator_cuts,_denominator_massbins);
  }

  calculate_ratios();

  save_results();
}

#endif
