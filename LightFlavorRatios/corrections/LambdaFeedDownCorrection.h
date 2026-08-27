#ifndef LAMBDAFEEDDOWNCORRECTION_H
#define LAMBDAFEEDDOWNCORRECTION_H

#include "CorrectionHistogram1D.h"

#include <TFile.h>

#include <iostream>
#include <string>
#include <memory>

struct LambdaFeedDownCorrection : CorrectionHistogram1D
{
  std::shared_ptr<TFile> f;
  
  LambdaFeedDownCorrection(std::string filename, std::string hname)
  {
    f = std::make_shared<TFile>(filename.c_str(),"READ");
    h_corr = (TH1D*)f->Get(hname.c_str())->Clone();
    // preserve histogram on file close
    h_corr->SetDirectory(nullptr);
    name = "lambdafeeddown";
    title = "#Lambda feeddown";
  }

  LambdaFeedDownCorrection(const LambdaFeedDownCorrection& c)
  {
    f = c.f;
    h_corr = c.h_corr;
    name = c.name;
    title = c.title;
  }

  void apply_correction(float xlow, float xhigh, TH1F* h, int bin) override
  {
    std::pair<double,double> corr_and_err = get_val_and_error(xlow,xhigh);
    std::cout << "lambda feeddown vals " << corr_and_err.first << " " << corr_and_err.second << std::endl;
    float new_val = h->GetBinContent(bin)*(1.-corr_and_err.first);
    float new_err = new_val * sqrt(pow(h->GetBinError(bin)/h->GetBinContent(bin),2.)+pow(corr_and_err.second/corr_and_err.first,2.));
    h->SetBinContent(bin,new_val);
    h->SetBinError(bin,new_err);
  }

};

#endif
