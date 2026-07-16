#ifndef CORRECTIONHISTOGRAM1D_H
#define CORRECTIONHISTOGRAM1D_H

#include <TH1.h>

#include <string>
#include <iostream>

struct CorrectionHistogram1D
{
  TH1* h_corr;
  std::string name;
  std::string title;

  CorrectionHistogram1D() = default;

  CorrectionHistogram1D(const CorrectionHistogram1D& ch)
  {
    h_corr = ch.h_corr;
    name = ch.name;
    title = ch.title;
  }

  double get_xmin()
  {
    return h_corr->GetBinLowEdge(1);
  }

  double get_xmax()
  {
    return h_corr->GetBinLowEdge(h_corr->GetNbinsX())+h_corr->GetBinWidth(h_corr->GetNbinsX());
  }

  bool in_range(double x)
  {
    return (x>=get_xmin() && x<=get_xmax());
  }

  void check_range_warn(double x_low, double x_high)
  {
    if(!in_range(x_low) || !in_range(x_high))
    {
      std::cout << "WARNING: selection [" << x_low << ", " << x_high << "]"
        << " is out of range [" << get_xmin() << ", " 
        << get_xmax() << "]" << std::endl;
    }
  }

  // get correction value by integrating appropriately over range of bins
  std::pair<double,double> get_val_and_error(double x_low, double x_high)
  {
    check_range_warn(x_low,x_high);

    // add small buffer to avoid edge cases
    int bin_low = h_corr->FindBin(x_low+fabs(.0001*x_low));
    int bin_high = h_corr->FindBin(x_high-fabs(.0001*x_high));

    if(bin_low == bin_high)
    {
      return std::make_pair(h_corr->GetBinContent(bin_low),h_corr->GetBinError(bin_low));
    }
    else
    {
      double binwidth_low = h_corr->GetBinWidth(bin_low);
      double binwidth_high = h_corr->GetBinWidth(bin_high);

      double upperedge_low = h_corr->GetBinLowEdge(bin_low)+binwidth_low;
      double loweredge_high = h_corr->GetBinLowEdge(bin_high);

      double endbin_frac_low = (upperedge_low - x_low)/binwidth_low;
      double endbin_frac_high = (x_high - loweredge_high)/binwidth_high;

      // weighted average by bin error
      double weighted_sum = 0.;
      double sum_inv_w2 = 0.;
      
      // add ends separately
      weighted_sum += h_corr->GetBinContent(bin_low)*endbin_frac_low/pow(h_corr->GetBinError(bin_low),2.);
      sum_inv_w2 += endbin_frac_low/pow(h_corr->GetBinError(bin_low),2.);

      weighted_sum += h_corr->GetBinContent(bin_high)*endbin_frac_high/pow(h_corr->GetBinError(bin_high),2.);
      sum_inv_w2 += endbin_frac_high/pow(h_corr->GetBinError(bin_high),2.);

      // and everything else in between
      for(int ibin = bin_low+1; ibin<bin_high; ibin++)
      {
        weighted_sum += h_corr->GetBinContent(ibin)/pow(h_corr->GetBinError(ibin),2.);
        sum_inv_w2 += 1./pow(h_corr->GetBinError(ibin),2.);
      }

      return std::make_pair(weighted_sum/sum_inv_w2,1/sqrt(sum_inv_w2));
    }
  }

  virtual void apply_correction(float xlow, float xhigh, TH1F* h, int bin)
  {
    std::cout << "WARNING: you haven't replaced apply_correction with a correction-specific function yet!!" << std::endl;
  }

  virtual ~CorrectionHistogram1D() = default;
};

#endif
