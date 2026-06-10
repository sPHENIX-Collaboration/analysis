#ifndef RESONANCE_RATIO_H
#define RESONANCE_RATIO_H

#include "TFile.h"
#include "TH1F.h"

#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooPlot.h"

#include <phool/PHRandomSeed.h>
#include <gsl/gsl_rng.h>

#include "../util/binning.h"
#include "../util/DifferentialContainer.h"
#include "../corrections/CorrectionHistogram1D.h"
#include "ParticleModel.h"

class ResonanceRatio
{
  public:
  ResonanceRatio(ParticleModel& numerator_model, ParticleModel& denominator_model,
                 TFile* outf, std::string rationame, std::string ratiotitle, float scalefactor, bool blind,
                 std::vector<HistogramInfo> variables, std::vector<std::vector<std::shared_ptr<CorrectionHistogram1D>>> corrections)
  : _numerator_model(numerator_model), _denominator_model(denominator_model),
    outfile(outf), _rationame(rationame), _ratiotitle(ratiotitle), _scalefactor(scalefactor), _blind(blind),
    _variables(variables), _corrections(corrections)
  {
    setup_yield_histograms();
  }

  // main analysis functions
  void calculate_ratios_unbinned(RooAbsData* numerator_data, RooAbsData* denominator_data);
  void calculate_ratios_binned(TH1F* integrated_numerator_data, std::vector<DifferentialContainer>& diff_numerator_data,
                               TH1F* integrated_denominator_data, std::vector<DifferentialContainer>& diff_denominator_data);

  // yield histograms
  TH1F* numerator_integrated_yield;
  TH1F* denominator_integrated_yield;
  std::vector<TH1F*> numerator_diff_yields;
  std::vector<TH1F*> denominator_diff_yields;

  TH1F* numerator_integrated_fit_syserr;
  TH1F* denominator_integrated_fit_syserr;
  std::vector<TH1F*> numerator_fit_syserr;
  std::vector<TH1F*> denominator_fit_syserr;
  std::vector<TH1F*> ratio_fit_syserr;

  // ratio histograms
  std::vector<TH1F*> integrated_ratio_w_corrections;
  std::vector<std::vector<TH1F*>> diff_ratios_w_corrections;
  // output file
  TFile* outfile;

  protected:
  // workflow functions
  void setup_yield_histograms();
  void get_yield(TH1F* h_yield, int i, RooAbsData* ds, ParticleModel model);
  void get_yield_constfit(TH1F* h_yield, TH1F* h_err, int i, RooAbsData* ds, ParticleModel model);
  void get_diff_yield_unbinned(TH1F* h_yield, TH1F* h_err, HistogramInfo& hinfo, RooAbsData* data, ParticleModel& model);
  void get_diff_yield_binned(TH1F* h_yield, HistogramInfo& hinfo, DifferentialContainer& data, ParticleModel& model);
  std::string get_corrected_title(std::string current_title, std::string correction_title);
  void calculate_ratios();
  void save_results();

  // fit models
  ParticleModel _numerator_model;
  ParticleModel _denominator_model;
  // differential variables and corresponding slates of corrections to apply
  std::vector<HistogramInfo> _variables;
  std::vector<std::vector<std::shared_ptr<CorrectionHistogram1D>>> _corrections;
  // naming, scale factor, and blind settings
  std::string _rationame;
  std::string _ratiotitle;
  float _scalefactor;
  bool _blind;
};

