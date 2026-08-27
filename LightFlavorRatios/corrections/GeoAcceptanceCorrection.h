#ifndef GEOACCEPTANCECORRECTION_H
#define GEOACCEPTANCECORRECTION_H

#include "CorrectionHistogram1D.h"

#include <TFile.h>
#include <TH1F.h>
#include <TCanvas.h>

#include <string>
#include <memory>

struct GeoAcceptanceCorrection : CorrectionHistogram1D
{
  std::shared_ptr<TFile> f;

  GeoAcceptanceCorrection(std::string filename, std::string hname)
  {
    f = std::make_shared<TFile>(filename.c_str(),"READ");
    TCanvas* c = (TCanvas*)f->Get("myCanvas");
    h_corr = (TH1F*)c->GetPrimitive(hname.c_str())->Clone();
    // preserve histogram on file close
    h_corr->SetDirectory(nullptr);
    name = "geoacceptance";
    title = "geometric acceptance";
  }

  GeoAcceptanceCorrection(const GeoAcceptanceCorrection& g)
  {
    f = g.f;
    h_corr = g.h_corr;
    name = g.name;
    title = g.title;
  }

  void apply_correction(float xlow, float xhigh, TH1F* h, int bin) override
  {
    std::pair<double,double> corr_and_err = get_val_and_error(xlow,xhigh);
    float new_val = h->GetBinContent(bin)/corr_and_err.first;
    float new_err = new_val * sqrt(pow(h->GetBinError(bin)/h->GetBinContent(bin),2.)+pow(corr_and_err.second/corr_and_err.first,2.));
    h->SetBinContent(bin,new_val);
    h->SetBinError(bin,new_err);
  }

};

#endif
