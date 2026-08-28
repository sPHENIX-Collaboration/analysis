#include "../util/binning.h"

void formatDefault(TH1F* h, EColor color)
{
  h->SetLineColor(color);
  h->SetMarkerStyle(kFullCircle);
  h->SetMarkerColor(color);
  h->SetMarkerSize(0.7);
}

void plot_lambda(TFile* f_inc, TFile* f_pos, TFile* f_neg, std::string hname, std::string title, std::string filename, float x_legend, float y_legend, bool scale)
{
  std::string clonename = scale? hname+"_scaled" : hname+"_unscaled";

  TH1F* h_inc = (TH1F*)f_inc->Get(hname.c_str())->Clone(clonename.c_str());
  TH1F* h_pos = (TH1F*)f_pos->Get(hname.c_str())->Clone(clonename.c_str());
  TH1F* h_neg = (TH1F*)f_neg->Get(hname.c_str())->Clone(clonename.c_str());
  if(scale)
  {
    h_inc->Scale(1.,"width");
    h_pos->Scale(1.,"width");
    h_neg->Scale(1.,"width");
  }

  TCanvas* c = new TCanvas("c","c",800,800);
  //c->SetLogy();

  c->SetTitle(title.c_str());

  float plot_min = std::min(h_pos->GetMinimum(),h_neg->GetMinimum());
  if(plot_min<0.) plot_min = 1.;

  float plot_max = std::max(h_inc->GetMaximum(),h_pos->GetMaximum());
  if(h_neg->GetMaximum()>plot_max) plot_max = h_neg->GetMaximum();

  h_inc->SetMinimum(0.);
  h_inc->SetMaximum(1.3*plot_max);

  formatDefault(h_inc,kBlack);
  formatDefault(h_pos,kRed);
  formatDefault(h_neg,kBlue);

  h_inc->Draw();
  h_pos->Draw("SAME");
  h_neg->Draw("SAME");

  float x_legend_high = std::min(x_legend+0.25,0.9);
  float y_legend_high = std::min(y_legend+0.13,0.9);

  TLegend* llyt = new TLegend(x_legend,y_legend,x_legend_high,y_legend_high);
  llyt->AddEntry(h_inc,"(#Lambda^{0}+#bar{#Lambda}^{0})");
  llyt->AddEntry(h_pos,"#Lambda^{0}");
  llyt->AddEntry(h_neg,"#bar{#Lambda}^{0}");
  llyt->Draw();

  c->SaveAs(filename.c_str());
  c->Close();
}

void plot_ks(TFile* f, std::string hname, std::string title, std::string filename, bool scale)
{
  std::string clonename = scale? hname+"_scaled" : hname+"_unscaled";

  TH1F* h = (TH1F*)f->Get(hname.c_str())->Clone(clonename.c_str());

  if(scale)
  {
    h->Scale(1.,"width");
  }

  h->SetMinimum(0.);
  h->SetMaximum(h->GetMaximum()*1.3);

  TCanvas* c = new TCanvas("c","c",800,800);
  //c->SetLogy();

  h->SetTitle(title.c_str());
  formatDefault(h,kBlack);
  h->Draw();

  c->SaveAs(filename.c_str());
  c->Close();
}

void plot_ratio(TH1F* ratio_inc, TH1F* ratio_pos, TH1F* ratio_neg, std::string title, std::string filename, float x_legend, float y_legend)
{
  TCanvas* c = new TCanvas("c","c",800,800);

  formatDefault(ratio_inc,kBlack);
  formatDefault(ratio_pos,kRed);
  formatDefault(ratio_neg,kBlue);

  ratio_inc->SetTitle(title.c_str());
  ratio_inc->SetMinimum(0.);
  ratio_inc->SetMaximum(1.3*std::max(ratio_pos->GetMaximum(),ratio_neg->GetMaximum()));

  ratio_inc->Draw();
  ratio_pos->Draw("SAME");
  ratio_neg->Draw("SAME");

  float x_legend_high = std::min(x_legend+0.25,0.9);
  float y_legend_high = std::min(y_legend+0.13,0.9);

  TLegend* lrt = new TLegend(x_legend,y_legend,x_legend_high,y_legend_high);
  lrt->AddEntry(ratio_inc,"(#Lambda^{0}+#bar{#Lambda}^{0})/2K_{S}^{0}");
  lrt->AddEntry(ratio_pos,"#Lambda^{0}/K_{S}^{0}");
  lrt->AddEntry(ratio_neg,"#bar{#Lambda}^{0}/K_{S}^{0}");
  lrt->Draw();

  c->SaveAs(filename.c_str());
  c->Close();
}