void ResonanceRatio::setup_yield_histograms()
{
  HistogramInfo numerator_massbins = BinInfo::mass_bins.at(_numerator_model.name);
  HistogramInfo denominator_massbins = BinInfo::mass_bins.at(_denominator_model.name);

  for(HistogramInfo& hinfo : _variables)
  {
    numerator_diff_yields.push_back(makeHistogram(_numerator_model.name+"_yield",_numerator_model.name+" yield",hinfo));
    denominator_diff_yields.push_back(makeHistogram(_denominator_model.name+"_yield",_denominator_model.name+" yield",hinfo));
    numerator_fit_syserr.push_back(makeHistogram(_numerator_model.name+"_fit_syserr",_numerator_model.name+" systematic error from mass fit",hinfo));
    denominator_fit_syserr.push_back(makeHistogram(_denominator_model.name+"_fit_syserr",_denominator_model.name+" systematic error from mass fit",hinfo));
    ratio_fit_syserr.push_back(makeHistogram("ratio_fit_syserr","systematic error from mass fit procedure",hinfo));
  }

  numerator_integrated_yield = new TH1F(("all_"+_numerator_model.name+"_yield").c_str(),("All "+_numerator_model.name+" yield").c_str(),1,0.,1.);
  denominator_integrated_yield = new TH1F(("all_"+_denominator_model.name+"_yield").c_str(),("All "+_denominator_model.name+" yield").c_str(),1,0.,1.);

  numerator_integrated_fit_syserr = new TH1F(("all_"+_numerator_model.name+"_fit_syserr").c_str(),("All "+_numerator_model.name+" fit systematic error").c_str(),1,0.,1.);
  denominator_integrated_fit_syserr = new TH1F(("all_"+_denominator_model.name+"_fit_syserr").c_str(),("All "+_denominator_model.name+" fit systematic error").c_str(),1,0.,1.);

  numerator_integrated_yield->SetTitle((numerator_massbins.title+";"+numerator_massbins.axis_label+";Candidates").c_str());
  denominator_integrated_yield->SetTitle((denominator_massbins.title+";"+denominator_massbins.axis_label+";Candidates").c_str());
}

