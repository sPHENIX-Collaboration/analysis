#ifndef SIDEBAND_FITTER_H
#define SIDEBAND_FITTER_H

#include <TDatabasePDG.h>

// bin integration methods from ROOT tutorial rf614_binned_fit_problems.C

// Force numeric integration and do this numeric integration with the
// RooBinIntegrator, which sums the function values at the bin centers.
void enableBinIntegrator(RooAbsReal &func, int numBins)
{
   RooNumIntConfig customConfig(*func.getIntegratorConfig());
   customConfig.method1D().setLabel("RooBinIntegrator");
   customConfig.getConfigSection("RooBinIntegrator").setRealValue("numBins", numBins);
   func.setIntegratorConfig(customConfig);
   func.forceNumInt(true);
}
 
// Reset the integrator config to disable the RooBinIntegrator.
void disableBinIntegrator(RooAbsReal &func)
{
   func.setIntegratorConfig();
   func.forceNumInt(false);
}

class SidebandFitter
{
  public:

  SidebandFitter(const std::string& nametag, const int pdgid, 
                 const std::pair<double,double>& left_sideband, const std::pair<double,double>& right_sideband, bool binned, int nmassbins);

  bool is_valid_fit(RooFitResult* result) const;
  void fit_orderN(RooAbsData* data, RooRealVar* mass, int n);
  void fit_all_orders_toN(RooAbsData* data, RooRealVar* mass, int n);

  RooWorkspace* get_best_fit() const;
  double get_best_nsignal() const;
  double get_best_nsignal_err() const;
  double get_best_nbkg() const;
  double get_best_nbkg_err() const;

  RooPlot* plot_best_fit(RooAbsData* data, RooRealVar* mass);

  double get_yield_syserr() const;

  void set_valid_binned_ranges(RooAbsData* data, RooRealVar* mass, double threshold);

  private:
  std::string m_nametag;
  int m_pdgid;
  double m_pdg_mass;
  std::pair<double,double> m_left_sideband;
  std::pair<double,double> m_right_sideband;
  bool m_binned;
  int m_nmassbins;

  // for yield extraction systematic uncertainty
  std::vector<double> m_valid_yields;

  RooWorkspace* best_fit_ws = nullptr;
  double best_AIC = DBL_MAX;
};

SidebandFitter::SidebandFitter(const std::string& nametag, const int pdgid, 
                               const std::pair<double,double>& left_sideband, const std::pair<double,double>& right_sideband, bool binned, int nmassbins=1)
: m_nametag(nametag), m_pdgid(pdgid), m_left_sideband(left_sideband), m_right_sideband(right_sideband), m_binned(binned), m_nmassbins(nmassbins)
{
  m_pdg_mass = TDatabasePDG::Instance()->GetParticle(pdgid)->Mass();
}

void SidebandFitter::set_valid_binned_ranges(RooAbsData* data, RooRealVar* mass, double threshold)
{
  TH1* h = data->createHistogram("h_temp",*mass);
  int n_bins_rightsideband = h->FindBin(m_right_sideband.second) - h->FindBin(m_right_sideband.first);
  std::cout << "n_bins_rightsideband " << n_bins_rightsideband << std::endl;
  double avg_background = h->Integral(h->FindBin(m_right_sideband.first),h->FindBin(m_right_sideband.second))/n_bins_rightsideband;
  std::cout << "avg_background " << avg_background << std::endl;
  int last_left_sideband_bin = h->FindBin(m_left_sideband.second);

  int last_bin_below_threshold = 1;
  for(int i=1;i<=last_left_sideband_bin;i++)
  {
    std::cout << "bin content " << h->GetBinContent(i) << std::endl;
    if(h->GetBinContent(i)<threshold*avg_background)
    {
      last_bin_below_threshold = i;
    }
  }
  std::cout << "last bin below threshold: " << last_bin_below_threshold << std::endl;
  double left_sideband_newmin = h->GetBinCenter(last_bin_below_threshold);
  if(m_left_sideband.second < left_sideband_newmin) m_left_sideband.second = left_sideband_newmin;
  mass->setRange("left_sideband",left_sideband_newmin,m_left_sideband.second);
  mass->setRange("full_range",left_sideband_newmin,m_right_sideband.second);
}

