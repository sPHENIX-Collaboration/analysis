#include "TFile.h"
#include "TH1F.h"

//#include <sPhenixStyle.h>
//#include <sPhenixStyle.C>

#include "../util/DifferentialContainer.h"
#include "../util/binning.h"

std::vector<RooPlot*> get_all_bins(TFile* f, const std::string& particle, const HistogramInfo& var)
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

std::vector<std::vector<RooPlot*>> get_all_fits_all_variables(TFile* f, std::string particle, std::vector<HistogramInfo> hinfos)
{
  std::vector<std::vector<RooPlot*>> all_fits;
  for(HistogramInfo& hinfo : hinfos)
  {
    all_fits.push_back(get_all_bins(f,particle,hinfo));
  }
  return all_fits;
}

void plot_results()
{
  gStyle->SetOptStat(0);
  gStyle->SetImageScaling(2.);
  //SetsPhenixStyle();

  bool finalize = true;

  std::string outdir;
  if(finalize) outdir = "/sphenix/tg/tg01/hf/mjpeters/LightFlavorResults/plots";
  else outdir = "plots";

  TFile* f = TFile::Open("fits.root");

  std::vector<HistogramInfo> variables =
  {
    BinInfo::final_pt_bins,
    BinInfo::final_eta_bins,
    BinInfo::final_rapidity_bins,
    BinInfo::final_phi_bins
  };

  std::vector<std::vector<RooPlot*>> Ks_fits = get_all_fits_all_variables(f,"Ks",variables);
  std::vector<std::vector<RooPlot*>> lambda_fits = get_all_fits_all_variables(f,"Lambda",variables);

  for(int i=0; i<variables.size(); i++)
  {
    // generate square figures
    int nbins = variables[i].bins.size()-1;
    std::cout << "nbins " << nbins << std::endl;
    int npix_x = 3600;
    int npix_y = 1800;
    TCanvas* c = new TCanvas("c","c",npix_x,npix_y);
    c->Divide(nbins/3+1,3);
    for(int bin=1; bin<=nbins; bin++)
    {
      c->cd(bin);
      std::cout << i << " " << bin << std::endl;
      Ks_fits[i][bin-1]->Draw();
    }
    std::string Ks_filename_pdf = outdir+"/pdf/Ks_fits_vs"+variables[i].name+".pdf";
    std::string Ks_filename = outdir+"/png/Ks_fits_vs"+variables[i].name+".png";
    c->SaveAs(Ks_filename.c_str());
    c->SaveAs(Ks_filename_pdf.c_str());
    c->Close();

    TCanvas* c1 = new TCanvas("c1","c1",npix_x,npix_y);
    c1->Divide(nbins/3+1,3);
/*
    for(int bin=1; bin<=nbins; bin++)
    {
      c->cd(bin+nbins);
      phi_fits[i][bin-1]->Draw();
    }
*/
    for(int bin=1; bin<=nbins; bin++)
    {
      c1->cd(bin);
      lambda_fits[i][bin-1]->Draw();
    }
    std::string lambda_filename_pdf = outdir+"/pdf/lambda_fits_vs"+variables[i].name+".pdf";
    std::string lambda_filename = outdir+"/png/lambda_fits_vs"+variables[i].name+".png";
    c1->SaveAs(lambda_filename.c_str());
    c1->SaveAs(lambda_filename_pdf.c_str());
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
    (TH1F*)f->Get("Ks_yield_vspT"),
    (TH1F*)f->Get("Ks_yield_vspseudorapidity"),
    (TH1F*)f->Get("Ks_yield_vsphi"),
    (TH1F*)f->Get("Ks_yield_vsrapidity"),
    (TH1F*)f->Get("Lambda_yield_vspT"),
    (TH1F*)f->Get("Lambda_yield_vspseudorapidity"),
    (TH1F*)f->Get("Lambda_yield_vsphi"),
    (TH1F*)f->Get("Lambda_yield_vsrapidity"),
  };

  for(TH1F* h : single_plots)
  {
    h->SetMarkerColor(kBlack);
    h->SetLineColor(kBlack);
    h->SetMarkerStyle(kFullCircle);
    h->SetMarkerSize(0.7);
    h->SetMinimum(0.);

    h->Draw();

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
}
