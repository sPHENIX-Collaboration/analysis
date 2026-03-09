#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

#include "../util/binning.h"
#include "../util/DifferentialContainer.h"

void get_yield(TH1* h_yield, TH1* h_mass, TF1* signal, TF1* background, TF1* total, float xval)
{
  std::cout << "xval: " << xval << std::endl;

  h_mass->Sumw2();

  double fmin;
  double fmax;
  signal->GetRange(fmin,fmax);

  // rescale constant parameters in signal and background to integral of mass slice
  float signal_height = signal->GetParameter(0);
  float bkg_height = background->GetParameter(1);
  float estimated_signal_purity = 1.;
  float total_integral = h_mass->Integral(h_mass->FindBin(fmin),h_mass->FindBin(fmax));

  std::cout << "signal height: " << signal_height << std::endl;
  std::cout << "bkg height: " << bkg_height << std::endl;
  std::cout << "purity: " << estimated_signal_purity << std::endl;
  std::cout << "integral: " << total_integral << std::endl;
  std::cout << "signal const: " << total_integral*estimated_signal_purity << std::endl;
  std::cout << "bkg const: " << total_integral*(1.-estimated_signal_purity) << std::endl;

  float nSignal = total_integral*estimated_signal_purity;
  float nBkg = total_integral*(1.-estimated_signal_purity);

  total->SetParameter(0,nSignal);
  //background->SetParameter(0,1.);

  TF1NormSum* fit_spectrum = new TF1NormSum(signal,background,nSignal,nBkg);

  TF1* fit_f = new TF1("fit_f",*fit_spectrum,fmin,fmax,fit_spectrum->GetNpar());
  fit_f->SetParameters(fit_spectrum->GetParameters().data());

  fit_f->SetParLimits(0,0.,1e9);
  fit_f->SetParLimits(1,0.,1e9);
  fit_f->SetParLimits(2,0.,1e9);
  fit_f->SetParLimits(3,0.,1e9);
  //fit_f->FixParameter(2,1.);
  //fit_f->FixParameter(2+signal->GetNpar(),1.);
  fit_f->SetParName(0,"NSignal");
  fit_f->SetParName(1,"NBkg");
  for(int i=2; i<fit_f->GetNpar(); i++)
  {
    fit_f->SetParName(i,fit_spectrum->GetParName(i));
  }
  fit_f->Write();

  //h_mass->Fit(background,"MRE");
  //total->FixParameter(3,background->GetParameter(0));
  //total->FixParameter(4,background->GetParameter(1));

  TFitResultPtr res = h_mass->Fit(total,"MRES");
  h_mass->Write();
 
  //background->Draw("SAME");

  double xmin, xmax;
  signal->GetRange(xmin,xmax);
  signal->SetParameters(total->GetParameter(0),total->GetParameter(1),total->GetParameter(2));
  signal->SetParError(0,total->GetParError(0));
  signal->SetParError(1,total->GetParError(1));
  signal->SetParError(2,total->GetParError(2));
  float yield = signal->Integral(xmin,xmax)/h_mass->GetBinWidth(1);
  float yield_error = signal->GetParError(0)/sqrt(2*M_PI*signal->GetParameter(2))*(signal->Integral(xmin,xmax)/(signal->GetParameter(0)/sqrt(2*M_PI*signal->GetParameter(2))))/h_mass->GetBinWidth(1);
 
  //float yield = fit_f->GetParameter(0);
  //float yield_error = fit_f->GetParError(0);

  h_yield->SetBinContent(h_yield->FindBin(xval),yield);
  h_yield->SetBinError(h_yield->FindBin(xval),yield_error);
}

void get_yield_allbins(TH1F* h_yield, DifferentialContainer& h_mass2d, TF1* signal, TF1* background, TF1* total)
{
  for(int i=1; i<=h_yield->GetNbinsX(); i++)
  {
    float xval = h_yield->GetBinCenter(i);

    std::cout << "bin " << i << " of " << h_yield->GetNbinsX() << "(size " << h_mass2d.hists.size() << std::endl;

    std::string slice_name = "mass_slice_"+std::string(h_mass2d.hists[i]->GetName());
    TH1F* mass_slice = h_mass2d.hists[i];

    get_yield(h_yield,mass_slice,signal,background,total,xval);
  }
}

