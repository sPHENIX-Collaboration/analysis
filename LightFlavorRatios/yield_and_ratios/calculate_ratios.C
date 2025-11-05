#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

#include "../util/binning.h"

void get_phiyield(TH1F* h_yield, TH1D* h_mass, float xval)
{
  TF1* phi_massfit = new TF1("phi_fit","[0]*TMath::Voigt(x-[1],[2],[3])+pol1(4)",1.,1.1);
  phi_massfit->SetParameter(0,1000.);
  phi_massfit->SetParameter(1,1.02);
  phi_massfit->SetParameter(2,0.01);
  phi_massfit->SetParameter(3,0.01);
  phi_massfit->SetParameter(4,1000.);
  phi_massfit->SetParameter(5,-0.);

  h_mass->Fit(phi_massfit,"R");

  TF1* phi_signal = new TF1("phi_signal","[0]*TMath::Voigt(x-[1],[2],[3])",1.,1.1);
  phi_signal->SetParameter(0,phi_massfit->GetParameter(0));
  phi_signal->SetParError(0,phi_massfit->GetParError(0));
  phi_signal->SetParameter(1,phi_massfit->GetParameter(1));
  phi_signal->SetParError(1,phi_massfit->GetParError(1));
  phi_signal->SetParameter(2,phi_massfit->GetParameter(2));
  phi_signal->SetParError(2,phi_massfit->GetParError(2));
  phi_signal->SetParameter(3,phi_massfit->GetParameter(3));
  phi_signal->SetParError(3,phi_massfit->GetParError(3));

  float phi_yield = phi_signal->Integral(0.95,1.1)/h_mass->GetBinWidth(2);
  float phi_yielderror = phi_signal->IntegralError(0.95,1.1)/h_mass->GetBinWidth(2);

  h_yield->SetBinContent(h_yield->FindBin(xval),phi_yield);
  h_yield->SetBinError(h_yield->FindBin(xval),phi_yielderror);
  delete phi_massfit;
  delete phi_signal;
}

void get_Ksyield(TH1F* h_yield, TH1D* h_mass, float xval)
{
  TF1* ks_massfit = new TF1("ks_fit","gaus(0)+pol1(3)",0.4,0.6);
  ks_massfit->SetParameter(0,1000.);
  ks_massfit->SetParameter(1,0.5);
  ks_massfit->SetParameter(2,0.1);
  ks_massfit->SetParameter(3,100.);
  ks_massfit->SetParameter(4,-0.1);

  h_mass->Fit(ks_massfit,"R");

  TF1* ks_signal = new TF1("ks_signal","gaus(0)",0.4,0.6);
  ks_signal->SetParameter(0,ks_massfit->GetParameter(0));
  ks_signal->SetParError(0,ks_massfit->GetParError(0));
  ks_signal->SetParameter(1,ks_massfit->GetParameter(1));
  ks_signal->SetParError(1,ks_massfit->GetParError(1));
  ks_signal->SetParameter(2,ks_massfit->GetParameter(2));
  ks_signal->SetParError(2,ks_massfit->GetParError(2));

  float ks_yield = ks_signal->Integral(0.4,0.6)/h_mass->GetBinWidth(2);
  float ks_yielderror = ks_signal->IntegralError(0.4,0.6)/h_mass->GetBinWidth(2);

  h_yield->SetBinContent(h_yield->FindBin(xval),ks_yield);
  h_yield->SetBinError(h_yield->FindBin(xval),ks_yielderror);
  delete ks_massfit;
  delete ks_signal;
}