void plot_truth_vsreco(TH1F* truth, TH1F* reco, std::string title, std::string filename, float x_legend, float y_legend)
{
  TCanvas* c = new TCanvas("c","c",800,800);

  formatDefault(truth,kBlack);
  formatDefault(reco,kRed);

  truth->SetTitle(title.c_str());
  truth->SetMinimum(0.);
  truth->SetMaximum(1.1*std::max(truth->GetMaximum(),reco->GetMaximum()));

  truth->Draw();
  reco->Draw("SAME");

  float x_legend_high = std::min(x_legend+0.25,0.9);
  float y_legend_high = std::min(y_legend+0.25,0.9);

  TLegend* ltr = new TLegend(x_legend,y_legend,x_legend_high,y_legend_high);
  ltr->AddEntry(truth,"truth");
  ltr->AddEntry(reco,"reco");

  c->SaveAs(filename.c_str());
  c->Close(); 
}

void plot_correction_sequence(TH1F* truth, TH1F* reco, TH1F* geo_acc, TH1F* cut_eff, std::string title, std::string filename, float x_legend, float y_legend)
{
  TCanvas* c = new TCanvas("c","c",800,800);

//  truth->Sumw2();
//  reco->Sumw2();
//  geo_acc->Sumw2();
//  cut_eff->Sumw2();

  TH1F* reco_geo = (TH1F*)reco->Clone((std::string(reco->GetName())+"_geo").c_str());
  TH1F* reco_geo_cut = (TH1F*)reco->Clone((std::string(reco->GetName())+"_geo_cut").c_str());

  formatDefault(truth,kBlack);
  formatDefault(reco,kRed);
  formatDefault(reco_geo,kBlue);
  formatDefault(reco_geo_cut,kGreen);

  reco_geo->Divide(geo_acc);
  reco_geo_cut->Divide(geo_acc);
  reco_geo_cut->Divide(cut_eff);

  std::cout << reco->GetName() << std::endl;
  for(int i=1;i<=truth->GetNbinsX();i++)
  {
    std::cout << "bin " << i << ": " << std::endl
      << "reco: " << reco->GetBinContent(i) << " +- " << reco->GetBinError(i) << std::endl
      << "geo acceptance: " << geo_acc->GetBinContent(i) << " +- " << geo_acc->GetBinError(i) << std::endl
      << "cut eff: " << cut_eff->GetBinContent(i) << " +- " << cut_eff->GetBinError(i) << std::endl
      << "reco/geoacc: " << reco_geo->GetBinContent(i) << " +- " << reco_geo->GetBinError(i) << std::endl
      << "reco/(geoacc*cuteff): " << reco_geo_cut->GetBinContent(i) << " +- " << reco_geo_cut->GetBinError(i) << std::endl;
  }

  truth->SetTitle(title.c_str());
  truth->SetMinimum(0.);
  truth->SetMaximum(1.1*std::max(truth->GetMaximum(),reco_geo_cut->GetMaximum()));

  truth->Draw();
  reco->Draw("SAME");
  reco_geo->Draw("SAME");
  reco_geo_cut->Draw("SAME");

  float x_legend_high = std::min(x_legend+0.25,0.9);
  float y_legend_high = std::min(y_legend+0.25,0.9);

  TLegend* lc = new TLegend(x_legend,y_legend,x_legend_high,y_legend_high);
  lc->AddEntry(truth,"truth");
  lc->AddEntry(reco,"reco uncorrected");
  lc->AddEntry(reco_geo,"reco, geometric acceptance corrected");
  lc->AddEntry(reco_geo_cut,"reco, geometric acceptance and cut efficiency corrected");
  lc->Draw();

  c->SaveAs(filename.c_str());
  c->Close();
}

void plot_correction(TH1F* corr, std::string title, std::string filename)
{
  TCanvas* c = new TCanvas("c","c",800,800);

  formatDefault(corr,kBlack);

  corr->SetTitle(title.c_str());
  corr->SetMinimum(0.);
  corr->SetMaximum(1.2*corr->GetMaximum());

  corr->Draw();

  c->SaveAs(filename.c_str());
  c->Close();
}