void SidebandFitter::fit_orderN(RooAbsData* data, RooRealVar* mass, int n)
{
  std::vector<RooRealVar> signal_parameters;
  std::vector<RooRealVar> sideband_parameters;
  std::vector<RooRealVar> background_parameters;

  mass->setRange("signal",m_left_sideband.second,m_right_sideband.first);
  if(!m_binned)
  {
    mass->setRange("left_sideband",m_left_sideband.first,m_left_sideband.second);
  }
  else
  {
    set_valid_binned_ranges(data,mass,0.02);
  }
  mass->setRange("right_sideband",m_right_sideband.first,m_right_sideband.second);

  std::string suffix = "_order"+std::to_string(n)+"_"+m_nametag;

  RooRealVar n_signal("n_signal","nSignal",0.9*data->sumEntries(),0.,data->sumEntries());
  RooRealVar n_bkg("n_bkg","nBackground",0.1*data->sumEntries(),0.,data->sumEntries());

  for(int order=0; order<n; order++)
  {
    std::string sideband_name = "q"+std::to_string(order+1);
    sideband_parameters.emplace_back(sideband_name.c_str(),sideband_name.c_str(),0.,-10.,10.);
    //background_parameters.emplace_back(bkg_name.c_str(),bkg_name.c_str(),0.,-10.,10.);
  }

  std::string sideband_name = "sideband_order"+std::to_string(n)+"_"+m_nametag;
  RooChebychev sideband_pdf(sideband_name.c_str(),"sideband fit",*mass,RooArgList(sideband_parameters.begin(),sideband_parameters.end()));

  //enableBinIntegrator(sideband_pdf,m_nmassbins);

  if(m_binned)
  {
    sideband_pdf.fitTo(*data,RooFit::Range("left_sideband,right_sideband"),RooFit::Minimizer("Minuit2","minimize"),RooFit::Offset("bin"),RooFit::Strategy(2),RooFit::PrintLevel(0));
  }
  else
  {
    sideband_pdf.fitTo(*data,RooFit::Range("left_sideband,right_sideband"),RooFit::Minimizer("Minuit2","minimize"),RooFit::Offset(true),RooFit::Strategy(2),RooFit::PrintLevel(0));
  }

  double signal_window_width = m_right_sideband.first-m_left_sideband.second;
/*
  RooRealVar mean("mean","mean",m_pdg_mass,(m_pdg_mass-0.5*signal_window_width),(m_pdg_mass+0.5*signal_window_width));
  RooRealVar width1("width1","width1",0.2*signal_window_width,0.01*signal_window_width,1.*signal_window_width);
  RooRealVar width2("width2","width2",0.2*signal_window_width,0.01*signal_window_width,1.*signal_window_width);

  RooGaussian gaus1("gaus1","gaus1",*mass,mean,width1);
  RooGaussian gaus2("gaus2","gaus2",*mass,mean,width2);

  RooRealVar gaus2_frac("gaus2_frac","gaus2_frac",0.01,0.,1.);

  RooAddPdf signal_pdf("signal","signal",RooArgList(gaus1,gaus2),RooArgList(gaus2_frac));
*/

  RooRealVar mean("mean","mean",m_pdg_mass,(m_pdg_mass-0.5*signal_window_width),(m_pdg_mass+0.5*signal_window_width));
  RooRealVar width("width","width",0.5*signal_window_width,0.01*signal_window_width,2.*signal_window_width);
  RooRealVar alpha_L("alpha_L","alpha_L",2.,1.,10.);
  RooRealVar alpha_R("alpha_R","alpha_R",2.,1.,10.);
  RooRealVar n_L("n_L","n_L",2.,1.,100.);
  RooRealVar n_R("n_R","n_R",2.,1.,100.);
  //alpha_L.setConstant();
  //alpha_R.setConstant();

  RooCrystalBall symm_signal_pdf("signal","signal",*mass,mean,width,alpha_L,n_L,true);

  for(int i=0; i<sideband_parameters.size(); i++)
  {
    std::string bkg_name = "k"+std::to_string(i+1);
    double val = sideband_parameters[i].getVal();
    background_parameters.emplace_back(bkg_name.c_str(),bkg_name.c_str(),val,-10.,10.);
    //background_parameters[i].setVal(sideband_parameters[i].getVal());
    //background_parameters[i].setError(sideband_parameters[i].getError());
    background_parameters[i].setConstant();
  }

  RooChebychev background_pdf("background","background",*mass,RooArgList(background_parameters.begin(),background_parameters.end()));

  RooAddPdf symmetric_fit_pdf("symm_fit","symmetric fit",RooArgList(symm_signal_pdf,background_pdf),RooArgList(n_signal,n_bkg));

  //enableBinIntegrator(symmetric_fit_pdf,m_nmassbins);

  if(m_binned)
  {
    symmetric_fit_pdf.fitTo(*data,RooFit::Range("full_range"),RooFit::Minimizer("Minuit2","minimize"),RooFit::Strategy(2),RooFit::Offset("bin"),RooFit::PrintLevel(0));
  }
  else
  {
    symmetric_fit_pdf.fitTo(*data,RooFit::Minimizer("Minuit2","minimize"),RooFit::Strategy(2),RooFit::Offset(true),RooFit::PrintLevel(0));
  }

  mean.setConstant();
  width.setConstant();
  alpha_R.setVal(alpha_L.getVal());
  n_R.setVal(n_L.getVal());
  //alpha_L.setConstant();
  //alpha_R.setConstant();

  //for(int i=0;i<sideband_parameters.size();i++)
  //{
  //  background_parameters[i].setConstant();
 // }

  RooCrystalBall asymm_signal_pdf("signal","signal",*mass,mean,width,alpha_L,n_L,alpha_R,n_R);

  RooAddPdf full_fit_pdf("full_fit","full fit",RooArgList(asymm_signal_pdf,background_pdf),RooArgList(n_signal,n_bkg));

  //enableBinIntegrator(full_fit_pdf,m_nmassbins);

  RooFitResult* full_result;
  if(m_binned)
  {
    full_result = full_fit_pdf.fitTo(*data,RooFit::Range("full_range"),RooFit::Minimizer("Minuit2","minimize"),RooFit::Strategy(2),RooFit::Offset("bin"),RooFit::Save(true),RooFit::PrintLevel(0));
  }
  else
  {
    full_result = full_fit_pdf.fitTo(*data,RooFit::Minimizer("Minuit2","minimize"),RooFit::Strategy(2),RooFit::Offset(true),RooFit::Save(true),RooFit::PrintLevel(0));
  }

  if(is_valid_fit(full_result))
  {
    double NLL = full_result->minNll();
    std::cout << "minNll = " << NLL << std::endl;

    // Akaike information criterion (AIC) counters overfitting:
    // AIC = 2NDF - 2NLL
    // NDF = {background parameters} + (mean + width + alpha_L + n_L + alpha_R + n_R) + (nsignal + nbackground)
    int ndf = background_parameters.size() + 8;
    double AIC = 2.*ndf + 2*NLL;

    std::cout << "NDF = " << ndf << ", AIC = " << AIC << std::endl;
    std::cout << "best AIC = " << best_AIC << std::endl;

    m_valid_yields.push_back(n_signal.getVal());

    if(AIC < best_AIC)
    {
      if(best_fit_ws) delete best_fit_ws;

      RooWorkspace* ws = new RooWorkspace(("w"+suffix).c_str(),"workspace");
      ws->import(full_fit_pdf);
      ws->import(*mass->Clone());

      best_fit_ws = ws;
      best_AIC = AIC;
    }
  }
  else
  {
    std::cout << "fit for order-" << n << " background failed" << std::endl;
  }
}

