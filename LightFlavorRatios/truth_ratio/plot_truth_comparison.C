#include "../util/HistogramTools.h"
#include "../config/binning.h"

void formatDefault(TH1F* h, EColor color)
{
  h->SetLineColor(color);
  h->SetMarkerStyle(kFullCircle);
  h->SetMarkerColor(color);
  h->SetMarkerSize(0.7);
}

void formatDefault_graph(TGraphMultiErrors* g, EColor color)
{
  g->SetMarkerStyle(kFullCircle);
  g->SetMarkerColor(color);
  g->SetMarkerSize(0.7);

  g->GetAttLine(0)->SetLineColor(color);
  g->GetAttLine(1)->SetLineColor(color);
  g->GetAttFill(1)->SetFillStyle(0);
}

TGraphMultiErrors* build_statsys_graph(TH1F* h_vals, std::vector<TH1F*> h_syserr, std::string name, std::string title)
{
  std::vector<double> xvals;
  std::vector<double> xerrlo;
  std::vector<double> xerrhi;

  std::vector<double> yvals;
  std::vector<double> staterrlo;
  std::vector<double> staterrhi;
  std::vector<double> syserrlo;
  std::vector<double> syserrhi;

  for(int i=1; i<=h_vals->GetNbinsX(); i++)
  {
    xvals.push_back(h_vals->GetBinCenter(i));
    xerrlo.push_back(h_vals->GetBinWidth(i)/2.);
    xerrhi.push_back(h_vals->GetBinWidth(i)/2.);

    yvals.push_back(h_vals->GetBinContent(i));
    staterrlo.push_back(h_vals->GetBinError(i));
    staterrhi.push_back(h_vals->GetBinError(i));

    double total_syserr2 = 0.;
    for(TH1F* h : h_syserr)
    {
      total_syserr2 += pow(h->GetBinContent(i)*h_vals->GetBinContent(i),2);
    }

    syserrlo.push_back(sqrt(total_syserr2));
    syserrhi.push_back(sqrt(total_syserr2));
  }

  TGraphMultiErrors* g = new TGraphMultiErrors(name.c_str(),title.c_str(),yvals.size(),xvals.data(),yvals.data(),xerrlo.data(),xerrhi.data(),staterrlo.data(),staterrhi.data());
  g->AddYError(yvals.size(),syserrlo.data(),syserrhi.data());

  return g;
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

void plot_lambda_reco(TH1F* yield_inc, TH1F* yield_pos, TH1F* yield_neg, std::vector<TH1F*> syserr_inc, std::vector<TH1F*> syserr_pos, std::vector<TH1F*> syserr_neg, std::string title, std::string filename, float x_legend, float y_legend)
{
  std::string hname = yield_inc->GetName();

  TGraphMultiErrors* g_inc = build_statsys_graph(yield_inc,syserr_inc,hname+"_graph",title);
  TGraphMultiErrors* g_pos = build_statsys_graph(yield_pos,syserr_pos,hname+"_pos_graph",title);
  TGraphMultiErrors* g_neg = build_statsys_graph(yield_neg,syserr_neg,hname+"_neg_graph",title);

  formatDefault_graph(g_inc,kBlack);
  formatDefault_graph(g_pos,kRed);
  formatDefault_graph(g_neg,kBlue);

  TMultiGraph* mg = new TMultiGraph();
  mg->Add(g_inc,"APS; Z ; 5 s=0.5");
  mg->Add(g_pos,"APS; Z ; 5 s=0.5");
  mg->Add(g_neg,"APS; Z ; 5 s=0.5");

  TCanvas* c = new TCanvas("c","c",800,800);
  //c->SetLogy();

  c->SetTitle(title.c_str());

  float plot_min = std::min(yield_pos->GetMinimum(),yield_neg->GetMinimum());
  if(plot_min<0.) plot_min = 1.;

  float plot_max = std::max(yield_inc->GetMaximum(),yield_pos->GetMaximum());
  if(yield_neg->GetMaximum()>plot_max) plot_max = yield_neg->GetMaximum();

  mg->GetYaxis()->SetRangeUser(0.,1.3*plot_max);

  mg->Draw("APS; Z ; 5 s=0.5");

  float x_legend_high = std::min(x_legend+0.25,0.9);
  float y_legend_high = std::min(y_legend+0.13,0.9);

  TLegend* llyt = new TLegend(x_legend,y_legend,x_legend_high,y_legend_high);
  llyt->AddEntry(yield_inc,"(#Lambda^{0}+#bar{#Lambda}^{0})");
  llyt->AddEntry(yield_pos,"#Lambda^{0}");
  llyt->AddEntry(yield_neg,"#bar{#Lambda}^{0}");
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

void plot_ks_reco(TH1F* yield, std::vector<TH1F*> syserr, std::string title, std::string filename)
{
  std::string hname = yield->GetName();

  TGraphMultiErrors* g = build_statsys_graph(yield,syserr,hname+"_graph",title);

  g->GetYaxis()->SetRangeUser(0.,yield->GetMaximum()*1.3);

  TCanvas* c = new TCanvas("c","c",800,800);
  //c->SetLogy();

  g->SetTitle(title.c_str());
  formatDefault_graph(g,kBlack);
  g->Draw("APS ; Z ; 5 s=0.5");

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

void plot_ratio_reco(TH1F* ratio_inc, TH1F* ratio_pos, TH1F* ratio_neg, std::vector<TH1F*> ratio_inc_err, std::vector<TH1F*> ratio_pos_err, std::vector<TH1F*> ratio_neg_err, std::string title, std::string filename, float x_legend, float y_legend)
{
  TCanvas* c = new TCanvas("c","c",800,800);

  TGraphMultiErrors* g_inc = build_statsys_graph(ratio_inc,ratio_inc_err,std::string(ratio_inc->GetName())+"_graph",title);
  TGraphMultiErrors* g_pos = build_statsys_graph(ratio_pos,ratio_pos_err,std::string(ratio_pos->GetName())+"_pos_graph",title);
  TGraphMultiErrors* g_neg = build_statsys_graph(ratio_neg,ratio_neg_err,std::string(ratio_neg->GetName())+"_neg_graph",title);

  formatDefault_graph(g_inc,kBlack);
  formatDefault_graph(g_pos,kRed);
  formatDefault_graph(g_neg,kBlue);

  TMultiGraph* mg = new TMultiGraph();
  mg->Add(g_inc,"APS; Z ; 5 s=0.5");
  mg->Add(g_pos,"APS; Z ; 5 s=0.5");
  mg->Add(g_neg,"APS; Z ; 5 s=0.5");

  mg->GetYaxis()->SetRangeUser(0.,1.3*std::max(ratio_pos->GetMaximum(),ratio_neg->GetMaximum()));

  mg->Draw("APS; Z ; 5 s=0.5");

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

void plot_truth_vsreco(TH1F* truth, TH1F* reco, std::vector<TH1F*> reco_syserr, std::string title, std::string filename, float x_legend, float y_legend)
{
  TCanvas* c = new TCanvas("c","c",800,800);

  TGraphMultiErrors* g = build_statsys_graph(reco,reco_syserr,std::string(reco->GetName())+"_graph",title);

  formatDefault(truth,kBlack);
  formatDefault_graph(g,kRed);

  g->GetYaxis()->SetRangeUser(0.,1.1*std::max(truth->GetMaximum(),reco->GetMaximum()));

  g->Draw("APS ; Z ; 5 s=0.5");
  truth->Draw("SAME");

  float x_legend_high = std::min(x_legend+0.25,0.9);
  float y_legend_high = std::min(y_legend+0.25,0.9);

  TLegend* ltr = new TLegend(x_legend,y_legend,x_legend_high,y_legend_high);
  ltr->AddEntry(truth,"truth");
  ltr->AddEntry(reco,"reco");

  c->SaveAs(filename.c_str());
  c->Close(); 
}

void plot_correction_sequence(TH1F* truth, TH1F* reco, TH1F* geo_acc, TH1F* cut_eff, TH1F* yield_err, TH1F* geo_acc_err, TH1F* cut_eff_err, std::string title, std::string filename, float x_legend, float y_legend)
{
  TCanvas* c = new TCanvas("c","c",800,800);

//  truth->Sumw2();
//  reco->Sumw2();
//  geo_acc->Sumw2();
//  cut_eff->Sumw2();

  TH1F* reco_geo = (TH1F*)reco->Clone((std::string(reco->GetName())+"_geo").c_str());
  TH1F* reco_geo_cut = (TH1F*)reco->Clone((std::string(reco->GetName())+"_geo_cut").c_str());

  formatDefault(truth,kBlack);
  //formatDefault(reco,kRed);
  //formatDefault(reco_geo,kBlue);
  //formatDefault(reco_geo_cut,kGreen);

  reco_geo->Divide(geo_acc);
  reco_geo_cut->Divide(geo_acc);
  reco_geo_cut->Divide(cut_eff);

  TGraphMultiErrors* g = build_statsys_graph(reco,{yield_err},std::string(reco->GetName())+"_graph",title);
  TGraphMultiErrors* g_geo = build_statsys_graph(reco_geo,{yield_err,geo_acc_err},std::string(reco_geo->GetName())+"_graph",title);
  TGraphMultiErrors* g_geo_cut = build_statsys_graph(reco_geo_cut,{yield_err,geo_acc_err,cut_eff_err},std::string(reco_geo_cut->GetName())+"_graph",title);

  formatDefault_graph(g,kRed);
  formatDefault_graph(g_geo,kBlue);
  formatDefault_graph(g_geo_cut,kGreen);

  TMultiGraph* mg = new TMultiGraph();
  mg->Add(g,"APS ; Z ; 5 s=0.5");
  mg->Add(g_geo,"APS ; Z ; 5 s=0.5");
  mg->Add(g_geo_cut,"APS ; Z ; 5 s=0.5");

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

  mg->GetYaxis()->SetRangeUser(0.,1.1*std::max(truth->GetMaximum(),reco_geo_cut->GetMaximum()));

  mg->Draw("APS ; Z ; 5 s=0.5");
  truth->Draw("SAME");

  float x_legend_high = std::min(x_legend+0.25,0.9);
  float y_legend_high = std::min(y_legend+0.25,0.9);

  TLegend* lc = new TLegend(x_legend,y_legend,x_legend_high,y_legend_high);
  lc->AddEntry(truth,"truth");
  lc->AddEntry(g,"reco uncorrected");
  lc->AddEntry(g_geo,"reco, geometric acceptance corrected");
  lc->AddEntry(g_geo_cut,"reco, geometric acceptance and cut efficiency corrected");
  lc->Draw();

  c->SaveAs(filename.c_str());
  c->Close();
}

void plot_correction(TH1F* corr, TH1F* err, std::string title, std::string filename)
{
  TCanvas* c = new TCanvas("c","c",800,800);

  std::string hname = corr->GetName();

  TGraphMultiErrors* g = build_statsys_graph(corr,{err},hname+"_graph",title);

  formatDefault_graph(g,kBlack);

  g->SetTitle(title.c_str());
  g->GetYaxis()->SetRangeUser(0.,1.2*corr->GetMaximum());

  g->Draw("APS ; Z ; 5 s=0.5");

  c->SaveAs(filename.c_str());
  c->Close();
}

void plot_inverse_correction_sequence(TH1F* truth, TH1F* reco, TH1F* geo_acc, TH1F* cut_eff, TH1F* yield_syserr, TH1F* geo_acc_syserr, TH1F* cut_eff_syserr, std::string title, std::string filename, float x_legend, float y_legend)
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
  //formatDefault(reco,kRed);
  formatDefault(truth_cut,kViolet);
  formatDefault(truth_cut_geo,kOrange);
  //formatDefault(reco_geo,kBlue);
  //formatDefault(reco_geo_cut,kGreen);

  truth_cut->Multiply(cut_eff);
  truth_cut_geo->Multiply(cut_eff);
  truth_cut_geo->Multiply(geo_acc);

  reco_geo->Divide(geo_acc);
  reco_geo_cut->Divide(geo_acc);
  reco_geo_cut->Divide(cut_eff);

  TGraphMultiErrors* g = build_statsys_graph(reco,{yield_syserr},std::string(reco->GetName())+"_graph",title);
  TGraphMultiErrors* g_geo = build_statsys_graph(reco_geo,{yield_syserr,geo_acc_syserr},std::string(reco_geo->GetName())+"_graph",title);
  TGraphMultiErrors* g_geo_cut = build_statsys_graph(reco_geo_cut,{yield_syserr,geo_acc_syserr,cut_eff_syserr},std::string(reco_geo_cut->GetName())+"_graph",title);

  formatDefault_graph(g,kRed);
  formatDefault_graph(g_geo,kBlue);
  formatDefault_graph(g_geo_cut,kGreen);

  TMultiGraph* mg = new TMultiGraph();
  mg->Add(g,"APS ; Z ; 5 s=0.5");
  mg->Add(g_geo,"APS ; Z ; 5 s=0.5");
  mg->Add(g_geo_cut,"APS ; Z ; 5 s=0.5");

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

  mg->GetYaxis()->SetRangeUser(0.,1.1*truth->GetMaximum());
  mg->Draw("APS ; Z ; 5 s=0.5");

  truth->Draw("SAME");
  truth_cut->Draw("SAME");
  truth_cut_geo->Draw("SAME");

  float x_legend_high = std::min(x_legend+0.4,0.9);
  float y_legend_high = std::min(y_legend+0.2,0.9);

  TLegend* lc = new TLegend(x_legend,y_legend,x_legend_high,y_legend_high);
  lc->AddEntry(truth,"truth");
  lc->AddEntry(g,"reco uncorrected");
  lc->AddEntry(truth_cut,"truth after cut efficiency");
  lc->AddEntry(truth_cut_geo,"truth after cut efficiency and geometric acceptance");
  lc->AddEntry(g_geo,"reco after geometric acceptance correction");
  lc->AddEntry(g_geo_cut,"reco after geometric acceptance and cut efficiency correction");
  lc->Draw();

  c->SaveAs(filename.c_str());
  c->Close();
}

void plot_correction_closure(TH1F* numerator_geoacc, TH1F* denominator_cuteff, std::string title, std::string filename, float x_legend, float y_legend)
{
  TCanvas* c = new TCanvas("c","c",800,800);

  formatDefault(numerator_geoacc,kBlue);
  formatDefault(denominator_cuteff,kGreen);

  numerator_geoacc->SetTitle(title.c_str());
  numerator_geoacc->SetMinimum(0.);
  numerator_geoacc->SetMaximum(1.2*std::max(numerator_geoacc->GetMaximum(),denominator_cuteff->GetMaximum()));

  numerator_geoacc->Draw();
  denominator_cuteff->Draw("SAME");

  float x_legend_high = std::min(x_legend+0.6,0.9);
  float y_legend_high = std::min(y_legend+0.15,0.9);

  TLegend* lc = new TLegend(x_legend,y_legend,x_legend_high,y_legend_high);
  lc->AddEntry(numerator_geoacc,"candidates passing geometric acceptance cut");
  lc->AddEntry(denominator_cuteff,"truth candidates in cut efficiency");
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

  TFile* fgeo_err = TFile::Open("/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/geometricAcceptanceCorrection/corrections/geo_acceptance_inclusive_syserr.root");
  TFile* fgeo_err_pos = TFile::Open("/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/geometricAcceptanceCorrection/corrections/geo_acceptance_lambdaonly_syserr.root");
  TFile* fgeo_err_neg = TFile::Open("/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/geometricAcceptanceCorrection/corrections/geo_acceptance_lambdabaronly_syserr.root");

  TFile* fcut_err = TFile::Open("/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/cutEfficiencyCorrection/cut_efficiency_correction_syserr.root");
  TFile* fcut_err_pos = TFile::Open("/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/cutEfficiencyCorrection/cut_efficiency_correction_pos_syserr.root");
  TFile* fcut_err_neg = TFile::Open("/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/cutEfficiencyCorrection/cut_efficiency_correction_neg_syserr.root");

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

    TH1F* lambda_reco_inc = (TH1F*)freco->Get(("Lambda0_yield_vs"+vname).c_str());
    TH1F* lambda_reco_pos = (TH1F*)freco_pos->Get(("Lambda0_yield_vs"+vname).c_str());
    TH1F* lambda_reco_neg = (TH1F*)freco_neg->Get(("Lambda0_yield_vs"+vname).c_str());

    TH1F* lambda_reco_inc_err = (TH1F*)freco->Get(("Lambda0_fit_syserr_vs"+vname).c_str());
    TH1F* lambda_reco_pos_err = (TH1F*)freco_pos->Get(("Lambda0_fit_syserr_vs"+vname).c_str());
    TH1F* lambda_reco_neg_err = (TH1F*)freco_neg->Get(("Lambda0_fit_syserr_vs"+vname).c_str());

    plot_lambda(ftruth,ftruth_pos,ftruth_neg,"hn_vs"+vname,"#Lambda yield vs. "+vtitle+" from truth",
                "plots/yields/lambda_yields_truth_vs"+vname+".png",0.65,0.77,false);
    plot_lambda(ftruth,ftruth_pos,ftruth_neg,"hn_vs"+vname,"#Lambda yield vs. "+vtitle+" from truth (scaled by bin width)",
                "plots/yields/lambda_yields_truth_scaled_vs"+vname+".png",0.65,0.77,true);
    plot_lambda(ftruth,ftruth_pos,ftruth_neg,"hn_wd_vs"+vname,"#Lambda yield vs. "+vtitle+" from truth, with all reco daughters reconstructed",
                "plots/yields/lambda_yields_wd_truth_vs"+vname+".png",0.65,0.77,false);
    plot_lambda(ftruth,ftruth_pos,ftruth_neg,"hn_wd_vs"+vname,"#Lambda yield vs. "+vtitle+" from truth, with all reco daughters reconstructed (scaled by bin width)","plots/yields/lambda_yields_wd_truth_scaled_vs"+vname+".png",0.65,0.77,true);

    plot_lambda_reco(lambda_reco_inc,lambda_reco_pos,lambda_reco_neg,{lambda_reco_inc_err},{lambda_reco_pos_err},{lambda_reco_neg_err},"#Lambda yield (uncorrected) vs. "+vtitle+" from reco","plots/yields/lambda_yields_reco_vs"+vname+".png",0.65,0.77);

    lambda_reco_inc->Scale(1.,"width");
    lambda_reco_pos->Scale(1.,"width");
    lambda_reco_neg->Scale(1.,"width");

    plot_lambda_reco(lambda_reco_inc,lambda_reco_pos,lambda_reco_neg,{lambda_reco_inc_err},{lambda_reco_pos_err},{lambda_reco_neg_err},"#Lambda yield (uncorrected) vs. "+vtitle+" from reco (scaled by bin width)","plots/yields/lambda_yields_reco_scaled_vs"+vname+".png",0.65,0.77);

//    plot_lambda(fdata,fdata_pos,fdata_neg,"Lambda0_yield_vs"+vname,"#Lambda yield (uncorrected) vs. "+vtitle+" from data",
//                "plots/yields/lambda_yields_data_vs"+vname+".png",0.65,0.77,false);
//    plot_lambda(fdata,fdata_pos,fdata_neg,"Lambda0_yield_vs"+vname,"#Lambda yield (uncorrected) vs. "+vtitle+" from data (scaled by bin width)",
//                "plots/yields/lambda_yields_data_scaled_vs"+vname+".png",0.65,0.77,true);

    // Ks yields

    TH1F* Kshort_reco_inc = (TH1F*)freco->Get(("K_S0_yield_vs"+vname).c_str());
    TH1F* Kshort_reco_inc_err = (TH1F*)freco->Get(("K_S0_fit_syserr_vs"+vname).c_str()); 

    plot_ks(ftruth,"hd_vs"+vname,"K_{S} yield vs. "+vtitle+" from truth","plots/yields/ks_yield_truth_vs"+vname+".png",false);
    plot_ks(ftruth,"hd_vs"+vname,"K_{S} yield vs. "+vtitle+" from truth (scaled by bin width)","plots/yields/ks_yield_truth_scaled_vs"+vname+".png",true);
    plot_ks(ftruth,"hd_wd_vs"+vname,"K_{S} yield vs. "+vtitle+" from truth, with all reco daughters reconstructed","plots/yields/ks_yield_wd_truth_vs"+vname+".png",false);
    plot_ks(ftruth,"hd_wd_vs"+vname,"K_{S} yield vs. "+vtitle+" from truth, with all reco daughters reconstructed (scaled by bin width)","plots/yields/ks_yield_wd_truth_scaled_vs"+vname+".png",true);

    plot_ks_reco(Kshort_reco_inc,{Kshort_reco_inc_err},"K_{S} yield vs. "+vtitle+" from reco","plots/yields/ks_yield_reco_vs"+vname+".png");

    Kshort_reco_inc->Scale(1.,"width");

    plot_ks_reco(Kshort_reco_inc,{Kshort_reco_inc_err},"K_{S} yield vs. "+vtitle+" from reco (scaled by bin width)","plots/yields/ks_yield_reco_scaled_vs"+vname+".png");
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
    std::string reco_fit_syserr_name = "ratio_fit_syserr_vs"+vname;
    std::string reco_geoacc_syserr_name = "Lambda0_over_K_S0_geo_acceptance_correction_syserr_vs"+vname;
    std::string reco_cuteff_syserr_name = "Lambda0_over_K_S0_cuteff_correction_syserr_vs"+vname;

    TH1F* ratio_reco_inc = (TH1F*)freco->Get(reco_uncorrected_name.c_str());
    TH1F* ratio_reco_pos = (TH1F*)freco_pos->Get(reco_uncorrected_name.c_str());
    TH1F* ratio_reco_neg = (TH1F*)freco_neg->Get(reco_uncorrected_name.c_str());
    TH1F* ratio_reco_corr_inc = (TH1F*)freco->Get(reco_corrected_name.c_str());
    TH1F* ratio_reco_corr_pos = (TH1F*)freco_pos->Get(reco_corrected_name.c_str());
    TH1F* ratio_reco_corr_neg = (TH1F*)freco_neg->Get(reco_corrected_name.c_str());

    TH1F* ratio_reco_fit_inc_err = (TH1F*)freco->Get(reco_fit_syserr_name.c_str());
    TH1F* ratio_reco_fit_pos_err = (TH1F*)freco_pos->Get(reco_fit_syserr_name.c_str());
    TH1F* ratio_reco_fit_neg_err = (TH1F*)freco_neg->Get(reco_fit_syserr_name.c_str());

    TH1F* ratio_reco_geoacc_inc_err = (TH1F*)fgeo_err->Get(reco_geoacc_syserr_name.c_str());
    TH1F* ratio_reco_geoacc_pos_err = (TH1F*)fgeo_err_pos->Get(reco_geoacc_syserr_name.c_str());
    TH1F* ratio_reco_geoacc_neg_err = (TH1F*)fgeo_err_neg->Get(reco_geoacc_syserr_name.c_str());

    TH1F* ratio_reco_cuteff_inc_err = (TH1F*)fcut_err->Get(reco_cuteff_syserr_name.c_str());
    TH1F* ratio_reco_cuteff_pos_err = (TH1F*)fcut_err_pos->Get(reco_cuteff_syserr_name.c_str());
    TH1F* ratio_reco_cuteff_neg_err = (TH1F*)fcut_err_neg->Get(reco_cuteff_syserr_name.c_str());

    std::vector<TH1F*> ratio_reco_inc_err = {ratio_reco_fit_inc_err,ratio_reco_geoacc_inc_err,ratio_reco_cuteff_inc_err};
    std::vector<TH1F*> ratio_reco_pos_err = {ratio_reco_fit_pos_err,ratio_reco_geoacc_pos_err,ratio_reco_cuteff_pos_err};
    std::vector<TH1F*> ratio_reco_neg_err = {ratio_reco_fit_neg_err,ratio_reco_geoacc_neg_err,ratio_reco_cuteff_neg_err};

/*
    TH1F* ratio_data_inc = (TH1F*)fdata->Get(reco_uncorrected_name.c_str());
    TH1F* ratio_data_pos = (TH1F*)fdata_pos->Get(reco_uncorrected_name.c_str());
    TH1F* ratio_data_neg = (TH1F*)fdata_neg->Get(reco_uncorrected_name.c_str());
    TH1F* ratio_data_corr_inc = (TH1F*)fdata->Get(reco_corrected_name.c_str());
    TH1F* ratio_data_corr_pos = (TH1F*)fdata_pos->Get(reco_corrected_name.c_str());
    TH1F* ratio_data_corr_neg = (TH1F*)fdata_neg->Get(reco_corrected_name.c_str());
*/

    // from Pythia decay tables
    double lambda_BR = 1.;//0.6391668;
    double Ks_BR = 1.;//0.6922420;

    for(TH1F* h : {ratio_reco_inc, ratio_reco_pos, ratio_reco_neg, ratio_reco_corr_inc, ratio_reco_corr_pos, ratio_reco_corr_neg})
//                   ratio_data_inc, ratio_data_pos, ratio_data_neg, ratio_data_corr_inc, ratio_data_corr_pos, ratio_data_corr_neg})
    {
      h->Scale(Ks_BR/lambda_BR);
    }

    plot_ratio(ratio_truth,ratio_truth_pos,ratio_truth_neg,"Ratio comparison vs. "+vtitle+" from truth","plots/ratios/ratio_truth_vs"+vname+".png",0.55,0.15);
    plot_ratio_reco(ratio_reco_inc,ratio_reco_pos,ratio_reco_neg,ratio_reco_inc_err,ratio_reco_pos_err,ratio_reco_neg_err,"Ratio comparison vs. "+vtitle+" (uncorrected) from reco","plots/ratios/ratio_reco_vs"+vname+".png",0.15,0.75);
    plot_ratio_reco(ratio_reco_corr_inc,ratio_reco_corr_pos,ratio_reco_corr_neg,ratio_reco_inc_err,ratio_reco_pos_err,ratio_reco_neg_err,"Ratio comparison vs. "+vtitle+" (fully corrected) from reco","plots/ratios/ratio_reco_corr_vs"+vname+".png",0.65,0.77);
//    plot_ratio(ratio_data_inc,ratio_data_pos,ratio_data_neg,"Ratio comparison vs. "+vtitle+" (uncorrected) from data","plots/ratios/ratio_data_vs"+vname+".png",0.65,0.77);
//    plot_ratio(ratio_data_corr_inc,ratio_data_corr_pos,ratio_data_corr_neg,"Ratio comparison vs. "+vtitle+" (fully corrected) from data","plots/ratios/ratio_data_corr_vs"+vname+".png",0.65,0.77);

    // Truth-reco closure comparisons
    plot_truth_vsreco(ratio_truth,ratio_reco_corr_inc,ratio_reco_inc_err,"(#Lambda+#bar{#Lambda})/2K_{S}^{0} ratio","plots/closure/closure_ratio_vs"+vname+".png",0.55,0.15);
    plot_truth_vsreco(ratio_truth_pos,ratio_reco_corr_pos,ratio_reco_pos_err,"#Lambda/K_{S}^{0} ratio","plots/closure/closure_ratio_pos_vs"+vname+".png",0.55,0.15);
    plot_truth_vsreco(ratio_truth_neg,ratio_reco_corr_neg,ratio_reco_neg_err,"#bar{#Lambda}/K_{S}^{0} ratio","plots/closure/closure_ratio_neg_vs"+vname+".png",0.55,0.15);

    TH1F* lambda_passing_geoacc = (TH1F*)fgeo_corr->Get(("Lambda0_passing_candidates_vs"+vname).c_str());
    TH1F* lambda_passing_geoacc_pos = (TH1F*)fgeo_corr_pos->Get(("Lambda0_passing_candidates_vs"+vname).c_str());
    TH1F* lambda_passing_geoacc_neg = (TH1F*)fgeo_corr_neg->Get(("Lambda0_passing_candidates_vs"+vname).c_str());
    TH1F* kshort_passing_geoacc = (TH1F*)fgeo_corr->Get(("K_S0_passing_candidates_vs"+vname).c_str());

    TH1F* lambda_truth_cuteff = (TH1F*)fcut_corr->Get(("Lambda0_truth_vs"+vname).c_str());
    TH1F* lambda_truth_cuteff_pos = (TH1F*)fcut_corr_pos->Get(("Lambda0_truth_vs"+vname).c_str());
    TH1F* lambda_truth_cuteff_neg = (TH1F*)fcut_corr_neg->Get(("Lambda0_truth_vs"+vname).c_str());
    TH1F* kshort_truth_cuteff = (TH1F*)fcut_corr->Get(("K_S0_truth_vs"+vname).c_str());

    lambda_passing_geoacc->Scale(1.,"width");
    lambda_passing_geoacc_pos->Scale(1.,"width");
    lambda_passing_geoacc_neg->Scale(1.,"width");
    kshort_passing_geoacc->Scale(1.,"width");

    lambda_truth_cuteff->Scale(1.,"width");
    lambda_truth_cuteff_pos->Scale(1.,"width");
    lambda_truth_cuteff_neg->Scale(1.,"width");
    kshort_truth_cuteff->Scale(1.,"width");

    plot_correction_closure(lambda_passing_geoacc,lambda_truth_cuteff,"(#Lambda+#bar{#Lambda}) correction closure","plots/corrections/lambda_correction_closure_vs"+vname+".png",0.25,0.15);
    plot_correction_closure(lambda_passing_geoacc_pos,lambda_truth_cuteff_pos,"#Lambda correction closure","plots/corrections/lambda_pos_correction_closure_vs"+vname+".png",0.25,0.15);
    plot_correction_closure(lambda_passing_geoacc_neg,lambda_truth_cuteff_neg,"#bar{#Lambda} correction closure","plots/corrections/lambda_neg_correction_closure_vs"+vname+".png",0.25,0.15);
    plot_correction_closure(kshort_passing_geoacc,kshort_truth_cuteff,"K_{S}^{0} correction closure","plots/corrections/kshort_correction_closure_vs"+vname+".png",0.35,0.15);

    // Direct yield comparisons
    TH1F* lambda_geo_acceptance = (TH1F*)fgeo_corr->Get(("Lambda0_acceptance_vs"+vname).c_str());
    TH1F* lambda_geo_acceptance_pos = (TH1F*)fgeo_corr_pos->Get(("Lambda0_acceptance_vs"+vname).c_str());
    TH1F* lambda_geo_acceptance_neg = (TH1F*)fgeo_corr_neg->Get(("Lambda0_acceptance_vs"+vname).c_str());

    TH1F* lambda_geo_acceptance_syserr = (TH1F*)fgeo_err->Get(("Lambda0_acceptance_syserr_vs"+vname).c_str());
    TH1F* lambda_geo_acceptance_pos_syserr = (TH1F*)fgeo_err_pos->Get(("Lambda0_acceptance_syserr_vs"+vname).c_str());
    TH1F* lambda_geo_acceptance_neg_syserr = (TH1F*)fgeo_err_neg->Get(("Lambda0_acceptance_syserr_vs"+vname).c_str());

    plot_correction(lambda_geo_acceptance,lambda_geo_acceptance_syserr,"(#Lambda+#bar{#Lambda}) geometric acceptance","plots/corrections/lambda_geo_acceptance_vs"+vname+".png");
    plot_correction(lambda_geo_acceptance_pos,lambda_geo_acceptance_pos_syserr,"#Lambda geometric acceptance","plots/corrections/lambda_pos_geo_acceptance_vs"+vname+".png");
    plot_correction(lambda_geo_acceptance_neg,lambda_geo_acceptance_neg_syserr,"#bar{#Lambda} geometric acceptance","plots/corrections/lambda_neg_geo_acceptance_vs"+vname+".png");

    TH1F* lambda_cut_efficiency = (TH1F*)fcut_corr->Get(("Lambda0_cuteff_vs"+vname).c_str());
    TH1F* lambda_cut_efficiency_pos = (TH1F*)fcut_corr_pos->Get(("Lambda0_cuteff_vs"+vname).c_str());
    TH1F* lambda_cut_efficiency_neg = (TH1F*)fcut_corr_neg->Get(("Lambda0_cuteff_vs"+vname).c_str());

    TH1F* lambda_cut_efficiency_syserr = (TH1F*)fcut_err->Get(("Lambda0_cuteff_syserr_vs"+vname).c_str());
    TH1F* lambda_cut_efficiency_pos_syserr = (TH1F*)fcut_err_pos->Get(("Lambda0_cuteff_syserr_vs"+vname).c_str());
    TH1F* lambda_cut_efficiency_neg_syserr = (TH1F*)fcut_err_neg->Get(("Lambda0_cuteff_syserr_vs"+vname).c_str());

    plot_correction(lambda_cut_efficiency,lambda_cut_efficiency_syserr,"(#Lambda+#bar{#Lambda}) cut efficiency","plots/corrections/lambda_cut_efficiency_vs"+vname+".png");
    plot_correction(lambda_cut_efficiency_pos,lambda_cut_efficiency_pos_syserr,"#Lambda cut efficiency","plots/corrections/lambda_pos_cut_efficiency_vs"+vname+".png");
    plot_correction(lambda_cut_efficiency_neg,lambda_cut_efficiency_neg_syserr,"#bar{#Lambda} cut efficiency","plots/corrections/lambda_neg_cut_efficiency_vs"+vname+".png");

    lambda_truth->Scale(1.,"width");
    lambda_truth_pos->Scale(1.,"width");
    lambda_truth_neg->Scale(1.,"width");

    TH1F* Kshort_geo_acceptance = (TH1F*)fgeo_corr->Get(("K_S0_acceptance_vs"+vname).c_str());
    TH1F* Kshort_geo_acceptance_syserr = (TH1F*)fgeo_err->Get(("K_S0_acceptance_syserr_vs"+vname).c_str());
    TH1F* Kshort_cut_efficiency = (TH1F*)fcut_corr->Get(("K_S0_cuteff_vs"+vname).c_str());
    TH1F* Kshort_cut_efficiency_syserr = (TH1F*)fcut_err->Get(("K_S0_cuteff_syserr_vs"+vname).c_str());

    plot_correction(Kshort_geo_acceptance,Kshort_geo_acceptance_syserr,"K_{S}^{0} geometric acceptance","plots/corrections/ks_geo_acceptance_vs"+vname+".png");
    plot_correction(Kshort_cut_efficiency,Kshort_cut_efficiency_syserr,"K_{S}^{0} cut efficiency","plots/corrections/ks_cut_efficiency_vs"+vname+".png");

    TH1F* geoacc_corr = (TH1F*)fgeo_corr->Get(("Lambda0_over_K_S0_geo_acceptance_correction_vs"+vname).c_str());
    TH1F* geoacc_corr_pos = (TH1F*)fgeo_corr_pos->Get(("Lambda0_over_K_S0_geo_acceptance_correction_vs"+vname).c_str());
    TH1F* geoacc_corr_neg = (TH1F*)fgeo_corr_neg->Get(("Lambda0_over_K_S0_geo_acceptance_correction_vs"+vname).c_str());
    TH1F* cuteff_corr = (TH1F*)fcut_corr->Get(("Lambda0_over_K_S0_cuteff_correction_vs"+vname).c_str());
    TH1F* cuteff_corr_pos = (TH1F*)fcut_corr->Get(("Lambda0_over_K_S0_cuteff_correction_vs"+vname).c_str());
    TH1F* cuteff_corr_neg = (TH1F*)fcut_corr->Get(("Lambda0_over_K_S0_cuteff_correction_vs"+vname).c_str());

    plot_correction(geoacc_corr,ratio_reco_geoacc_inc_err,"(#Lambda+#bar{#Lambda})/2K_{S}^{0} geometric acceptance correction","plots/corrections/ratio_geo_acceptance_vs"+vname+".png");
    plot_correction(geoacc_corr_pos,ratio_reco_geoacc_pos_err,"#Lambda/K_{S}^{0} geometric acceptance correction","plots/corrections/ratio_geo_acceptance_pos_vs"+vname+".png");
    plot_correction(geoacc_corr_neg,ratio_reco_geoacc_neg_err,"#bar{#Lambda}/K_{S}^{0} geometric acceptance correction","plots/corrections/ratio_geoacceptance_neg_vs"+vname+".png");
    plot_correction(cuteff_corr,ratio_reco_cuteff_inc_err,"(#Lambda+#bar{#Lambda})/2K_{S}^{0} cut efficiency correction","plots/corrections/ratio_cuteff_vs"+vname+".png");
    plot_correction(cuteff_corr_pos,ratio_reco_cuteff_pos_err,"#Lambda/K_{S}^{0} cut efficiency correction","plots/corrections/ratio_cuteff_pos_vs"+vname+".png");
    plot_correction(cuteff_corr_neg,ratio_reco_cuteff_neg_err,"#bar{#Lambda}/K_{S}^{0} cut efficiency correction","plots/corrections/ratio_cuteff_neg_vs"+vname+".png");

    ks_truth->Scale(1.,"width");

    Kshort_reco_inc->Scale(1./Ks_BR);
    lambda_reco_inc->Scale(1./lambda_BR);
    lambda_reco_pos->Scale(1./lambda_BR);
    lambda_reco_neg->Scale(1./lambda_BR);

    plot_correction_sequence(lambda_truth,lambda_reco_inc,lambda_geo_acceptance,lambda_cut_efficiency,lambda_reco_inc_err,lambda_geo_acceptance_syserr,lambda_cut_efficiency_syserr,"(#Lambda+#bar{#Lambda}) yield","plots/closure/closure_lambda_corrections_vs"+vname+".png",0.55,0.15);
    plot_correction_sequence(lambda_truth_pos,lambda_reco_pos,lambda_geo_acceptance_pos,lambda_cut_efficiency_pos,lambda_reco_pos_err,lambda_geo_acceptance_pos_syserr,lambda_cut_efficiency_pos_syserr,"#Lambda yield","plots/closure/closure_lambda_pos_corrections_vs"+vname+".png",0.55,0.15);
    plot_correction_sequence(lambda_truth_neg,lambda_reco_neg,lambda_geo_acceptance_neg,lambda_cut_efficiency_neg,lambda_reco_neg_err,lambda_geo_acceptance_neg_syserr,lambda_cut_efficiency_neg_syserr,"#bar{#Lambda} yield","plots/closure/closure_lambda_neg_corrections_vs"+vname+".png",0.55,0.15);
    plot_correction_sequence(ks_truth,Kshort_reco_inc,Kshort_geo_acceptance,Kshort_cut_efficiency,Kshort_reco_inc_err,Kshort_geo_acceptance_syserr,Kshort_cut_efficiency_syserr,"K_{S}^{0} yield","plots/closure/closure_kshort_corrections_vs"+vname+".png",0.55,0.15);

    plot_inverse_correction_sequence(lambda_truth,lambda_reco_inc,lambda_geo_acceptance,lambda_cut_efficiency,lambda_reco_inc_err,lambda_geo_acceptance_syserr,lambda_cut_efficiency_syserr,"(#Lambda+#bar{#Lambda}) yield","plots/closure/inverse_closure_lambda_corrections_vs"+vname+".png",0.45,0.2);
    plot_inverse_correction_sequence(lambda_truth_pos,lambda_reco_pos,lambda_geo_acceptance_pos,lambda_cut_efficiency_pos,lambda_reco_pos_err,lambda_geo_acceptance_pos_syserr,lambda_cut_efficiency_pos_syserr,"#Lambda yield","plots/closure/inverse_closure_lambda_pos_corrections_vs"+vname+".png",0.45,0.2);
    plot_inverse_correction_sequence(lambda_truth_neg,lambda_reco_neg,lambda_geo_acceptance_neg,lambda_cut_efficiency_neg,lambda_reco_neg_err,lambda_geo_acceptance_neg_syserr,lambda_cut_efficiency_neg_syserr,"#bar{#Lambda} yield","plots/closure/inverse_closure_lambda_neg_corrections_vs"+vname+".png",0.45,0.2);
    plot_inverse_correction_sequence(ks_truth,Kshort_reco_inc,Kshort_geo_acceptance,Kshort_cut_efficiency,Kshort_reco_inc_err,Kshort_geo_acceptance_syserr,Kshort_cut_efficiency_syserr,"K_{S}^{0} yield","plots/closure/inverse_closure_kshort_corrections_vs"+vname+".png",0.45,0.2);
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