void plot_inverse_correction_sequence(TH1F* truth, TH1F* reco, TH1F* geo_acc, TH1F* cut_eff, std::string title, std::string filename, float x_legend, float y_legend)
{
  TCanvas* c = new TCanvas("c","c",800,800);

//  truth->Sumw2();
//  reco->Sumw2();
//  geo_acc->Sumw2();
//  cut_eff->Sumw2();

  TH1F* truth_cut = (TH1F*)truth->Clone((std::string(truth->GetName())+"_geo").c_str());
  TH1F* truth_cut_geo = (TH1F*)truth->Clone((std::string(truth->GetName())+"_geo_cut").c_str());

  TH1F* reco_geo = (TH1F*)reco->Clone((std::string(reco->GetName())+"_geo").c_str());
  TH1F* reco_geo_cut = (TH1F*)reco->Clone((std::string(reco->GetName())+"_geo_cut").c_str());

  formatDefault(truth,kBlack);
  formatDefault(reco,kRed);
  formatDefault(truth_cut,kViolet);
  formatDefault(truth_cut_geo,kOrange);
  formatDefault(reco_geo,kBlue);
  formatDefault(reco_geo_cut,kGreen);

  truth_cut->Multiply(cut_eff);
  truth_cut_geo->Multiply(cut_eff);
  truth_cut_geo->Multiply(geo_acc);

  reco_geo->Divide(geo_acc);
  reco_geo_cut->Divide(geo_acc);
  reco_geo_cut->Divide(cut_eff);

  std::cout << reco->GetName() << std::endl;
  for(int i=1;i<=truth->GetNbinsX();i++)
  {
    std::cout << "bin " << i << ": " << std::endl
      << "truth: " << truth->GetBinContent(i) << " +- " << truth->GetBinError(i) << std::endl
      << "geo acceptance: " << geo_acc->GetBinContent(i) << " +- " << geo_acc->GetBinError(i) << std::endl
      << "cut eff: " << cut_eff->GetBinContent(i) << " +- " << cut_eff->GetBinError(i) << std::endl
      << "truth*cuteff: " << truth_cut->GetBinContent(i) << " +- " << truth_cut->GetBinError(i) << std::endl
      << "truth*(geoacc*cuteff): " << truth_cut_geo->GetBinContent(i) << " +- " << truth_cut_geo->GetBinError(i) << std::endl
      << "reco: " << reco->GetBinContent(i) << " +- " << reco->GetBinError(i) << std::endl;
  }

  truth->SetTitle(title.c_str());
  truth->SetMinimum(0.);
  truth->SetMaximum(1.1*truth->GetMaximum());

  truth->Draw();
  reco->Draw("SAME");
  truth_cut->Draw("SAME");
  truth_cut_geo->Draw("SAME");
  reco_geo->Draw("SAME");
  reco_geo_cut->Draw("SAME");

  float x_legend_high = std::min(x_legend+0.4,0.9);
  float y_legend_high = std::min(y_legend+0.2,0.9);

  TLegend* lc = new TLegend(x_legend,y_legend,x_legend_high,y_legend_high);
  lc->AddEntry(truth,"truth");
  lc->AddEntry(reco,"reco uncorrected");
  lc->AddEntry(truth_cut,"truth after cut efficiency");
  lc->AddEntry(truth_cut_geo,"truth after cut efficiency and geometric acceptance");
  lc->AddEntry(reco_geo,"reco after geometric acceptance correction");
  lc->AddEntry(reco_geo_cut,"reco after geometric acceptance and cut efficiency correction");
  lc->Draw();

  c->SaveAs(filename.c_str());
  c->Close();
}