void ResonanceRatio::get_yield_constfit(TH1F* h_yield, TH1F* h_err, int i, RooAbsData* ds, ParticleModel model)
{
  TH1* ds_h = ds->createHistogram((std::string(h_yield->GetName())+"_"+std::to_string(i)+"_h").c_str(),*model.mass);
  ds_h->Write();

  RooRealVar const_leftsideband("const_leftsideband","left sideband level",ds->sumEntries(),0.,1e12);
  RooRealVar const_rightsideband("const_rightsideband","right sideband level",ds->sumEntries(),0.,1e12);

  model.mass->setRange("full",model.mass->getMin(),model.mass->getMax());
  model.mass->setRange("left_sideband",model.left_sideband.first,model.left_sideband.second);
  model.mass->setRange("right_sideband",model.right_sideband.first,model.right_sideband.second);

  RooConstVar dummy_parameter("dummy_par","dummy parameter",1.);

  RooPolynomial uniform_left("uniform_left","uniform left",*(model.mass),RooArgList(dummy_parameter));
  RooPolynomial uniform_right("uniform_right","uniform right",*(model.mass),RooArgList(dummy_parameter));

  RooExtendPdf leftside_model("leftside_model","leftside model",uniform_left,const_leftsideband,"left_sideband");
  RooExtendPdf rightside_model("rightside_model","rightside model",uniform_right,const_rightsideband,"right_sideband");

  leftside_model.fitTo(*ds,RooFit::Range("left_sideband"));
  rightside_model.fitTo(*ds,RooFit::Range("right_sideband"));

  double signal_region_width = model.right_sideband.first - model.left_sideband.second;
  double left_sideband_width = model.left_sideband.second-model.left_sideband.first;
  double right_sideband_width = model.right_sideband.second-model.right_sideband.first;

  double yield = ds->sumEntries() - (const_leftsideband.getVal() + const_rightsideband.getVal())/2.;
  double yield_err = sqrt(ds->sumEntriesW2() + (pow(const_leftsideband.getError(),2.) + pow(const_rightsideband.getError(),2.))/2.);

  std::cout << "total integral: " << ds->sumEntries() << std::endl;

  std::cout << "from fit:" << std::endl;
  std::cout << "left sideband: " << const_leftsideband.getVal() << " +- " << const_leftsideband.getError() << std::endl;
  //std::cout << "bkg yield from left sideband: " << const_leftsideband.getVal()*mass_window_width << " +- " << const_leftsideband.getError()*mass_window_width << std::endl;
  std::cout << "right sideband: " << const_rightsideband.getVal() << " +- " << const_rightsideband.getError() << std::endl;
  //std::cout << "bkg yield from right sideband: " << const_rightsideband.getVal()*mass_window_width << " +- " << const_rightsideband.getError()*mass_window_width << std::endl;
  std::cout << "signal yield: " << yield << " +- " << yield_err << std::endl;

  std::string signalregion_cut = model.name + "_mass > " + std::to_string(model.left_sideband.second) + " && " + model.name + "_mass < " + std::to_string(model.right_sideband.first);
  std::string leftsideband_cut = model.name + "_mass > " + std::to_string(model.left_sideband.first) + " && " + model.name + "_mass < " + std::to_string(model.left_sideband.second);
  std::string rightsideband_cut = model.name + "_mass > " + std::to_string(model.right_sideband.first) + " && " + model.name + "_mass < " + std::to_string(model.right_sideband.second);

  double total_signalregion_integral = ds->sumEntries(signalregion_cut.c_str());
  double leftsideband_const_fromintegral = ds->sumEntries(leftsideband_cut.c_str());
  double rightsideband_const_fromintegral = ds->sumEntries(rightsideband_cut.c_str());
  double yield_fromintegral = total_signalregion_integral - (leftsideband_const_fromintegral/left_sideband_width + rightsideband_const_fromintegral/right_sideband_width)/2.*signal_region_width;
  double yield_fromintegral_err = sqrt(total_signalregion_integral + (leftsideband_const_fromintegral/pow(left_sideband_width,2.) + rightsideband_const_fromintegral/pow(right_sideband_width,2.))/4.*pow(signal_region_width,2.));

  std::cout << "from integral:" << std::endl;
  std::cout << "signal region integral: " << total_signalregion_integral << std::endl;
  std::cout << "left sideband: " << leftsideband_const_fromintegral*signal_region_width/left_sideband_width << " +- " << sqrt(leftsideband_const_fromintegral)*signal_region_width/left_sideband_width << std::endl;
  std::cout << "right sideband: " << rightsideband_const_fromintegral*signal_region_width/right_sideband_width << " +- " << sqrt(rightsideband_const_fromintegral)*signal_region_width/right_sideband_width << std::endl;
  std::cout << "signal yield: " << yield_fromintegral << " +- " << yield_fromintegral_err << std::endl;

  double syserr = fabs(leftsideband_const_fromintegral/left_sideband_width-rightsideband_const_fromintegral/right_sideband_width)/2.*signal_region_width;

  std::cout << "sys_err from fit: " << syserr << std::endl;

  h_yield->SetBinContent(i,yield_fromintegral);
  h_yield->SetBinError(i,yield_fromintegral_err);

  h_err->SetBinContent(i,syserr/yield_fromintegral);

  std::string name;
  if(i>=0) name = std::string(h_yield->GetName())+"_"+std::to_string(i);
  else name = std::string(h_yield->GetName())+"_";
  std::string title;
  if(i>=0) title = std::string(h_yield->GetTitle())+" bin "+std::to_string(i);
  else title = std::string(h_yield->GetTitle());

  RooPlot* plot = model.mass->frame(RooFit::Title(title.c_str()));
  plot->SetName(name.c_str());
  ds->plotOn(plot);
  leftside_model.plotOn(plot,RooFit::DrawOption("L"),RooFit::Range("left_sideband"));
  rightside_model.plotOn(plot,RooFit::DrawOption("L"),RooFit::Range("right_sideband"));
  plot->Write();
}