bool SidebandFitter::is_valid_fit(RooFitResult* result) const
{
  int status = result->status();
  int cov_status = result->covQual();
  std::cout << "status " << status << " covQual " << cov_status << std::endl;
  return (status==0 || status==1 || status==5) && (cov_status>=0);
}

void SidebandFitter::fit_all_orders_toN(RooAbsData* data, RooRealVar* mass, int n)
{
  for(int order=1; order<=n; order++)
  {
    fit_orderN(data,mass,order);
  }
}

RooWorkspace* SidebandFitter::get_best_fit() const
{
  return best_fit_ws;
}

double SidebandFitter::get_best_nsignal() const
{
  RooRealVar* nsignal = best_fit_ws->var("n_signal");
  return nsignal->getVal();
}

double SidebandFitter::get_best_nsignal_err() const
{
  RooRealVar* nsignal = best_fit_ws->var("n_signal");
  return nsignal->getError();
}

double SidebandFitter::get_best_nbkg() const
{
  RooRealVar* nbkg = best_fit_ws->var("n_bkg");
  return nbkg->getVal();
}

double SidebandFitter::get_best_nbkg_err() const
{
  RooRealVar* nbkg = best_fit_ws->var("n_bkg");
  return nbkg->getError();
}

RooPlot* SidebandFitter::plot_best_fit(RooAbsData* data, RooRealVar* mass)
{
  RooAbsPdf* full_fit = best_fit_ws->pdf("full_fit");

  RooPlot* plot;
  if(m_binned)
  {
    plot = mass->frame(RooFit::Range("full_range"));
  }
  else
  {
    plot = mass->frame();
  }
  data->plotOn(plot);
  if(m_binned)
  {
    full_fit->plotOn(plot,RooFit::Range("full_range"),RooFit::NormRange("full_range"),RooFit::Components("background"),RooFit::DrawOption("FL"),RooFit::LineStyle(kDashed),RooFit::FillColor(kGray),RooFit::MoveToBack());
    full_fit->plotOn(plot,RooFit::Range("full_range"),RooFit::NormRange("full_range"),RooFit::DrawOption("FL"),RooFit::FillColor(kAzure+1),RooFit::MoveToBack());
  }
  else
  {
    full_fit->plotOn(plot,RooFit::Components("background"),RooFit::DrawOption("FL"),RooFit::LineStyle(kDashed),RooFit::FillColor(kGray),RooFit::MoveToBack());
    full_fit->plotOn(plot,RooFit::DrawOption("FL"),RooFit::FillColor(kAzure+1),RooFit::MoveToBack());
  }

  return plot;
}

double SidebandFitter::get_yield_syserr() const
{
  //auto minmax_yield = std::minmax_element(m_valid_yields.begin(),m_valid_yields.end());
  //return (*minmax_yield.second-*minmax_yield.first)/(sqrt(12.)*get_best_nsignal());

  const double best_fit_yield = get_best_nsignal();

  std::vector<double> relative_diffs;
  for(double yield : m_valid_yields)
  {
    relative_diffs.push_back(fabs(best_fit_yield - yield)/best_fit_yield);
  }
  std::sort(relative_diffs.begin(),relative_diffs.end());

  int median_i = relative_diffs.size() / 2;
  if(relative_diffs.size() % 2 == 0)
  {
    return (relative_diffs[median_i-1] + relative_diffs[median_i])/2.;
  }
  else
  {
    return relative_diffs[median_i];
  }

}

#endif
