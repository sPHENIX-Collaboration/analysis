#include "TFile.h"
#include "TH1F.h"

//#include <sPhenixStyle.h>
//#include <sPhenixStyle.C>

#include "../util/DifferentialContainer.h"
#include "../util/HistogramTools.h"
#include "../config/binning.h"

std::vector<TH1F*> get_all_bins_TH1F(TFile* f, const std::string& particle, const HistogramInfo& var)
{
  //DifferentialContainer dc(f,particle,var);
  //return dc.hists;

  std::vector<TH1F*> plot_dist;

  int nbins = var.bins.size()-1;
  for(int i=1; i<=nbins; i++)
  {
    std::string name = particle+"_yield_vs"+var.name+std::to_string(i)+"__"+particle+"_mass";
    std::cout << name << std::endl;
    TH1F* dist = (TH1F*)f->Get(name.c_str());
    plot_dist.push_back(dist);
  }
  std::cout << "final size: " << plot_dist.size() << std::endl;
  return plot_dist;
}

std::vector<RooPlot*> get_all_bins_RooPlot(TFile* f, const std::string& particle, const HistogramInfo& var)
{
  //DifferentialContainer dc(f,particle,var);
  //return dc.hists;

  std::vector<RooPlot*> plot_dist;

  int nbins = var.bins.size()-1;
  for(int i=1; i<=nbins; i++)
  {
    std::string name = particle+"_yield_vs"+var.name+"_"+std::to_string(i);
    std::cout << name << std::endl;
    RooPlot* dist = (RooPlot*)f->Get(name.c_str());
    plot_dist.push_back(dist);
  }
  std::cout << "final size: " << plot_dist.size() << std::endl;
  return plot_dist;
}

std::vector<std::vector<TH1F*>> get_all_fits_all_variables_TH1F(TFile* f, std::string particle, std::vector<HistogramInfo> hinfos)
{
  std::vector<std::vector<TH1F*>> all_fits;
  for(HistogramInfo& hinfo : hinfos)
  {
    all_fits.push_back(get_all_bins_TH1F(f,particle,hinfo));
  }
  return all_fits;
}

std::vector<std::vector<RooPlot*>> get_all_fits_all_variables_RooPlot(TFile* f, std::string particle, std::vector<HistogramInfo> hinfos)
{
  std::vector<std::vector<RooPlot*>> all_fits;
  for(HistogramInfo& hinfo : hinfos)
  {
    all_fits.push_back(get_all_bins_RooPlot(f,particle,hinfo));
  }
  return all_fits;
}