void ResonanceRatio::get_yield(TH1F* h_yield, int i, RooAbsData* ds, ParticleModel model)
{
  model.generate_fitFunction();
  model.n_signal->setVal(ds->sumEntries());
  model.n_background->setVal(.01*model.n_signal->getVal());
  model.fitTo(*ds);

  double nsignal = model.n_signal->getVal();
  double nsignal_err = model.n_signal->getError();

  double nbkg = model.n_background->getVal();
  double nbkg_err = model.n_background->getError();

  double yield = ds->sumEntries() - nbkg;
  double yield_err = nbkg_err;

  std::cout << "nsignal val " << nsignal << std::endl;
  std::cout << "nsignal err " << nsignal_err << std::endl;
  std::cout << "nbkg " << nbkg << std::endl;
  std::cout << "nbkg err " << nbkg_err << std::endl;
  std::cout << "sum entries " << ds->sumEntries() << std::endl;
  std::cout << "yield " << yield << std::endl;

  h_yield->SetBinContent(i,yield);
  h_yield->SetBinError(i,yield_err);

  std::string name;
  if(i>=0) name = std::string(h_yield->GetName())+"_"+std::to_string(i);
  else name = std::string(h_yield->GetName())+"_";
  std::string title;
  if(i>=0) title = std::string(h_yield->GetTitle())+" bin "+std::to_string(i);
  else title = std::string(h_yield->GetTitle());

  RooPlot* plot = model.mass->frame(RooFit::Title(title.c_str()));
  plot->SetName(name.c_str());
  ds->plotOn(plot);
  model.fit_function->plotOn(plot,RooFit::Components(model.background_function->GetName()),RooFit::DrawOption("FL"),RooFit::LineStyle(kDashed),RooFit::FillColor(kGray),RooFit::MoveToBack());
  model.fit_function->plotOn(plot,RooFit::DrawOption("FL"),RooFit::FillColor(kAzure+1),RooFit::MoveToBack());
  plot->Write();
}

void ResonanceRatio::get_diff_yield_unbinned(TH1F* h_yield, TH1F* h_err, HistogramInfo& hinfo, RooAbsData* data, ParticleModel& model)
{
  //if(hinfo.name == "pT" && model.name == "Lambda0") model.use_threshold = true;
  //else model.use_threshold = false;
  
  model.use_threshold = false;

  for(int i=1; i<=h_yield->GetNbinsX(); i++)
  {
    std::cout << "bin " << i << " of " << h_yield->GetNbinsX() << std::endl;

    std::string selection = hinfo.get_bin_selection(std::string(data->GetName())+"_"+hinfo.name,i);
    std::cout << "selection: " << selection << std::endl;

    RooDataSet* ds_selected = (RooDataSet*)data->reduce({*(model.mass)},selection.c_str());

    get_yield_constfit(h_yield,h_err,i,ds_selected,model);
  }
}

void ResonanceRatio::get_diff_yield_binned(TH1F* h_yield, HistogramInfo& hinfo, DifferentialContainer& data, ParticleModel& model)
{
  for(int i=1; i<=h_yield->GetNbinsX(); i++)
  {
    std::cout << "bin " << i << " of " << h_yield->GetNbinsX() << std::endl;
    RooDataHist dh("binned_massfit","binned_massfit",*(model.mass),RooFit::Import(*(data.hists[i])));
    get_yield(h_yield,i,&dh,model);
  }
}

std::string ResonanceRatio::get_corrected_title(std::string current_title,std::string correction_title)
{
  std::string new_title;
  int semicolon_pos = current_title.find(";");
  if(semicolon_pos<current_title.length())
  {
    std::string main_title = current_title.substr(0,current_title.find(";"));
    std::string axis_labels = current_title.substr(current_title.find(";"),current_title.length());
    std::string new_main_title = main_title + ", " + correction_title + " corrected";
    new_title = new_main_title + axis_labels;
  }
  else
  {
    new_title = current_title + ", " + correction_title + " corrected";
  }
  return new_title;
}

