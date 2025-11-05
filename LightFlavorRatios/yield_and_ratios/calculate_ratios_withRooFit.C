#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

#include "RooRealVar.h"
#include "RooGaussian.h"
#include "RooVoigtian.h"
#include "RooDataHist.h"
#include "RooPlot.h"


void calculate_ratios()
{
  TFile* Ks_file = TFile::Open("../mass_histograms/data_Kshort_mass.root");
  TFile* phi_file = TFile::Open("../mass_histograms/data_phi_mass.root");
  TFile* lambda_file = TFile::Open("../mass_histograms/data_lambda_mass.root");

  TH1F* Ks_mass = (TH1F*)Ks_file->Get("Kshort_mass");
  TH2F* Ks_massvspt = (TH2F*)Ks_file->Get("Kshort_mass_vspt");
  TH2F* Ks_massvsntrk = (TH2F*)Ks_file->Get("Kshort_mass_vsntrk");

  TH1F* phi_mass = (TH1F*)phi_file->Get("phi_mass");
  TH2F* phi_massvspt = (TH2F*)phi_file->Get("phi_mass_vspt");
  TH2F* phi_massvsntrk = (TH2F*)phi_file->Get("phi_mass_vsntrk");

  TH1F* lambda_mass = (TH1F*)lambda_file->Get("lambda_mass");
  TH2F* lambda_massvspt = (TH2F*)lambda_file->Get("lambda_mass_vspt");
  TH2F* lambda_massvsntrk = (TH2F*)lambda_file->Get("lambda_mass_vsntrk");


  // Kshort signal and background models
  RooRealVar m_ks("m_ks","m",0.4,0.6);

  RooRealVar ks_mean("ks_mean","mean",0.5);
  RooRealVar ks_width("ks_width","width",0.1,0.,10.);
  RooGaussian ks_signal("ks_signal","signal",m_ks,ks_mean,ks_width);

  RooRealVar ks_const_coef("ks_a0","a0",100.,0.,1e9);
  RooRealVar ks_lin_coef("ks_a1","a1",0.5,-10.,10.);
  RooRealVar ks_quad_coef("ks_a2","a2",0.1,-10.,10.);
  RooPolynomial ks_bkg("ks_bkg","background",m_ks,RooArgList(ks_const_coef,ks_lin_coef,ks_quad_coef));

  RooRealVar ks_nsig("ks_nsig","nsignal",100.,0.,1e9);
  RooRealVar ks_nbkg("ks_nbkg","nbkg",100.,0.,1e9);
  RooAddPdf ks_massfit("ks_massfit","combined signal and background",RooArgList(ks_bkg,ks_signal),RooArgList(ks_nsig,ks_nbkg));

  // phi signal and background models
  RooRealVar m_phi("m_phi","m",0.95,1.1);

  RooRealVar phi_mean("phi_mean","mean",1.02);
  RooRealVar phi_sigma("phi_sigma","sigma",0.1,0.,10.);
  RooRealVar phi_width("phi_width","width",0.01,0.,10.);
  RooVoigtian phi_signal("phi_signal","signal",m_phi,phi_mean,phi_width,phi_sigma);

  RooRealVar phi_const_coef("phi_a0","a0",100.,0.,1e9);
  RooRealVar phi_lin_coef("phi_a1","a1",0.5,-10.,10.);
  RooRealVar phi_quad_coef("phi_a2","a2",0.1,-10.,10.);
  RooPolynomial phi_bkg("phi_bkg","background",m_phi,RooArgList(phi_const_coef,phi_lin_coef,phi_quad_coef));

  RooRealVar phi_bkg_frac("ks_bkg_frac","background fraction",0.5,0.,1.);
  RooAddPdf phi_massfit("ks_massfit","combined signal and background",RooArgList(phi_bkg,phi_signal),phi_bkg_frac);

  // Lambda signal and background models
  RooRealVar m_lambda("m_lambda","m",1.05,1.2);

  RooRealVar lambda_mean("lambda_mean","mean",0.5);
  RooRealVar lambda_width("lambda_width","width",0.1,0.,10.);
  RooGaussian lambda_signal("lambda_signal","signal",m_lambda,lambda_mean,lambda_width);

  RooRealVar lambda_const_coef("lambda_a0","a0",100,0.,1e9);
  RooRealVar lambda_lin_coef("lambda_a1","a1",0.5,-10.,10.);
  RooRealVar lambda_quad_coef("lambda_a2","a2",0.1,-10.,10.);
  RooPolynomial lambda_bkg("lambda_bkg","background",m_lambda,RooArgList(lambda_const_coef,lambda_lin_coef,lambda_quad_coef));

  RooRealVar lambda_bkg_frac("lambda_bkg_frac","background fraction",0.5,0.,1.);
  RooAddPdf lambda_massfit("lambda_massfit","combined signal and background",RooArgList(lambda_bkg,lambda_signal),lambda_bkg_frac);
  
  // Import histograms as RooDataHists
  RooDataHist ks_hist("ks_hist","ks_hist",m_ks,RooFit::Import(*Ks_mass));
  RooDataHist phi_hist("phi_hist","phi_hist",m_phi,RooFit::Import(*phi_mass));
  RooDataHist lambda_hist("lambda_hist","lambda_hist",m_lambda,RooFit::Import(*lambda_mass));

  // Do the fit
  ks_massfit.fitTo(ks_hist);
  //phi_massfit.fitTo(phi_hist);
  //lambda_massfit.fitTo(lambda_hist);

  RooPlot* ks_plot = m_ks.frame(RooFit::Title("Kshort mass"));
  ks_hist.plotOn(ks_plot);
  ks_massfit.plotOn(ks_plot);

}
