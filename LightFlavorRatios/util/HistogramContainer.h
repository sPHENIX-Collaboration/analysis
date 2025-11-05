#ifndef HISTOGRAMCONTAINER_H
#define HISTOGRAMCONTAINER_H

#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"

#include "binning.h"

struct HistogramContainer
{
  TH1F* h_pt;
  TH1F* h_y;
  TH1F* h_phi;
  TH1F* h_ntracks;

  TH2F* h_pt_y;
  TH2F* h_pt_phi;
  TH2F* h_pt_ntracks;
  TH2F* h_y_phi;
  TH2F* h_y_ntracks;
  TH2F* h_phi_ntracks;

  std::string legend_name;

  // a bit of home-grown reflection, for ease of plot-making
  std::map<std::string,TH1F*> spectra;
  std::map<std::string,TH2F*> correlations;

  void InitMaps()
  {
    spectra = {
      { "pt", h_pt },
      { "y", h_y },
      { "phi", h_phi },
      { "ntracks", h_ntracks }
    };

    correlations = {
      { "pt_y", h_pt_y },
      { "pt_phi", h_pt_phi },
      { "pt_ntracks", h_pt_ntracks },
      { "y_phi", h_y_phi },
      { "y_ntracks", h_y_ntracks },
      { "phi_ntracks", h_phi_ntracks }
    };
  }

  HistogramContainer(std::string basename, std::string basetitle)
  {
    h_pt = makeHistogram(basename, basetitle, BinInfo::pt_bins);
    h_y = makeHistogram(basename, basetitle, BinInfo::rapidity_bins);
    h_phi = makeHistogram(basename, basetitle, BinInfo::phi_bins);
    h_ntracks = makeHistogram(basename, basetitle, BinInfo::ntrack_bins);

    h_pt_y = make2DHistogram(basename, basetitle, BinInfo::pt_bins, BinInfo::rapidity_bins);
    h_pt_phi = make2DHistogram(basename, basetitle, BinInfo::pt_bins, BinInfo::phi_bins);
    h_pt_ntracks = make2DHistogram(basename, basetitle, BinInfo::pt_bins, BinInfo::ntrack_bins);
    h_y_phi = make2DHistogram(basename, basetitle, BinInfo::rapidity_bins, BinInfo::phi_bins);
    h_y_ntracks = make2DHistogram(basename, basetitle, BinInfo::rapidity_bins, BinInfo::ntrack_bins);
    h_phi_ntracks = make2DHistogram(basename, basetitle, BinInfo::phi_bins, BinInfo::ntrack_bins);

    InitMaps();
    SetStyle();
  }

  HistogramContainer(TFile* f, std::string basename, std::string lstring)
  {
    Load(f,basename);
    legend_name = lstring;
    InitMaps();
    SetStyle();
  }

  void Load(TFile* f, std::string basename)
  {
    std::string ptname = BinInfo::pt_bins.name;
    std::string yname = BinInfo::rapidity_bins.name;
    std::string phiname = BinInfo::phi_bins.name;
    std::string ntrkname = BinInfo::ntrack_bins.name;

    h_pt = (TH1F*)f->Get((basename+ptname).c_str());
    h_y = (TH1F*)f->Get((basename+yname).c_str());
    h_phi = (TH1F*)f->Get((basename+phiname).c_str());
    h_ntracks = (TH1F*)f->Get((basename+ntrkname).c_str());

    h_pt_y = (TH2F*)f->Get((basename+ptname+yname).c_str());
    h_pt_phi = (TH2F*)f->Get((basename+ptname+phiname).c_str());
    h_pt_ntracks = (TH2F*)f->Get((basename+ptname+ntrkname).c_str());
    h_y_phi = (TH2F*)f->Get((basename+yname+phiname).c_str());
    h_y_ntracks = (TH2F*)f->Get((basename+yname+ntrkname).c_str());
    h_phi_ntracks = (TH2F*)f->Get((basename+phiname+ntrkname).c_str());
  }

  void SetColor(Color_t color)
  {
    h_pt->SetLineColor(color);
    h_y->SetLineColor(color);
    h_phi->SetLineColor(color);
    h_ntracks->SetLineColor(color);

    h_pt->SetMarkerColor(color);
    h_y->SetMarkerColor(color);
    h_phi->SetMarkerColor(color);
    h_ntracks->SetMarkerColor(color);
  }

  void SetStyle()
  {
    h_pt->SetMarkerStyle(kFullCircle);
    h_y->SetMarkerStyle(kFullCircle);
    h_phi->SetMarkerStyle(kFullCircle);
    h_ntracks->SetMarkerStyle(kFullCircle);

    h_pt->SetMarkerSize(0.7);
    h_y->SetMarkerSize(0.7);
    h_phi->SetMarkerSize(0.7);
    h_ntracks->SetMarkerSize(0.7);
  }

  void Fill(float pt, float y, float phi, float ntracks)
  {
    h_pt->Fill(pt);
    h_y->Fill(y);
    h_phi->Fill(phi);
    h_ntracks->Fill(ntracks);

    h_pt_y->Fill(pt,y);
    h_pt_phi->Fill(pt,phi);
    h_pt_ntracks->Fill(pt,ntracks);
    h_y_phi->Fill(y,phi);
    h_y_ntracks->Fill(y,ntracks);
    h_phi_ntracks->Fill(phi,ntracks);
  }

  void Write()
  {
    h_pt->Write();
    h_y->Write();
    h_phi->Write();
    h_ntracks->Write();

    h_pt_y->Write();
    h_pt_phi->Write();
    h_pt_ntracks->Write();
    h_y_phi->Write();
    h_y_ntracks->Write();
    h_phi_ntracks->Write();
  }
};

#endif // HISTOGRAMCONTAINER_H