void get_lambdayield(TH1F* h_yield, TH1D* h_mass, float xval)
{
  TF1* lambda_massfit = new TF1("lambda_fit","gaus(0)+pol2(3)",1.08,1.2);
  lambda_massfit->SetParameter(0,1000.);
  lambda_massfit->SetParameter(1,1.11);
  lambda_massfit->SetParameter(2,0.1);
  lambda_massfit->SetParameter(3,100.);
  lambda_massfit->SetParameter(4,0.1);
  lambda_massfit->SetParameter(5,-0.1);

  h_mass->Fit(lambda_massfit,"R");

  TF1* lambda_signal = new TF1("lambda_signal","gaus(0)",0.4,0.6);
  lambda_signal->SetParameter(0,lambda_massfit->GetParameter(0));
  lambda_signal->SetParError(0,lambda_massfit->GetParError(0));
  lambda_signal->SetParameter(1,lambda_massfit->GetParameter(1));
  lambda_signal->SetParError(1,lambda_massfit->GetParError(1));
  lambda_signal->SetParameter(2,lambda_massfit->GetParameter(2));
  lambda_signal->SetParError(2,lambda_massfit->GetParError(2));

  float lambda_yield = lambda_signal->Integral(1.05,1.2)/h_mass->GetBinWidth(2);
  float lambda_yielderror = lambda_signal->IntegralError(1.05,1.2)/h_mass->GetBinWidth(2);

  h_yield->SetBinContent(h_yield->FindBin(xval),lambda_yield);
  h_yield->SetBinError(h_yield->FindBin(xval),lambda_yielderror);
  delete lambda_massfit;
  delete lambda_signal;
}