void plot_truth_comparison()
{
  gROOT->SetBatch();

  TFile* ftruth = TFile::Open("merged_truth_ratio.root");
  TFile* ftruth_pos = TFile::Open("merged_ratio_pos.root");
  TFile* ftruth_neg = TFile::Open("merged_ratio_neg.root");

  TFile* freco = TFile::Open("../yield_and_ratios/fits_MC.root");
  TFile* freco_pos = TFile::Open("../yield_and_ratios/fits_MC_pos.root");
  TFile* freco_neg = TFile::Open("../yield_and_ratios/fits_MC_neg.root");

  TFile* fdata = TFile::Open("../yield_and_ratios/fits.root");
  TFile* fdata_pos = TFile::Open("../yield_and_ratios/fits_pos.root");
  TFile* fdata_neg = TFile::Open("../yield_and_ratios/fits_neg.root");

  TFile* fgeo_corr = TFile::Open("/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/geometricAcceptanceCorrection/corrections/geo_acceptance_inclusive.root");
  TFile* fgeo_corr_pos = TFile::Open("/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/geometricAcceptanceCorrection/corrections/geo_acceptance_lambdaonly.root");
  TFile* fgeo_corr_neg = TFile::Open("/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/geometricAcceptanceCorrection/corrections/geo_acceptance_lambdabaronly.root");

  TFile* fcut_corr = TFile::Open("/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/cutEfficiencyCorrection/cut_efficiency_correction.root");
  TFile* fcut_corr_pos = TFile::Open("/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/cutEfficiencyCorrection/cut_efficiency_correction_pos.root");
  TFile* fcut_corr_neg = TFile::Open("/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/cutEfficiencyCorrection/cut_efficiency_correction_neg.root");

  std::vector<HistogramInfo> variables = {
    BinInfo::final_pt_bins,
    BinInfo::final_eta_bins,
    BinInfo::final_phi_bins,
    BinInfo::final_rapidity_bins
  };

  gStyle->SetOptStat(0);

  for(int i=0; i<variables.size(); i++)
  {
    std::string vname = variables[i].name;
    std::string vtitle = variables[i].title;

    // lambda yields

    plot_lambda(ftruth,ftruth_pos,ftruth_neg,"hn_vs"+vname,"#Lambda yield vs. "+vtitle+" from truth",
                "plots/yields/lambda_yields_truth_vs"+vname+".png",0.65,0.77,false);
    plot_lambda(ftruth,ftruth_pos,ftruth_neg,"hn_vs"+vname,"#Lambda yield vs. "+vtitle+" from truth (scaled by bin width)",
                "plots/yields/lambda_yields_truth_scaled_vs"+vname+".png",0.65,0.77,true);
    plot_lambda(ftruth,ftruth_pos,ftruth_neg,"hn_wd_vs"+vname,"#Lambda yield vs. "+vtitle+" from truth, with all reco daughters reconstructed",
                "plots/yields/lambda_yields_wd_truth_vs"+vname+".png",0.65,0.77,false);
    plot_lambda(ftruth,ftruth_pos,ftruth_neg,"hn_wd_vs"+vname,"#Lambda yield vs. "+vtitle+" from truth, with all reco daughters reconstructed (scaled by bin width)","plots/yields/lambda_yields_wd_truth_scaled_vs"+vname+".png",0.65,0.77,true);
    plot_lambda(freco,freco_pos,freco_neg,"Lambda0_yield_vs"+vname,"#Lambda yield (uncorrected) vs. "+vtitle+" from reco",
                "plots/yields/lambda_yields_reco_vs"+vname+".png",0.65,0.77,false);
    plot_lambda(freco,freco_pos,freco_neg,"Lambda0_yield_vs"+vname,"#Lambda yield (uncorrected) vs. "+vtitle+" from reco (scaled by bin width)",
                "plots/yields/lambda_yields_reco_scaled_vs"+vname+".png",0.65,0.77,true);
//    plot_lambda(fdata,fdata_pos,fdata_neg,"Lambda0_yield_vs"+vname,"#Lambda yield (uncorrected) vs. "+vtitle+" from data",
//                "plots/yields/lambda_yields_data_vs"+vname+".png",0.65,0.77,false);
//    plot_lambda(fdata,fdata_pos,fdata_neg,"Lambda0_yield_vs"+vname,"#Lambda yield (uncorrected) vs. "+vtitle+" from data (scaled by bin width)",
//                "plots/yields/lambda_yields_data_scaled_vs"+vname+".png",0.65,0.77,true);

    // Ks yields

    plot_ks(ftruth,"hd_vs"+vname,"K_{S} yield vs. "+vtitle+" from truth","plots/yields/ks_yield_truth_vs"+vname+".png",false);
    plot_ks(ftruth,"hd_vs"+vname,"K_{S} yield vs. "+vtitle+" from truth (scaled by bin width)","plots/yields/ks_yield_truth_scaled_vs"+vname+".png",true);
    plot_ks(ftruth,"hd_wd_vs"+vname,"K_{S} yield vs. "+vtitle+" from truth, with all reco daughters reconstructed","plots/yields/ks_yield_wd_truth_vs"+vname+".png",false);
    plot_ks(ftruth,"hd_wd_vs"+vname,"K_{S} yield vs. "+vtitle+" from truth, with all reco daughters reconstructed (scaled by bin width)","plots/yields/ks_yield_wd_truth_scaled_vs"+vname+".png",true);
    plot_ks(freco,"K_S0_yield_vs"+vname,"K_{S} yield vs. "+vtitle+" from reco","plots/yields/ks_yield_reco_vs"+vname+".png",false);
    plot_ks(freco,"K_S0_yield_vs"+vname,"K_{S} yield vs. "+vtitle+" from reco (scaled by bin width)","plots/yields/ks_yield_reco_scaled_vs"+vname+".png",true);
//    plot_ks(fdata,"K_S0_yield_vs"+vname,"K_{S} yield vs. "+vtitle+" from data","plots/yields/ks_yield_data_vs"+vname+".png",false);
//    plot_ks(fdata,"K_S0_yield_vs"+vname,"K_{S} yield vs. "+vtitle+" from data (scaled by bin width)","plots/yields/ks_yield_data_scaled_vs"+vname+".png",true);

    // Ratios

    TH1F* ratio_truth = makeHistogram("ratio_truth","(#Lambda^{0}+#bar{#Lambda^{0}})/2K^{0}_{S} ratio from truth",variables[i]);
    TH1F* ratio_truth_pos = makeHistogram("ratio_truth_pos","#Lambda^{0}/K^{0}_{S} ratio from truth",variables[i]);
    TH1F* ratio_truth_neg = makeHistogram("ratio_truth_neg","#bar{#Lambda^{0}}/K^{0}_{S} ratio from truth",variables[i]);

    TH1F* lambda_truth = (TH1F*)ftruth->Get(("hn_vs"+vname).c_str());
    TH1F* lambda_truth_pos = (TH1F*)ftruth_pos->Get(("hn_vs"+vname).c_str());
    TH1F* lambda_truth_neg = (TH1F*)ftruth_neg->Get(("hn_vs"+vname).c_str());
    TH1F* ks_truth = (TH1F*)ftruth->Get(("hd_vs"+vname).c_str());

    for(TH1F* h : {lambda_truth, lambda_truth_pos, lambda_truth_neg, ks_truth})
    {
      h->Sumw2();
    }

    ratio_truth->Divide(lambda_truth,ks_truth,1.,2.);
    ratio_truth_pos->Divide(lambda_truth_pos,ks_truth);
    ratio_truth_neg->Divide(lambda_truth_neg,ks_truth);

    std::string reco_uncorrected_name = "lambdaKsratio_vs"+vname;
    std::string reco_corrected_name = "lambdaKsratio_vs"+vname+"_lambdafeeddowncorrected_effcorrected_geoacceptancecorrected_cutefficiencycorrected";

    TH1F* ratio_reco_inc = (TH1F*)freco->Get(reco_uncorrected_name.c_str());
    TH1F* ratio_reco_pos = (TH1F*)freco_pos->Get(reco_uncorrected_name.c_str());
    TH1F* ratio_reco_neg = (TH1F*)freco_neg->Get(reco_uncorrected_name.c_str());
    TH1F* ratio_reco_corr_inc = (TH1F*)freco->Get(reco_corrected_name.c_str());
    TH1F* ratio_reco_corr_pos = (TH1F*)freco_pos->Get(reco_corrected_name.c_str());
    TH1F* ratio_reco_corr_neg = (TH1F*)freco_neg->Get(reco_corrected_name.c_str());
/*
    TH1F* ratio_data_inc = (TH1F*)fdata->Get(reco_uncorrected_name.c_str());
    TH1F* ratio_data_pos = (TH1F*)fdata_pos->Get(reco_uncorrected_name.c_str());
    TH1F* ratio_data_neg = (TH1F*)fdata_neg->Get(reco_uncorrected_name.c_str());
    TH1F* ratio_data_corr_inc = (TH1F*)fdata->Get(reco_corrected_name.c_str());
    TH1F* ratio_data_corr_pos = (TH1F*)fdata_pos->Get(reco_corrected_name.c_str());
    TH1F* ratio_data_corr_neg = (TH1F*)fdata_neg->Get(reco_corrected_name.c_str());
*/

    double lambda_BR = 0.638719992;
    double Ks_BR = 0.691086452;

    for(TH1F* h : {ratio_reco_inc, ratio_reco_pos, ratio_reco_neg, ratio_reco_corr_inc, ratio_reco_corr_pos, ratio_reco_corr_neg})
//                   ratio_data_inc, ratio_data_pos, ratio_data_neg, ratio_data_corr_inc, ratio_data_corr_pos, ratio_data_corr_neg})
    {
      h->Scale(Ks_BR/lambda_BR);
    }

    plot_ratio(ratio_truth,ratio_truth_pos,ratio_truth_neg,"Ratio comparison vs. "+vtitle+" from truth","plots/ratios/ratio_truth_vs"+vname+".png",0.55,0.15);
    plot_ratio(ratio_reco_inc,ratio_reco_pos,ratio_reco_neg,"Ratio comparison vs. "+vtitle+" (uncorrected) from reco","plots/ratios/ratio_reco_vs"+vname+".png",0.15,0.75);
    plot_ratio(ratio_reco_corr_inc,ratio_reco_corr_pos,ratio_reco_corr_neg,"Ratio comparison vs. "+vtitle+" (fully corrected) from reco","plots/ratios/ratio_reco_corr_vs"+vname+".png",0.65,0.77);
//    plot_ratio(ratio_data_inc,ratio_data_pos,ratio_data_neg,"Ratio comparison vs. "+vtitle+" (uncorrected) from data","plots/ratios/ratio_data_vs"+vname+".png",0.65,0.77);
//    plot_ratio(ratio_data_corr_inc,ratio_data_corr_pos,ratio_data_corr_neg,"Ratio comparison vs. "+vtitle+" (fully corrected) from data","plots/ratios/ratio_data_corr_vs"+vname+".png",0.65,0.77);

    // Truth-reco closure comparisons
    plot_truth_vsreco(ratio_truth,ratio_reco_corr_inc,"(#Lambda+#bar{#Lambda})/2K_{S}^{0} ratio","plots/closure/closure_ratio_vs"+vname+".png",0.55,0.15);
    plot_truth_vsreco(ratio_truth_pos,ratio_reco_corr_pos,"#Lambda/K_{S}^{0} ratio","plots/closure/closure_ratio_pos_vs"+vname+".png",0.55,0.15);
    plot_truth_vsreco(ratio_truth_neg,ratio_reco_corr_neg,"#bar{#Lambda}/K_{S}^{0} ratio","plots/closure/closure_ratio_neg_vs"+vname+".png",0.55,0.15);

    // Direct yield comparisons
    TH1F* lambda_geo_acceptance = (TH1F*)fgeo_corr->Get(("Lambda0_acceptance_vs"+vname).c_str());
    TH1F* lambda_geo_acceptance_pos = (TH1F*)fgeo_corr_pos->Get(("Lambda0_acceptance_vs"+vname).c_str());
    TH1F* lambda_geo_acceptance_neg = (TH1F*)fgeo_corr_neg->Get(("Lambda0_acceptance_vs"+vname).c_str());

    plot_correction(lambda_geo_acceptance,"(#Lambda+#bar{#Lambda}) geometric acceptance","plots/corrections/lambda_geo_acceptance_vs"+vname+".png");
    plot_correction(lambda_geo_acceptance_pos,"#Lambda geometric acceptance","plots/corrections/lambda_pos_geo_acceptance_vs"+vname+".png");
    plot_correction(lambda_geo_acceptance_neg,"#bar{#Lambda} geometric acceptance","plots/corrections/lambda_neg_geo_acceptance_vs"+vname+".png");

    TH1F* lambda_cut_efficiency = (TH1F*)fcut_corr->Get(("Lambda0_cuteff_vs"+vname).c_str());
    TH1F* lambda_cut_efficiency_pos = (TH1F*)fcut_corr_pos->Get(("Lambda0_cuteff_vs"+vname).c_str());
    TH1F* lambda_cut_efficiency_neg = (TH1F*)fcut_corr_neg->Get(("Lambda0_cuteff_vs"+vname).c_str());

    plot_correction(lambda_cut_efficiency,"(#Lambda+#bar{#Lambda}) cut efficiency","plots/corrections/lambda_cut_efficiency_vs"+vname+".png");
    plot_correction(lambda_cut_efficiency_pos,"#Lambda cut efficiency","plots/corrections/lambda_pos_cut_efficiency_vs"+vname+".png");
    plot_correction(lambda_cut_efficiency_neg,"#bar{#Lambda} cut efficiency","plots/corrections/lambda_neg_cut_efficiency_vs"+vname+".png");

    TH1F* lambda_reco_inc = (TH1F*)freco->Get(("Lambda0_yield_vs"+vname).c_str());
    TH1F* lambda_reco_pos = (TH1F*)freco_pos->Get(("Lambda0_yield_vs"+vname).c_str());
    TH1F* lambda_reco_neg = (TH1F*)freco_neg->Get(("Lambda0_yield_vs"+vname).c_str());

    lambda_truth->Scale(1.,"width");
    lambda_truth_pos->Scale(1.,"width");
    lambda_truth_neg->Scale(1.,"width");
    lambda_reco_inc->Scale(1.,"width");
    lambda_reco_pos->Scale(1.,"width");
    lambda_reco_neg->Scale(1.,"width");

    TH1F* Kshort_geo_acceptance = (TH1F*)fgeo_corr->Get(("K_S0_acceptance_vs"+vname).c_str());

    TH1F* Kshort_cut_efficiency = (TH1F*)fcut_corr->Get(("K_S0_cuteff_vs"+vname).c_str());

    plot_correction(Kshort_geo_acceptance,"K_{S}^{0} geometric acceptance","plots/corrections/ks_geo_acceptance_vs"+vname+".png");
    plot_correction(Kshort_cut_efficiency,"K_{S}^{0} cut efficiency","plots/corrections/ks_cut_efficiency_vs"+vname+".png");

    TH1F* geoacc_corr = (TH1F*)fgeo_corr->Get(("Lambda0_over_K_S0_geo_acceptance_correction_vs"+vname).c_str());
    TH1F* geoacc_corr_pos = (TH1F*)fgeo_corr_pos->Get(("Lambda0_over_K_S0_geo_acceptance_correction_vs"+vname).c_str());
    TH1F* geoacc_corr_neg = (TH1F*)fgeo_corr_neg->Get(("Lambda0_over_K_S0_geo_acceptance_correction_vs"+vname).c_str());
    TH1F* cuteff_corr = (TH1F*)fcut_corr->Get(("Lambda0_over_K_S0_cuteff_correction_vs"+vname).c_str());
    TH1F* cuteff_corr_pos = (TH1F*)fcut_corr->Get(("Lambda0_over_K_S0_cuteff_correction_vs"+vname).c_str());
    TH1F* cuteff_corr_neg = (TH1F*)fcut_corr->Get(("Lambda0_over_K_S0_cuteff_correction_vs"+vname).c_str());

    plot_correction(geoacc_corr,"(#Lambda+#bar{#Lambda})/2K_{S}^{0} geometric acceptance correction","plots/corrections/ratio_geo_acceptance_vs"+vname+".png");
    plot_correction(geoacc_corr_pos,"#Lambda/K_{S}^{0} geometric acceptance correction","plots/corrections/ratio_geo_acceptance_pos_vs"+vname+".png");
    plot_correction(geoacc_corr_neg,"#bar{#Lambda}/K_{S}^{0} geometric acceptance correction","plots/corrections/ratio_geoacceptance_neg_vs"+vname+".png");
    plot_correction(cuteff_corr,"(#Lambda+#bar{#Lambda})/2K_{S}^{0} cut efficiency correction","plots/corrections/ratio_cuteff_vs"+vname+".png");
    plot_correction(cuteff_corr_pos,"#Lambda/K_{S}^{0} cut efficiency correction","plots/corrections/ratio_cuteff_pos_vs"+vname+".png");
    plot_correction(cuteff_corr_neg,"#bar{#Lambda}/K_{S}^{0} cut efficiency correction","plots/corrections/ratio_cuteff_neg_vs"+vname+".png");

    TH1F* Kshort_reco_inc = (TH1F*)freco->Get(("K_S0_yield_vs"+vname).c_str());

    Kshort_reco_inc->Scale(1.,"width");
    ks_truth->Scale(1.,"width");

    Kshort_reco_inc->Scale(1./Ks_BR);
    lambda_reco_inc->Scale(1./lambda_BR);
    lambda_reco_pos->Scale(1./lambda_BR);
    lambda_reco_neg->Scale(1./lambda_BR);

    plot_correction_sequence(lambda_truth,lambda_reco_inc,lambda_geo_acceptance,lambda_cut_efficiency,"(#Lambda+#bar{#Lambda}) yield","plots/closure/closure_lambda_corrections_vs"+vname+".png",0.55,0.15);
    plot_correction_sequence(lambda_truth_pos,lambda_reco_pos,lambda_geo_acceptance_pos,lambda_cut_efficiency_pos,"#Lambda yield","plots/closure/closure_lambda_pos_corrections_vs"+vname+".png",0.55,0.15);
    plot_correction_sequence(lambda_truth_neg,lambda_reco_neg,lambda_geo_acceptance_neg,lambda_cut_efficiency_neg,"#bar{#Lambda} yield","plots/closure/closure_lambda_neg_corrections_vs"+vname+".png",0.55,0.15);
    plot_correction_sequence(ks_truth,Kshort_reco_inc,Kshort_geo_acceptance,Kshort_cut_efficiency,"K_{S}^{0} yield","plots/closure/closure_kshort_corrections_vs"+vname+".png",0.55,0.15);

    plot_inverse_correction_sequence(lambda_truth,lambda_reco_inc,lambda_geo_acceptance,lambda_cut_efficiency,"(#Lambda+#bar{#Lambda}) yield","plots/closure/inverse_closure_lambda_corrections_vs"+vname+".png",0.45,0.2);
    plot_inverse_correction_sequence(lambda_truth_pos,lambda_reco_pos,lambda_geo_acceptance_pos,lambda_cut_efficiency_pos,"#Lambda yield","plots/closure/inverse_closure_lambda_pos_corrections_vs"+vname+".png",0.45,0.2);
    plot_inverse_correction_sequence(lambda_truth_neg,lambda_reco_neg,lambda_geo_acceptance_neg,lambda_cut_efficiency_neg,"#bar{#Lambda} yield","plots/closure/inverse_closure_lambda_neg_corrections_vs"+vname+".png",0.45,0.2);
    plot_inverse_correction_sequence(ks_truth,Kshort_reco_inc,Kshort_geo_acceptance,Kshort_cut_efficiency,"K_{S}^{0} yield","plots/closure/inverse_closure_kshort_corrections_vs"+vname+".png",0.45,0.2);
  }
/*
  TH1F* lambda_eff = makeHistogram("lambda_eff","(#Lambda^{0}+#bar{#Lambda}^{0}) fraction with all daughter tracks reconstructed",BinInfo::final_pt_bins);
  TH1F* lambda_eff_pos = makeHistogram("lambda_eff_pos","#Lambda^{0} fraction with all daughter tracks reconstructed",BinInfo::final_pt_bins);
  TH1F* lambda_eff_neg = makeHistogram("lambda_eff_neg","#bar{#Lambda}^{0} fraction with all daughter tracks reconstructed",BinInfo::final_pt_bins);
  TH1F* ks_eff = makeHistogram("ks_eff","K^{0}_{S} fraction with all daughter tracks reconstructed",BinInfo::final_pt_bins);

  lambda_eff->Divide(lambda_truth_withdaughters,lambda_truth);
  lambda_eff_pos->Divide(lambda_truth_withdaughters_pos,lambda_truth_pos);
  lambda_eff_neg->Divide(lambda_truth_withdaughters_neg,lambda_truth_neg);
  ks_eff->Divide(ks_truth_withdaughters,ks_truth);

  TCanvas* eff_n = new TCanvas("en","en",800,800);

  formatDefault(lambda_eff,kBlack);
  formatDefault(lambda_eff_pos,kRed);
  formatDefault(lambda_eff_neg,kBlue);

  lambda_eff->SetTitle("#Lambda fraction with both daughters reconstructed;pT [GeV/c];");
  lambda_eff->SetMinimum(0.);
  lambda_eff->SetMaximum(1.1*std::max(lambda_eff_pos->GetMaximum(),lambda_eff_neg->GetMaximum()));

  lambda_eff->Draw();
  lambda_eff_pos->Draw("SAME");
  lambda_eff_neg->Draw("SAME");

  TLegend* le = new TLegend(0.55,0.15,0.88,0.35);
  le->AddEntry(ratio_truth,"#Lambda^{0}+#bar{#Lambda}^{0}");
  le->AddEntry(ratio_truth_pos,"#Lambda^{0}");
  le->AddEntry(ratio_truth_neg,"#bar{#Lambda}^{0}");
  le->Draw();

  eff_n->SaveAs("plots/eff_n.png");
  eff_n->Close();

  TCanvas* eff_d = new TCanvas("ed","ed",800,800);

  formatDefault(ks_eff,kBlack);

  ks_eff->SetTitle("K^{0}_{S} fraction with both daughters reconstructed;pT [GeV/c];");
  ks_eff->SetMinimum(0.);

  ks_eff->Draw();

  eff_d->SaveAs("plots/eff_d.png");
  eff_d->Close();
*/
}