void ResonanceRatio::calculate_ratios()
{
  // calculate integrated ratio and apply corrections

  TH1F* integrated_ratio = new TH1F("integrated_ratio",("Integrated "+_rationame).c_str(),1,0.,1.);
  float integrated_ratio_val = numerator_integrated_yield->GetBinContent(1)/denominator_integrated_yield->GetBinContent(1)*_scalefactor;
  float integrated_ratio_err = integrated_ratio_val * sqrt(pow(numerator_integrated_yield->GetBinError(1)/numerator_integrated_yield->GetBinContent(1),2.)+
                                                           pow(denominator_integrated_yield->GetBinError(1)/denominator_integrated_yield->GetBinContent(1),2.));
  integrated_ratio->SetBinContent(1,integrated_ratio_val);
  integrated_ratio->SetBinError(1,integrated_ratio_err);

  integrated_ratio_w_corrections.push_back(integrated_ratio);

  for(std::shared_ptr<CorrectionHistogram1D> correction : _corrections.at(0)) // retrieve one set of corrections to integrate over
  {
    TH1F* current_h = integrated_ratio_w_corrections.back();
    std::string name = std::string(current_h->GetName()) + "_" + correction->name + "corrected";

    std::string current_title = std::string(current_h->GetTitle());
    std::string new_title = get_corrected_title(current_title,correction->title);

    TH1F* new_h = (TH1F*)current_h->Clone(name.c_str());
    new_h->SetTitle(new_title.c_str());

    correction->apply_correction(correction->get_xmin(),correction->get_xmax(),new_h,1);

    integrated_ratio_w_corrections.push_back(new_h); 
  }

  // calculate differential ratios

  for(int ivar=0; ivar<_variables.size(); ivar++)
  {
    TH1F* diffratio = makeHistogram(_rationame,_ratiotitle,_variables[ivar]);
    for(int ibin=1; ibin<=diffratio->GetNbinsX(); ibin++)
    {
      float ratio_val = numerator_diff_yields[ivar]->GetBinContent(ibin) * _scalefactor / denominator_diff_yields[ivar]->GetBinContent(ibin);
      float ratio_err = ratio_val * sqrt(pow(numerator_diff_yields[ivar]->GetBinError(ibin)/numerator_diff_yields[ivar]->GetBinContent(ibin),2.)+pow(denominator_diff_yields[ivar]->GetBinError(ibin)/denominator_diff_yields[ivar]->GetBinContent(ibin),2.));
      diffratio->SetBinContent(ibin,ratio_val);
      diffratio->SetBinError(ibin,ratio_err);
    }
    diff_ratios_w_corrections.push_back({diffratio});
  }

  // apply corrections to differential ratios

  for(int ivar=0; ivar<_variables.size(); ivar++)
  {
    for(int icorr=0; icorr<_corrections[ivar].size(); icorr++)
    {
      TH1F* current_h = diff_ratios_w_corrections[ivar][icorr];
      std::shared_ptr<CorrectionHistogram1D> correction = _corrections[ivar][icorr];
      std::string name = std::string(current_h->GetName()) + "_" + correction->name + "corrected";

      // transforming title is a bit complicated due to axis labels being in the way
      std::string current_title = std::string(current_h->GetTitle());
      std::string new_title = get_corrected_title(current_title,correction->title);

      TH1F* new_h = (TH1F*)current_h->Clone(name.c_str());
      new_h->SetTitle(new_title.c_str());

      for(int ibin=1; ibin<=new_h->GetNbinsX(); ibin++)
      {
        float xlow = new_h->GetBinLowEdge(ibin);
        float xhigh = xlow + new_h->GetBinWidth(ibin);
        std::cout << "ivar " << ivar << " icorr " << icorr << " ibin " << ibin << std::endl;
        std::cout << "before correction: " << new_h->GetBinContent(ibin) << " +- " << new_h->GetBinError(ibin) << std::endl;
        correction->apply_correction(xlow,xhigh,new_h,ibin);
        std::cout << "after " << correction->title << " correction: " << new_h->GetBinContent(ibin) << " +- " << new_h->GetBinError(ibin) << std::endl;
      }
      diff_ratios_w_corrections[ivar].push_back(new_h);
    }
  }

  // calculate systematic error on ratio from mass peak fits

  for(int i=0;i<ratio_fit_syserr.size();i++)
  {
    for(int bin=1;bin<=ratio_fit_syserr[i]->GetNbinsX();bin++)
    {
      double numerator_relative_err = numerator_fit_syserr[i]->GetBinContent(bin);
      double denominator_relative_err = denominator_fit_syserr[i]->GetBinContent(bin);
      ratio_fit_syserr[i]->SetBinContent(bin,_scalefactor * sqrt(pow(numerator_relative_err,2.)+pow(denominator_relative_err,2.)));
    }
  }

  // apply blinding

  if (_blind)
  {
    const uint seed = PHRandomSeed();
    std::unique_ptr<gsl_rng> m_rng;
    m_rng.reset(gsl_rng_alloc(gsl_rng_mt19937));
    gsl_rng_set(m_rng.get(), seed);
    float blind_par = 9.9*gsl_rng_uniform_pos(m_rng.get())+0.1; //Take a value anywhere between 0.1 and 10.

    for(TH1F* h : integrated_ratio_w_corrections)
    {
      float content = h->GetBinContent(1);
      h->SetBinContent(1,content * blind_par);
      h->SetBinError(1,h->GetBinError(1) * blind_par);
    }

    for(std::vector<TH1F*>& v_corr : diff_ratios_w_corrections)
    {
      for(TH1F* h : v_corr)
      {
        for(int ibin=0; ibin<=h->GetNbinsX(); ibin++)
        {
          float content = h->GetBinContent(ibin);
          h->SetBinContent(ibin,content * blind_par);
          h->SetBinError(ibin,h->GetBinError(ibin) * blind_par);
        }
      }
    }
  }
}