void plot_results(std::string infile = "fits.root", std::string dirname = "plots")
{
  gROOT->SetBatch();

  gStyle->SetOptStat(0);
  //gStyle->SetImageScaling(2.);
  //SetsPhenixStyle();

  bool finalize = false;

  std::string outdir;
  if(finalize) outdir = "/sphenix/tg/tg01/hf/mjpeters/LightFlavorResults/"+dirname;
  else outdir = dirname;

  TFile* f = TFile::Open(infile.c_str());

  std::vector<HistogramInfo> variables =
  {
    BinInfo::final_pt_bins,
    BinInfo::final_eta_bins,
    BinInfo::final_rapidity_bins,
    BinInfo::final_phi_bins
  };

  std::vector<std::vector<RooPlot*>> Ks_fits = get_all_fits_all_variables_RooPlot(f,"K_S0",variables);
  std::vector<std::vector<RooPlot*>> lambda_fits = get_all_fits_all_variables_RooPlot(f,"Lambda0",variables);

  //std::vector<std::vector<TH1F*>> Ks_fits = get_all_fits_all_variables_TH1F(f,"K_S0",variables);
  //std::vector<std::vector<TH1F*>> lambda_fits = get_all_fits_all_variables_TH1F(f,"Lambda0",variables);

  for(int i=0; i<variables.size(); i++)
  {
    // generate square figures
    int nbins = variables[i].bins.size()-1;
    std::cout << "nbins " << nbins << std::endl;
    int npix_x = 1500;
    int npix_y = 750;
    TCanvas* c_lin_ks = new TCanvas("c_lin_ks","c",npix_x,npix_y);
    TCanvas* c_log_ks = new TCanvas("c_log_ks","c",npix_x,npix_y);
    int n_columns = nbins/3;
    if(nbins % 3 == 1 || nbins % 3 == 2) n_columns++;
    c_lin_ks->Divide(n_columns,3);
    c_log_ks->Divide(n_columns,3);
    for(int bin=1; bin<=nbins; bin++)
    {
      c_lin_ks->cd(bin);
      Ks_fits[i][bin-1]->Draw("goff");
      c_log_ks->cd(bin);
      gPad->SetLogy();
      Ks_fits[i][bin-1]->Draw("goff");
    }
    std::string Ks_filename_pdf = outdir+"/pdf/Ks_fits_vs"+variables[i].name+".pdf";
    std::string Ks_filename = outdir+"/png/Ks_fits_vs"+variables[i].name+".png";
    std::string Ks_filename_log_pdf = outdir+"/pdf/Ks_fits_logscale_vs"+variables[i].name+".pdf";
    std::string Ks_filename_log = outdir+"/png/Ks_fits_logscale_vs"+variables[i].name+".png";
    c_lin_ks->SaveAs(Ks_filename.c_str());
    c_lin_ks->SaveAs(Ks_filename_pdf.c_str());
    c_log_ks->SaveAs(Ks_filename_log.c_str());
    c_log_ks->SaveAs(Ks_filename_log_pdf.c_str());

    c_lin_ks->Close();
    c_log_ks->Close();

    TCanvas* c_lin_lambda = new TCanvas("c_lin_lambda","c1",npix_x,npix_y);
    TCanvas* c_log_lambda = new TCanvas("c_log_lambda","c1",npix_x,npix_y);
    c_lin_lambda->Divide(n_columns,3);
    c_log_lambda->Divide(n_columns,3);

    //for(int bin=1; bin<=nbins; bin++)
    //{
    //  c->cd(bin+nbins);
    //  phi_fits[i][bin-1]->Draw();
    //}

    for(int bin=1; bin<=nbins; bin++)
    {
      c_lin_lambda->cd(bin);
      lambda_fits[i][bin-1]->Draw("goff");
      c_log_lambda->cd(bin);
      gPad->SetLogy();
      lambda_fits[i][bin-1]->Draw("goff");
    }
    std::string lambda_filename_pdf = outdir+"/pdf/lambda_fits_vs"+variables[i].name+".pdf";
    std::string lambda_filename = outdir+"/png/lambda_fits_vs"+variables[i].name+".png";
    std::string lambda_filename_log_pdf = outdir+"/pdf/lambda_fits_logscale_vs"+variables[i].name+".pdf";
    std::string lambda_filename_log = outdir+"/png/lambda_fits_logscale_vs"+variables[i].name+".png";
    c_lin_lambda->SaveAs(lambda_filename.c_str());
    c_lin_lambda->SaveAs(lambda_filename_pdf.c_str());
    c_log_lambda->SaveAs(lambda_filename_log.c_str());
    c_log_lambda->SaveAs(lambda_filename_log_pdf.c_str());

    c_lin_lambda->Close();
    c_log_lambda->Close();

    TCanvas* c_pull_ks = new TCanvas("c_pull_ks","c_pull_ks",npix_x,npix_y);
    TCanvas* c_pull_lambda = new TCanvas("c_pull_lambda","c_pull_lambda",npix_x,npix_y);
    TCanvas* c_hpull_ks = new TCanvas("c_hpull_ks","c_hpull_ks",npix_x,npix_y);
    TCanvas* c_hpull_lambda = new TCanvas("c_hpull_lambda","c_hpull_lambda",npix_x,npix_y);
    c_pull_ks->Divide(nbins/3,3);
    c_pull_lambda->Divide(nbins/3,3);
    c_hpull_ks->Divide(nbins/3,3);
    c_hpull_lambda->Divide(nbins/3,3);

    gStyle->SetOptFit(1111);

    for(int bin=1; bin<=nbins; bin++)
    {
      c_pull_ks->cd(bin);
      RooHist* pull_ks = Ks_fits[i][bin-1]->pullHist("h_binned_massfit",Ks_fits[i][bin-1]->getObject(0)->GetName(),true);
      RooPlot* pull_frame_ks = Ks_fits[i][bin-1]->emptyClone(("pull_ks_"+std::to_string(bin)+"_vs"+variables[i].name).c_str());
      pull_frame_ks->addPlotable(pull_ks,"P");
      pull_frame_ks->Draw("goff");

      c_hpull_ks->cd(bin);
      TH1F* hpull_ks = new TH1F("hpull_ks",("Pull distribution for K_{S}^{0} "+variables[i].name+" bin "+std::to_string(bin)).c_str(),pull_ks->GetN()/4,1.1*pull_ks->GetMinimum(),1.1*pull_ks->GetMaximum());
      for(int ip=1;ip<=pull_ks->GetN();ip++)
      {
        hpull_ks->Fill(pull_ks->GetPointY(ip));
      }
      TF1* ks_gaus = new TF1("ks_gaus","gaus");
      hpull_ks->Fit(ks_gaus);
      hpull_ks->Draw();

      c_pull_lambda->cd(bin);
      RooHist* pull_lambda = lambda_fits[i][bin-1]->pullHist("h_binned_massfit",lambda_fits[i][bin-1]->getObject(0)->GetName(),true);
      RooPlot* pull_frame_lambda = lambda_fits[i][bin-1]->emptyClone(("pull_lambda_"+std::to_string(bin)+"_vs"+variables[i].name).c_str());
      pull_frame_lambda->addPlotable(pull_lambda,"P");
      pull_frame_lambda->Draw("goff");

      c_hpull_lambda->cd(bin);
      TH1F* hpull_lambda = new TH1F("hpull_lambda",("Pull distribution for #Lambda "+variables[i].name+" bin "+std::to_string(bin)).c_str(),pull_lambda->GetN()/4,1.1*pull_lambda->GetMinimum(),1.1*pull_lambda->GetMaximum());
      for(int ip=1;ip<=pull_lambda->GetN();ip++)
      {
        hpull_lambda->Fill(pull_lambda->GetPointY(ip));
      }
      TF1* lambda_gaus = new TF1("ks_gaus","gaus");
      hpull_lambda->Fit(lambda_gaus);
      hpull_lambda->Draw();
    }
    std::string Ks_pull_filename_pdf = outdir+"/pdf/Ks_pull_vs"+variables[i].name+".pdf";
    std::string Ks_pull_filename = outdir+"/png/Ks_pull_vs"+variables[i].name+".png";
    std::string lambda_pull_filename_pdf = outdir+"/pdf/lambda_pull_vs"+variables[i].name+".pdf";
    std::string lambda_pull_filename = outdir+"/png/lambda_pull_vs"+variables[i].name+".png";
    std::string Ks_hpull_filename_pdf = outdir+"/pdf/Ks_hpull_vs"+variables[i].name+".pdf";
    std::string Ks_hpull_filename = outdir+"/png/Ks_hpull_vs"+variables[i].name+".png";
    std::string lambda_hpull_filename_pdf = outdir+"/pdf/lambda_hpull_vs"+variables[i].name+".pdf";
    std::string lambda_hpull_filename = outdir+"/png/lambda_hpull_vs"+variables[i].name+".png";
    c_pull_ks->SaveAs(Ks_pull_filename.c_str());
    c_pull_ks->SaveAs(Ks_pull_filename_pdf.c_str());
    c_pull_lambda->SaveAs(lambda_pull_filename.c_str());
    c_pull_lambda->SaveAs(lambda_pull_filename_pdf.c_str());
    c_hpull_ks->SaveAs(Ks_hpull_filename.c_str());
    c_hpull_ks->SaveAs(Ks_hpull_filename_pdf.c_str());
    c_hpull_lambda->SaveAs(lambda_hpull_filename.c_str());
    c_hpull_lambda->SaveAs(lambda_hpull_filename_pdf.c_str());

    c_pull_ks->Close();
    c_pull_lambda->Close();
    c_hpull_ks->Close();
    c_hpull_lambda->Close();
  }

  TCanvas* c = new TCanvas("singleplots","single plots",800,800);
  c->SetLeftMargin(0.15);
  c->SetRightMargin(0.05);
  std::vector<TH1F*> single_plots =
  {
    (TH1F*)f->Get("lambdaKsratio_vspT"),
    (TH1F*)f->Get("lambdaKsratio_vspseudorapidity"),
    (TH1F*)f->Get("lambdaKsratio_vsphi"),
    (TH1F*)f->Get("lambdaKsratio_vsrapidity"),

    (TH1F*)f->Get("K_S0_yield_vspT"),
    (TH1F*)f->Get("K_S0_yield_vspseudorapidity"),
    (TH1F*)f->Get("K_S0_yield_vsphi"),
    (TH1F*)f->Get("K_S0_yield_vsrapidity"),
    (TH1F*)f->Get("Lambda0_yield_vspT"),
    (TH1F*)f->Get("Lambda0_yield_vspseudorapidity"),
    (TH1F*)f->Get("Lambda0_yield_vsphi"),
    (TH1F*)f->Get("Lambda0_yield_vsrapidity"),

  };

  std::vector<std::pair<std::string,std::string>> doubleplot_names = 
  {
    {"lambdaKsratio_vspT","lambdaKsratio_feeddown_acc_eff_corrected_vspT"},
    {"lambdaKsratio_vspseudorapidity","lambdaKsratio_feeddown_acc_eff_corrected_vspseudorapidity"},
    {"lambdaKsratio_vsphi","lambdaKsratio_feeddown_acc_eff_corrected_vsphi"},
    {"lambdaKsratio_vsrapidity","lambdaKsratio_feeddown_acc_eff_corrected_vsrapidity"},
    {"Lambda_yield_vspT","Lambda_yield_vspT_corrected"},
    {"Lambda_yield_vspseudorapidity","Lambda_yield_vspseudorapidity_corrected"},
    {"Lambda_yield_vsrapidity","Lambda_yield_vsrapidity_corrected"},
    {"Lambda_yield_vsphi","Lambda_yield_vsphi_corrected"}
  };

  std::vector<std::pair<std::string,std::string>> correction_types =
  {
    {"","Uncorrected"},
    //{"_acc","Geo. acceptance"},
    //{"_eff","Efficiency"},
    {"_lambdafeeddowncorrected","#Lambda feed-down"},
    //{"_acc_eff","Geo. acceptance + efficiency"},
    //{"_feeddown_acc","Geo. acceptance + #Lambda feed-down"},
    {"_lambdafeeddowncorrected_effcorrected","Efficiency + #Lambda feed-down"},
    {"_lambdafeeddowncorrected_effcorrected_geoacceptancecorrected","Geo. acceptance + efficiency + #Lambda feed-down"},
    {"_lambdafeeddowncorrected_effcorrected_geoacceptancecorrected_cutefficiencycorrected","Geo. acceptance + efficiency +#Lambda feed-down + cut selection efficiency"}
  };

  std::vector<std::vector<std::pair<TH1F*,std::string>>> multiplots;

  for(int i=0; i<variables.size(); i++)
  {
    multiplots.emplace_back();
    for(int j=0; j<correction_types.size(); j++)
    {
      std::pair<std::string,std::string> pr = correction_types[j];
      std::string hname = "lambdaKsratio_vs"+variables[i].name+pr.first;
      std::cout << "fetching " << hname << std::endl;
      TH1F* h = (TH1F*)f->Get(hname.c_str());
      multiplots[i].push_back({h,pr.second});
    }
  }

  std::vector<std::pair<TH1F*,TH1F*>> double_plots;
  for(std::pair<std::string,std::string>& name : doubleplot_names)
  {
    double_plots.push_back(std::make_pair((TH1F*)f->Get(name.first.c_str()),(TH1F*)f->Get(name.second.c_str())));
  }

  for(TH1F* h : single_plots)
  {
    h->SetMarkerColor(kBlack);
    h->SetLineColor(kBlack);
    h->SetMarkerStyle(kFullCircle);
    h->SetMarkerSize(0.7);
    h->SetMinimum(0.);

    h->Draw("goff");

    TLatex latex;
    latex.SetNDC();           // Use normalized coordinates
    latex.SetTextSize(0.04); // Adjust size as needed
    latex.SetTextAlign(13);   // Left-top alignment
    latex.DrawLatex(0.68, 0.93, "#it{#bf{sPHENIX}} internal");
    latex.DrawLatex(0.68, 0.90, "#it{p+p} #sqrt{200} GeV");

    std::string filename_pdf = outdir+"/pdf/"+std::string(h->GetName())+".pdf";
    std::string filename_png = outdir+"/png/"+std::string(h->GetName())+".png";
    c->SaveAs(filename_pdf.c_str());
    c->SaveAs(filename_png.c_str());
  }
/*
  for(std::pair<TH1F*,TH1F*> pair : double_plots)
  {
    pair.first->SetMarkerColor(kBlack);
    pair.first->SetLineColor(kBlack);
    pair.first->SetMarkerStyle(kFullCircle);
    pair.first->SetMarkerSize(0.7);
    pair.first->SetMinimum(0.);

    pair.second->SetMarkerColor(kRed);
    pair.second->SetLineColor(kRed);
    pair.second->SetMarkerStyle(kFullCircle);
    pair.second->SetMarkerSize(0.7);
    pair.second->SetMinimum(0.);
    pair.second->SetMaximum(1.2*std::max(pair.first->GetMaximum(),pair.second->GetMaximum()));

    pair.second->Draw();
    pair.first->Draw("SAME");

    TLatex latex;
    latex.SetNDC();           // Use normalized coordinates
    latex.SetTextSize(0.04); // Adjust size as needed
    latex.SetTextAlign(13);   // Left-top alignment
    latex.DrawLatex(0.68, 0.93, "#it{#bf{sPHENIX}} internal");
    latex.DrawLatex(0.68, 0.90, "#it{p+p} #sqrt{200} GeV");

    TLegend* l = new TLegend(0.2,0.2,0.4,0.3);
    l->AddEntry(pair.first,"Uncorrected","lep");
    l->AddEntry(pair.second,"Corrected","lep");
    l->Draw();

    std::string filename_pdf = outdir+"/pdf/"+std::string(pair.second->GetName())+".pdf";
    std::string filename_png = outdir+"/png/"+std::string(pair.second->GetName())+".png";

    c->SaveAs(filename_pdf.c_str());
    c->SaveAs(filename_png.c_str());
  }
*/
  //gStyle->SetPalette(90);

  std::vector<Color_t> colors = {
    kBlack,
    kRed,
    kOrange,
    kBlue,
    kGreen+2
  };

  for(int i=0; i<multiplots.size(); i++)
  {
    // find max max
    float max_max = 0.;
    for(int j=0; j<multiplots[i].size(); j++)
    {
      float this_max = multiplots[i][j].first->GetMaximum();
      if(this_max>max_max)
      {
        max_max = this_max;
      }
    }
    for(int j=0; j<multiplots[i].size(); j++)
    {
      TH1F* h = multiplots[i][j].first;
      h->SetMinimum(0.);
      h->SetMaximum(1.2*max_max);
      h->SetMarkerSize(0.7);
      h->SetMarkerStyle(kFullCircle);
      h->SetLineColor(colors[j]);
      h->SetMarkerColor(colors[j]);

      if(j==0) h->Draw("goff");
      else h->Draw("SAME goff");
    }

    TLatex latex;
    latex.SetNDC();           // Use normalized coordinates
    latex.SetTextSize(0.04); // Adjust size as needed
    latex.SetTextAlign(13);   // Left-top alignment
    latex.DrawLatex(0.68, 0.93, "#it{#bf{sPHENIX}} internal");
    latex.DrawLatex(0.68, 0.90, "#it{p+p} #sqrt{200} GeV");

    TLegend* l = new TLegend(0.6,0.7,0.95,0.85);
    for(int j=0; j<multiplots[i].size(); j++)
    {
      l->AddEntry(multiplots[i][j].first->GetName(),multiplots[i][j].second.c_str(),"lep");
    }
    l->Draw();

    std::string png_filename = outdir+"/png/ratio_allcorrections_vs"+variables[i].name+".png";
    std::string pdf_filename = outdir+"/pdf/ratio_allcorrections_vs"+variables[i].name+".pdf";

    c->SaveAs(png_filename.c_str());
    c->SaveAs(pdf_filename.c_str());
  }

}
