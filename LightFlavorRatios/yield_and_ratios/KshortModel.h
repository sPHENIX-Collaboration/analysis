#ifndef KSHORTMODEL_H
#define KSHORTMODEL_H

#include "ParticleModel.h"

struct KshortModel : ParticleModel
{
  KshortModel()
  {
    name = "K_S0";

    setup_default_mass_nsignal_nbackground("K_S0","K_{S}^{0}",0.4,0.6);

    add_signal_parameter("ks_mean","mean",0.496,0.48,0.51);
    add_signal_parameter("ks_mean2","mean2",0.497,0.48,0.51);
    add_signal_parameter("ks_mean3","mean3",0.496,0.48,0.51);
    add_signal_parameter("ks_width","width",0.005,0.0001,0.015);
    add_signal_parameter("ks_width2","width2",0.005,0.0001,0.015);
    add_signal_parameter("ks_width3","width3",0.005,0.0001,0.015);

    signal_gaus1 = std::make_shared<RooGaussian>("ks_gaus1","signal_gaus1",*mass,signal_parameters[0],signal_parameters[3]);
    signal_gaus2 = std::make_shared<RooGaussian>("ks_gaus2","signal_gaus2",*mass,signal_parameters[0],signal_parameters[4]);
    signal_gaus3 = std::make_shared<RooGaussian>("ks_gaus3","signal_gaus3",*mass,signal_parameters[0],signal_parameters[5]);
    n_gaus1 = std::make_shared<RooRealVar>("ks_ngaus1","gaus 1",0.,1e12);
    n_gaus2 = std::make_shared<RooRealVar>("ks_ngaus2","gaus 2",0.,1e12);
    n_gaus3 = std::make_shared<RooRealVar>("ks_ngaus3","gaus 3",0.,1e12);

    //signal_function = std::make_shared<RooGaussian>("ks_signal","signal",*mass,signal_parameters[0],signal_parameters[2]);
    signal_function = std::make_shared<RooAddPdf>("ks_signal","double gaussian signal",RooArgList(*signal_gaus1,*signal_gaus2),RooArgList(*n_gaus1,*n_gaus2));

    background_parameters.emplace_back("ks_q1","q1",0.,0.,1.);
    background_parameters.emplace_back("ks_q2","q2",0.,0.,1.);
    background_parameters.emplace_back("ks_q3","q3",0.,0.,1.);
    background_parameters.emplace_back("ks_q4","q4",0.,0.,1.);
    background_parameters.emplace_back("ks_q5","q5",0.,0.,1.);
    background_parameters.emplace_back("ks_q6","q6",0.,0.,1.);

    background_function = std::make_shared<RooBernstein>("ks_bkg","background",*mass,RooArgList(background_parameters.begin(),background_parameters.end()));


    //add_background_parameter("ks_k","k",-1.,-100.,100.);

    //background_function = std::make_shared<RooExponential>("ks_bkg","background",*mass,background_parameters[0]);

    left_sideband = {0.4,0.46};
    right_sideband = {0.54,0.6};

  }

  std::shared_ptr<RooGaussian> signal_gaus1;
  std::shared_ptr<RooGaussian> signal_gaus2;
  std::shared_ptr<RooGaussian> signal_gaus3;
  std::shared_ptr<RooRealVar> n_gaus1;
  std::shared_ptr<RooRealVar> n_gaus2;
  std::shared_ptr<RooRealVar> n_gaus3;
};

#endif