void calculate_ratios()
{
//  TFile* Ks_file = TFile::Open("../mass_histograms/data_Kshort_mass.root");
//  TFile* phi_file = TFile::Open("../mass_histograms/data_phi_mass.root");
//  TFile* lambda_file = TFile::Open("../mass_histograms/data_lambda_mass.root");

  TFile* Ks_file = TFile::Open("../mass_histograms/Kshort_79514_hist.root");
  TFile* lambda_file = TFile::Open("../mass_histograms/Lambda_79514_hist.root");

  TH1F* Ks_mass = (TH1F*)Ks_file->Get("Kshort_mass");
//  TH1F* phi_mass = (TH1F*)phi_file->Get("phi_mass");
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

/*
  std::vector<TH2F*> Ks_distributions = 
  {
    (TH2F*)Ks_file->Get("Kshort_mass_vspt"),
    (TH2F*)Ks_file->Get("Kshort_mass_vsntrk"),
    (TH2F*)Ks_file->Get("Kshort_mass_vsy"),
    (TH2F*)Ks_file->Get("Kshort_mass_vsphi")
  };
*/
/*
  std::vector<TH2F*> phi_distributions = 
  {
    (TH2F*)phi_file->Get("phi_mass_vspt"),
    (TH2F*)phi_file->Get("phi_mass_vsntrk"),
    (TH2F*)phi_file->Get("phi_mass_vsy"),
    (TH2F*)phi_file->Get("phi_mass_vsphi")
  };
*/
/*
  std::vector<TH2F*> lambda_distributions = 
  {
    (TH2F*)lambda_file->Get("lambda_mass_vspt"),
    (TH2F*)lambda_file->Get("lambda_mass_vsntrk"),
    (TH2F*)lambda_file->Get("lambda_mass_vsy"),
    (TH2F*)lambda_file->Get("lambda_mass_vsphi")
  };
*/
//  TH1F* integrated_phi_yield = new TH1F("integrated_phi_yield","Integrated #phi yield",1,0.,1.);
  TH1F* integrated_Ks_yield = new TH1F("integrated_Ks_yield","Integrated K_{S}^{0} yield",1,0.,1.);
  TH1F* integrated_lambda_yield = new TH1F("integrated_lambda_yield","Integrated #Lambda yield",1,0.,1.);

  std::vector<HistogramInfo> yield_hinfo =
  {
    BinInfo::final_pt_bins,
    BinInfo::final_rapidity_bins,
    BinInfo::final_phi_bins,
    BinInfo::final_ntrack_bins
  };

  std::vector<TH1F*> Ks_yields;
  for(HistogramInfo& hinfo : yield_hinfo) Ks_yields.push_back(makeHistogram("Ks_yield","K_{S}^{0} yield",hinfo));

//  std::vector<TH1F*> phi_yields;
//  for(HistogramInfo& hinfo : yield_hinfo) phi_yields.push_back(makeHistogram("phi_yield","#phi yield",hinfo));

  std::vector<TH1F*> lambda_yields;
  for(HistogramInfo& hinfo : yield_hinfo) lambda_yields.push_back(makeHistogram("lambda_yield","#Lambda yield",hinfo));

  TF1* Ks_total = new TF1("ks_total","[0]*TMath::Gaus(x,[1],[2])+[3]*x+[4]",0.4,0.6);
  Ks_total->SetParameters(1000.,0.4976,0.01,0.,1.);
  Ks_total->SetParLimits(0,0.,1e9);
  Ks_total->SetParLimits(1,0.45,0.5);
  Ks_total->SetParLimits(2,0.001,0.1);
  //Ks_total->SetParLimits(3,-1000.,1000.);
  //Ks_total->SetParLimits(4,0.,1e9);

  TF1* Lambda_total = new TF1("lambda_total","[0]*TMath::Gaus(x,[1],[2])+[3]*x+[4]",1.08,1.15);
  Lambda_total->SetParameters(1000.,1.11,0.01,0.,1.);
  Lambda_total->SetParLimits(0,0.,1e9);
  Lambda_total->SetParLimits(1,1.10,1.12);
  Lambda_total->SetParLimits(2,0.001,0.1);
  //Lambda_total->SetParLimits(3,-1000.,1000.);
  //Lambda_total->SetParLimits(4,0.,1e9);
  
  
  TF1* Ks_signal = new TF1("ks_signal","gaus",0.4,0.6);
  Ks_signal->SetParameters(1000.,0.4976,0.01);
  Ks_signal->SetParLimits(0,0.,1e9);
  Ks_signal->SetParLimits(1,0.4,0.6);
  Ks_signal->SetParLimits(2,0.,0.2);

  TF1* Ks_background = new TF1("ks_bkg","[0]*x+[1]",0.55,0.6);
  Ks_background->SetParameters(0.,10.);
//  Ks_background->SetParName(0,"Constant"); // needed for TF1NormSum
//  Ks_background->SetParLimits(0,0.,1e9);
/*
  TF1* phi_signal = new TF1("phi_signal","[0]*TMath::Voigt(x-[1],[2],[3])",1.,1.1);
  phi_signal->SetParameters(1000.,1.02,0.001,0.005);
  phi_signal->SetParName(0,"Constant"); // needed for TF1NormSum
  phi_signal->SetParLimits(0,0.,1e9);
  phi_signal->SetParLimits(1,1.,1.1);
  phi_signal->SetParLimits(2,0.,0.1);
  phi_signal->SetParLimits(3,0.,0.1);

  TF1* phi_background = new TF1("phi_bkg","pol4",1.,1.1);
  phi_background->SetParameters(1000.,0.,0.,0.,0.);
  phi_background->SetParName(0,"Constant"); // needed for TF1NormSum
  phi_background->SetParLimits(0,0.,1e9);
*/
  TF1* lambda_signal = new TF1("lambda_signal","gaus",1.08,1.15);
  lambda_signal->SetParameters(1000.,1.11,0.01);
  lambda_signal->SetParLimits(0,0.,1e9);
  lambda_signal->SetParLimits(1,1.08,1.13);
  lambda_signal->SetParLimits(2,0.,0.5);

  TF1* lambda_background = new TF1("lambda_bkg","[0]*x+[1]+[2]*x^2",1.13,1.15);
  lambda_background->SetParameters(0.,10.);
  //lambda_background->SetParName(0,"Constant"); // needed for TF1NormSum
  //lambda_background->SetParLimits(0,0.,1e9);

  TFile* fout = new TFile("fits.root","RECREATE");

  //std::cout << "======== Integrated Phi Yield ========" << std::endl;
  //get_yield(integrated_phi_yield,phi_mass,phi_signal,phi_background,0.5);
  std::cout << "======== Integrated Ks Yield ========" << std::endl;
  get_yield(integrated_Ks_yield,Ks_mass,Ks_signal,Ks_background,Ks_total,0.5);
  std::cout << "======== Integrated Lambda Yield =======" << std::endl;
  get_yield(integrated_lambda_yield,lambda_mass,lambda_signal,lambda_background,Lambda_total,0.5);

  for(size_t i=0; i<yield_hinfo.size(); i++)
  {
    //std::cout << "======= Differential " << phi_yields[i]->GetName() << " =======" << std::endl;
    //get_yield_allbins(phi_yields[i],phi_distributions[i],phi_signal,phi_background);
    std::cout << "======= Differential " << Ks_yields[i]->GetName() << " =======" << std::endl;
    get_yield_allbins(Ks_yields[i],Ks_distributions[i],Ks_signal,Ks_background,Ks_total);
    std::cout << "======= Differential " << lambda_yields[i]->GetName() << " =======" << std::endl;
    get_yield_allbins(lambda_yields[i],lambda_distributions[i],lambda_signal,lambda_background,Lambda_total);

    //std::cout << "integrated phi yield: " << phi_yields[i]->Integral() << " vs. " << integrated_phi_yield->GetBinContent(1) << std::endl;
    std::cout << "integrated Ks yield: " << Ks_yields[i]->Integral() << " vs. " << integrated_Ks_yield->GetBinContent(1) << std::endl;
    std::cout << "integrated lambda yield: " << lambda_yields[i]->Integral() << " vs. " << integrated_lambda_yield->GetBinContent(1) << std::endl;
  }

  TH1F* lambdaKsratio = new TH1F("integrated_lambdaKs_ratio","Integrated #Lambda/K_{S}^{0} Ratio",1,0.,1.);
  //TH1F* phiKsratio = new TH1F("integrated_phiKs_ratio","Integrated #phi/K_{S}^{0} Ratio",1,0.,1.);

  lambdaKsratio->Divide(integrated_lambda_yield,integrated_Ks_yield);
  //phiKsratio->Divide(integrated_phi_yield,integrated_Ks_yield);

  //std::vector<TH1F*> phiKs_diffratios;
  //for(HistogramInfo& hinfo : yield_hinfo) phiKs_diffratios.push_back(makeHistogram("phiKsratio","#phi/K_{S}^{0} Ratio",hinfo));

  std::vector<TH1F*> lambdaKs_diffratios;
  for(HistogramInfo& hinfo : yield_hinfo) lambdaKs_diffratios.push_back(makeHistogram("lambdaKsratio","#Lambda/K_{S}^{0} Ratio",hinfo));

  for(size_t i=0; i<yield_hinfo.size(); i++)
  {
    //phiKs_diffratios[i]->Divide(phi_yields[i],Ks_yields[i]);
    lambdaKs_diffratios[i]->Divide(lambda_yields[i],Ks_yields[i],1.,2.);
  }

  Ks_mass->Write();
  //phi_mass->Write();
  lambda_mass->Write();

  //integrated_phi_yield->Write();
  integrated_Ks_yield->Write();
  integrated_lambda_yield->Write();

  lambdaKsratio->Write();
  //phiKsratio->Write();

  for(size_t i=0; i<yield_hinfo.size(); i++)
  {
    //phi_yields[i]->Write();
    Ks_yields[i]->Write();
    lambda_yields[i]->Write();

    //phiKs_diffratios[i]->Write();
    lambdaKs_diffratios[i]->Write();
  }

}
