#ifndef SYSTEMATIC_UNCERTAINTY_H
#define SYSTEMATIC_UNCERTAINTY_H

class SystematicUncertainty
{
  public:

  SystematicUncertainty(TMatrixDSym& cov_in, std::vector<double>& bins_in)
  {
    covariance = cov_in;
    bins = bins_in;
    hBins = std::make_unique<TH1D>("hBins","hBins",bins.size()-1,bins.data());
  }

  bool in_range(double x)
  {
    return (x>=bins[0] && x<=bins[bins.size()-1]);
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

  double uncorrelatedUncertaintyAt(double x)
  {
    if(!in_range(x))
    {
      std::cout << "WARNING: point " << x << " is out of range [" << bins[0] << ", " << bins[bins.size()-1] << "]" << std::endl;
      std::cout << "returning 0" << std::endl;
      return 0.;
    }
    
    int iBin = hBins->FindBin(x)-1;
    return sqrt(covariance(iBin,iBin));
  }

  double nextBinCovarianceAt(double x)
  {
    if(!in_range(x))
    {
      std::cout << "WARNING: point " << x << " is out of range [" << bins[0] << ", " << bins[bins.size()-1] << "]" << std::endl;
      std::cout << "returning 0" << std::endl;
      return 0.;
    }

    int iBin = hBins->FindBin(x)-1;
    if(iBin<bins.size()) return covariance(iBin,iBin+1);
    else return 0.;
  }

  double prevBinCovarianceAt(double x)
  {
    if(!in_range(x))
    {
      std::cout << "WARNING: point " << x << " is out of range [" << bins[0] << ", " << bins[bins.size()-1] << "]" << std::endl;
      std::cout << "returning 0" << std::endl;
      return 0.;
    }

    int iBin = hBins->FindBin(x)-1;
    if(iBin>0) return covariance(iBin,iBin-1);
    else return 0.;
  }

  // TODO: verify this is actually correct
  double fullyCorrelatedUncertainty()
  {
    double sum = 0.;

    for(int i=0;i<bins.size();i++)
    {
      for(int j=0;j<bins.size();j++)
      {
        if(i!=j) sum += covariance(i,j);
      }
    }

    return sum/(pow(bins.size(),2)-bins.size());
  }

  // TODO: implement
  double uncorrelatedUncertaintyInBin(double xlow, double xup)
  {
    std::cout << "WARNING: uncorrelatedUncertaintyInBin not yet implemented" << std::endl;
    return 0.;
  }

  // TODO: implement
  double twoBinCovariance(double xlow, double xup, double xlow2, double xup2)
  {
    std::cout << "WARNING: twoBinCovariance not yet implemented" << std::endl;
    return 0.;
  }

  protected:

  TMatrixDSym covariance;
  std::vector<double> bins;
  std::unique_ptr<TH1D> hBins; // dummy histogram for easy bin-finding

};

#endif