void calculate_ratios()
{
  gStyle->SetOptStat(1);

  TFile* Ks_file = TFile::Open("../mass_histograms/data_Kshort_mass.root");
  TFile* phi_file = TFile::Open("../mass_histograms/data_phi_mass.root");
  TFile* lambda_file = TFile::Open("../mass_histograms/data_lambda_mass.root");

  TH1D* Ks_mass = (TH1D*)Ks_file->Get("Kshort_mass");
  TH2F* Ks_massvspt = (TH2F*)Ks_file->Get("Kshort_mass_vspt");
  TH2F* Ks_massvsntrk = (TH2F*)Ks_file->Get("Kshort_mass_vsntrk");
  TH2F* Ks_massvsy = (TH2F*)Ks_file->Get("Kshort_mass_vsy");
  TH2F* Ks_massvsphi = (TH2F*)Ks_file->Get("Kshort_mass_vsphi");

  TH1D* phi_mass = (TH1D*)phi_file->Get("phi_mass");
  TH2F* phi_massvspt = (TH2F*)phi_file->Get("phi_mass_vspt");
  TH2F* phi_massvsntrk = (TH2F*)phi_file->Get("phi_mass_vsntrk");
  TH2F* phi_massvsy = (TH2F*)phi_file->Get("phi_mass_vsy");
  TH2F* phi_massvsphi = (TH2F*)phi_file->Get("phi_mass_vsphi");

  TH1D* lambda_mass = (TH1D*)lambda_file->Get("lambda_mass");
  TH2F* lambda_massvspt = (TH2F*)lambda_file->Get("lambda_mass_vspt");
  TH2F* lambda_massvsntrk = (TH2F*)lambda_file->Get("lambda_mass_vsntrk");
  TH2F* lambda_massvsy = (TH2F*)lambda_file->Get("lambda_mass_vsy");
  TH2F* lambda_massvsphi = (TH2F*)lambda_file->Get("lambda_mass_vsphi");

  TH1F* integrated_phi_yield = new TH1F("integrated_phi_yield","Integrated #{phi} yield",1,0.,1.);
  TH1F* integrated_Ks_yield = new TH1F("integrated_Ks_yield","Integrated K_{s}^{0} yield",1,0.,1.);
  TH1F* integrated_lambda_yield = new TH1F("integrated_lambda_yield","Integrated #{Lambda} yield",1,0.,1.);

  TH1F* phi_yield_vspt = makeHistogram("phi_yield","#{phi} yield",BinInfo::final_pt_bins);
  TH1F* Ks_yield_vspt = makeHistogram("Ks_yield","K_{s}^{0} yield",BinInfo::final_pt_bins);
  TH1F* lambda_yield_vspt = makeHistogram("lambda_yield","#{Lambda} yield",BinInfo::final_pt_bins);

  TH1F* phi_yield_vsy = makeHistogram("phi_yield","#{phi} yield",BinInfo::final_rapidity_bins);
  TH1F* Ks_yield_vsy = makeHistogram("Ks_yield","K_{s}^{0} yield",BinInfo::final_rapidity_bins);
  TH1F* lambda_yield_vsy = makeHistogram("lambda_yield","#{Lambda} yield",BinInfo::final_rapidity_bins);

  TH1F* phi_yield_vsphi = makeHistogram("phi_yield","#{phi} yield",BinInfo::final_phi_bins);
  TH1F* Ks_yield_vsphi = makeHistogram("Ks_yield","K_{s}^{0} yield",BinInfo::final_phi_bins);
  TH1F* lambda_yield_vsphi = makeHistogram("lambda_yield","#{Lambda} yield",BinInfo::final_phi_bins);

  TH1F* phi_yield_vsntrack = makeHistogram("phi_yield","#{phi} yield",BinInfo::final_ntrack_bins);
  TH1F* Ks_yield_vsntrack = makeHistogram("Ks_yield","K_{s}^{0} yield",BinInfo::final_ntrack_bins);
  TH1F* lambda_yield_vsntrack = makeHistogram("lambda_yield","#{Lambda} yield",BinInfo::final_ntrack_bins);

  get_phiyield(integrated_phi_yield,phi_mass,0.5);
  get_Ksyield(integrated_Ks_yield,Ks_mass,0.5);
  get_lambdayield(integrated_lambda_yield,lambda_mass,0.5);

  for(int i=1;i<=BinInfo::pt_bins.nBins;i++)
  {
    TH1D* phi_mass_slice = phi_massvspt->ProjectionX("phi_slice",i,i+1,"e");
    TH1D* Ks_mass_slice = Ks_massvspt->ProjectionX("Ks_slice",i,i+1,"e");
    TH1D* lambda_mass_slice = lambda_massvspt->ProjectionX("lambda_slice",i,i+1,"e");

    get_phiyield(phi_yield_vspt,phi_mass_slice,phi_yield_vspt->GetBinCenter(i));
    get_Ksyield(Ks_yield_vspt,Ks_mass_slice,Ks_yield_vspt->GetBinCenter(i));
    get_lambdayield(lambda_yield_vspt,lambda_mass_slice,lambda_yield_vspt->GetBinCenter(i));
    delete phi_mass_slice;
    delete Ks_mass_slice;
    delete lambda_mass_slice;
  }

  for(int i=1;i<=BinInfo::rapidity_bins.nBins;i++)
  {
    TH1D* phi_mass_slice = phi_massvsy->ProjectionX("phi_slice",i,i+1,"e");
    TH1D* Ks_mass_slice = Ks_massvsy->ProjectionX("Ks_slice",i,i+1,"e");
    TH1D* lambda_mass_slice = lambda_massvsy->ProjectionX("lambda_slice",i,i+1,"e");

    get_phiyield(phi_yield_vsy,phi_mass_slice,phi_yield_vsy->GetBinCenter(i));
    get_Ksyield(Ks_yield_vsy,Ks_mass_slice,Ks_yield_vsy->GetBinCenter(i));
    get_lambdayield(lambda_yield_vsy,lambda_mass_slice,lambda_yield_vsy->GetBinCenter(i));
    delete phi_mass_slice;
    delete Ks_mass_slice;
    delete lambda_mass_slice;
  }

  for(int i=1;i<=BinInfo::phi_bins.nBins;i++)
  {
    TH1D* phi_mass_slice = phi_massvsphi->ProjectionX("phi_slice",i,i+1,"e");
    TH1D* Ks_mass_slice = Ks_massvsphi->ProjectionX("Ks_slice",i,i+1,"e");
    TH1D* lambda_mass_slice = lambda_massvsphi->ProjectionX("lambda_slice",i,i+1,"e");

    get_phiyield(phi_yield_vsphi,phi_mass_slice,phi_yield_vsphi->GetBinCenter(i));
    get_Ksyield(Ks_yield_vsphi,Ks_mass_slice,Ks_yield_vsphi->GetBinCenter(i));
    get_lambdayield(lambda_yield_vsphi,lambda_mass_slice,lambda_yield_vsphi->GetBinCenter(i));
    delete phi_mass_slice;
    delete Ks_mass_slice;
    delete lambda_mass_slice;
  }

  for(int i=1;i<=BinInfo::ntrack_bins.nBins;i++)
  {
    TH1D* phi_mass_slice = phi_massvsntrk->ProjectionX("phi_slice",i,i+1,"e");
    TH1D* Ks_mass_slice = Ks_massvsntrk->ProjectionX("Ks_slice",i,i+1,"e");
    TH1D* lambda_mass_slice = lambda_massvsntrk->ProjectionX("lambda_slice",i,i+1,"e");

    get_phiyield(phi_yield_vsntrack,phi_mass_slice,phi_yield_vsntrack->GetBinCenter(i));
    get_Ksyield(Ks_yield_vsntrack,Ks_mass_slice,Ks_yield_vsntrack->GetBinCenter(i));
    get_lambdayield(lambda_yield_vsntrack,lambda_mass_slice,lambda_yield_vsntrack->GetBinCenter(i));
    delete phi_mass_slice;
    delete Ks_mass_slice;
    delete lambda_mass_slice;
  }

  TH1F* lambdaKsratio = new TH1F("integrated_lambdaKs_ratio","Integrated #{Lambda}/K_{s}^{0} Ratio",1,0.,1.);
  TH1F* phiKsratio = new TH1F("integrated_phiKs_ratio","Integrated #{phi}/K_{s}^{0} Ratio",1,0.,1.);

  lambdaKsratio->Divide(integrated_lambda_yield,integrated_Ks_yield);
  phiKsratio->Divide(integrated_phi_yield,integrated_Ks_yield);

  TH1F* lambdaKsratio_vspt = makeHistogram("lambdaKsratio","#{Lambda}/K_{s}^{0} Ratio",BinInfo::final_pt_bins);
  TH1F* phiKsratio_vspt = makeHistogram("phiKsratio","#{phi}/K_{s}^{0} Ratio",BinInfo::final_pt_bins);

  lambdaKsratio_vspt->Divide(lambda_yield_vspt,Ks_yield_vspt);
  phiKsratio_vspt->Divide(phi_yield_vspt,Ks_yield_vspt);

  TH1F* lambdaKsratio_vsy = makeHistogram("lambdaKsratio","#{Lambda}/K_{s}^{0} Ratio",BinInfo::final_rapidity_bins);
  TH1F* phiKsratio_vsy = makeHistogram("phiKsratio","#{phi}/K_{s}^{0} Ratio",BinInfo::final_rapidity_bins);

  lambdaKsratio_vsy->Divide(lambda_yield_vsy,Ks_yield_vsy);
  phiKsratio_vsy->Divide(phi_yield_vsy,Ks_yield_vsy);

  TH1F* lambdaKsratio_vsphi = makeHistogram("lambdaKsratio","#{Lambda}/K_{s}^{0} Ratio",BinInfo::final_phi_bins);
  TH1F* phiKsratio_vsphi = makeHistogram("phiKsratio","#{phi}/K_{s}^{0} Ratio",BinInfo::final_phi_bins);

  lambdaKsratio_vsphi->Divide(lambda_yield_vsphi,Ks_yield_vsphi);
  phiKsratio_vsphi->Divide(phi_yield_vsphi,Ks_yield_vsphi);

  TH1F* lambdaKsratio_vsntrack = makeHistogram("lambdaKsratio","#{Lambda}/K_{s}^{0} Ratio",BinInfo::final_ntrack_bins);
  TH1F* phiKsratio_vsntrack = makeHistogram("phiKsratio","#{phi}/K_{s}^{0} Ratio",BinInfo::final_ntrack_bins);

  lambdaKsratio_vsntrack->Divide(lambda_yield_vsntrack,Ks_yield_vsntrack);
  phiKsratio_vsntrack->Divide(phi_yield_vsntrack,Ks_yield_vsntrack);

  TFile* fout = new TFile("fits.root","RECREATE");
  Ks_mass->Write();
  phi_mass->Write();
  lambda_mass->Write();

  integrated_phi_yield->Write();
  integrated_Ks_yield->Write();
  integrated_lambda_yield->Write();

  phi_yield_vspt->Write();
  Ks_yield_vspt->Write();
  lambda_yield_vspt->Write();

  phi_yield_vsy->Write();
  Ks_yield_vsy->Write();
  lambda_yield_vsy->Write();

  phi_yield_vsphi->Write();
  Ks_yield_vsphi->Write();
  lambda_yield_vsphi->Write();

  phi_yield_vsntrack->Write();
  Ks_yield_vsntrack->Write();
  lambda_yield_vsntrack->Write();

  lambdaKsratio->Write();
  phiKsratio->Write();

  lambdaKsratio_vspt->Write();
  phiKsratio_vspt->Write();

  lambdaKsratio_vsy->Write();
  phiKsratio_vsy->Write();

  lambdaKsratio_vsphi->Write();
  phiKsratio_vsphi->Write();

  lambdaKsratio_vsntrack->Write();
  phiKsratio_vsntrack->Write();
}
