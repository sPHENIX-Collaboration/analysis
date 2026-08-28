#ifndef PARTICLEMODEL_H
#define PARTICLEMODEL_H

#include <RooRealVar.h>
#include <RooAbsPdf.h>
#include <RooFitResult.h>

struct ParticleModel
{
  std::string name;
  int pdgid;
  std::vector<int> daughter_pdgids;

  bool use_threshold = false;

  std::shared_ptr<RooRealVar> mass;

  // Some RooFit functions prefer RooRealVars, some prefer RooArgLists
  // (they are not interconvertible, as RooArgLists object-slice RooRealVars into RooAbsArgs)
  // so we fill our parameter lists with a separate function

  std::shared_ptr<RooAbsPdf> signal_function;
  std::vector<RooRealVar> signal_parameters;
  RooArgList signal_parameter_arglist;

  std::shared_ptr<RooAbsPdf> background_function;
  std::vector<RooRealVar> background_parameters;
  RooArgList background_parameter_arglist;

  void add_signal_parameter(const std::string& name, const std::string& title, float init_val, float min_val, float max_val)
  {
    signal_parameters.emplace_back(name.c_str(),title.c_str(),init_val,min_val,max_val);
    signal_parameter_arglist.add(signal_parameters.back());
  }
  void add_background_parameter(const std::string& name, const std::string& title, float init_val, float min_val, float max_val)
  {
    background_parameters.emplace_back(name.c_str(),title.c_str(),init_val,min_val,max_val);
    background_parameter_arglist.add(background_parameters.back());
  }

  std::shared_ptr<RooRealVar> n_signal;
  std::shared_ptr<RooRealVar> n_background;

  std::pair<double,double> left_sideband;
  std::pair<double,double> right_sideband;

  // (optional) setup reasonable defaults for mass, nSignal, nBackground
  void setup_default_mass_nsignal_nbackground(const std::string& particle_name, const std::string& particle_title, float min_mass, float max_mass)
  {
    mass = std::make_shared<RooRealVar>((particle_name+"_mass").c_str(),(particle_title+" mass").c_str(),min_mass,max_mass);
    n_signal = std::make_shared<RooRealVar>((particle_name+"_nsignal").c_str(),(particle_title+" nSignal").c_str(),0.,1e12);
    n_background = std::make_shared<RooRealVar>((particle_name+"_nbackground").c_str(),(particle_title+" nBackground").c_str(),0.,1e12);
  }

  std::shared_ptr<RooAddPdf> fit_function;

  void fitTo(RooAbsData& ds)
  {
    fit_function->fitTo(ds,RooFit::Offset(true));
  }

  void generate_fitFunction()
  {
    fit_function = std::make_shared<RooAddPdf>((name+"_model").c_str(),"combined signal and background",RooArgList(*signal_function,*background_function),RooArgList(*n_signal,*n_background));
  }                                                                                                                                                 
};

#endif