void ResonanceRatio::save_results()
{
  // Save everything to file

  outfile->cd();

  for(TH1F* h : numerator_diff_yields)
  {
    //h->Scale(1.,"width");
    h->Write();
  }

  for(TH1F* h : denominator_diff_yields)
  {
    //h->Scale(1.,"width");
    h->Write();
  }

  for(TH1F* h : numerator_fit_syserr)
  {
    h->Write();
  }

  for(TH1F* h : denominator_fit_syserr)
  {
    h->Write();
  }

  for(TH1F* h : ratio_fit_syserr)
  {
    h->Write();
  }

  for(TH1F* h : integrated_ratio_w_corrections)
  {
    h->Write();
  }

  for(std::vector<TH1F*>& v_corr : diff_ratios_w_corrections)
  {
    for(TH1F* h : v_corr)
    {
      h->Write();
    }
  }
}

void ResonanceRatio::calculate_ratios_unbinned(RooAbsData* numerator_data, RooAbsData* denominator_data)
{
  get_yield_constfit(numerator_integrated_yield,numerator_integrated_fit_syserr,-1,numerator_data,_numerator_model);
  get_yield_constfit(denominator_integrated_yield,numerator_integrated_fit_syserr,-1,denominator_data,_denominator_model);

  // extract differential yields

  for(size_t i=0; i<_variables.size(); i++)
  {
    std::cout << "======= Differential " << numerator_diff_yields[i]->GetName() << " =======" << std::endl;
    get_diff_yield_unbinned(numerator_diff_yields[i],numerator_fit_syserr[i],_variables[i],numerator_data,_numerator_model);
    std::cout << "======= Differential " << denominator_diff_yields[i]->GetName() << " =======" << std::endl;
    get_diff_yield_unbinned(denominator_diff_yields[i],denominator_fit_syserr[i],_variables[i],denominator_data,_denominator_model);
  }

  calculate_ratios();

  save_results();
}

void ResonanceRatio::calculate_ratios_binned(TH1F* integrated_numerator_data, std::vector<DifferentialContainer>& diff_numerator_data, 
                                             TH1F* integrated_denominator_data, std::vector<DifferentialContainer>& diff_denominator_data)
{
  RooDataHist integrated_numerator_dh("integrated_numerator_dh","integrated_numerator_dh",RooArgList(*(_numerator_model.mass)),RooFit::Import(*integrated_numerator_data));
  RooDataHist integrated_denominator_dh("integrated_denominator_dh","integrated_denominator_dh",RooArgList(*(_denominator_model.mass)),RooFit::Import(*integrated_denominator_data));

  get_yield(numerator_integrated_yield,-1,&integrated_numerator_dh,_numerator_model);
  get_yield(denominator_integrated_yield,-1,&integrated_denominator_dh,_denominator_model);

  for(int i=0; i<_variables.size(); i++)
  {
    get_diff_yield_binned(numerator_diff_yields[i],_variables[i],diff_numerator_data[i],_numerator_model);
    get_diff_yield_binned(denominator_diff_yields[i],_variables[i],diff_denominator_data[i],_denominator_model);
  }
}

#endif
