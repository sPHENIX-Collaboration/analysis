#ifndef LAMBDAMODEL_H
#define LAMBDAMODEL_H

#include "ParticleModel.h"

#include <RooAddPdf.h>
#include <RooCrystalBall.h>

struct LambdaModel : ParticleModel
{
  LambdaModel()
  {
    name = "Lambda0";

    setup_default_mass_nsignal_nbackground("Lambda0","#Lambda^{0}",1.08,1.15);

    add_signal_parameter("lambda_mean","mean",1.1145,1.105,1.125);
    add_signal_parameter("lambda_mean2","mean2",1.115,1.105,1.125);
    add_signal_parameter("lambda_mean3","mean3",1.1155,1.105,1.125);
    add_signal_parameter("lambda_width","width",0.002,0.001,0.01);
    add_signal_parameter("lambda_width2","width2",0.002,0.001,0.01);
    add_signal_parameter("lambda_width3","width3",0.002,0.001,0.01);

    signal_gaus1 = std::make_shared<RooGaussian>("lambda_gaus1","signal_gaus1",*mass,signal_parameters[0],signal_parameters[3]);
    signal_gaus2 = std::make_shared<RooGaussian>("lambda_gaus2","signal_gaus2",*mass,signal_parameters[0],signal_parameters[4]);
    signal_gaus3 = std::make_shared<RooGaussian>("lambda_gaus3","signal_gaus3",*mass,signal_parameters[0],signal_parameters[5]);
    n_gaus1 = std::make_shared<RooRealVar>("lambda_ngaus1","gaus 1",0.,1.);
    n_gaus2 = std::make_shared<RooRealVar>("lambda_ngaus2","gaus 2",0.,1.);
    n_gaus3 = std::make_shared<RooRealVar>("lambda_ngaus3","gaus 3",0.,1.);

    //signal_function = std::make_shared<RooGaussian>("ks_signal","signal",*mass,signal_parameters[0],signal_parameters[2]);
    signal_function = std::make_shared<RooAddPdf>("lambda_signal","double gaussian signal",RooArgList(*signal_gaus1,*signal_gaus2,*signal_gaus3),RooArgList(*n_gaus1,*n_gaus2,*n_gaus3));

/*
    add_signal_parameter("lambda_mean","mean",1.113,1.1,1.14);
    add_signal_parameter("lambda_mean2","mean2",1.113,1.1,1.14);
    add_signal_parameter("lambda_width","width",0.01,0.0001,0.2);
    add_signal_parameter("lambda_width2","width2",0.01,0.0001,0.2);

    signal_gaus1 = std::make_shared<RooGaussian>("lambda_gaus1","signal_gaus1",*mass,signal_parameters[0],signal_parameters[2]);
    signal_gaus2 = std::make_shared<RooGaussian>("lambda_gaus2","signal_gaus2",*mass,signal_parameters[1],signal_parameters[3]);
    n_gaus1 = std::make_shared<RooRealVar>("lngaus1","gaus 1",100.,0.,1e9);
    n_gaus2 = std::make_shared<RooRealVar>("lngaus2","gaus 2",100.,0.,1e9);

    //signal_function = std::make_shared<RooGaussian>("ks_signal","signal",*mass,signal_parameters[0],signal_parameters[2]);
    signal_function = std::make_shared<RooAddPdf>("ks_signal","double gaussian signal",RooArgList(*signal_gaus1,*signal_gaus2),RooArgList(*n_gaus1,*n_gaus2));


    signal_parameters.emplace_back("lambda_mean","mean",1.113,1.1,1.14);
    signal_parameters.emplace_back("lambda_width","width",0.01,0.0001,0.2);

    signal_function = std::make_shared<RooGaussian>("lambda_signal","signal",*mass,signal_parameters[0],signal_parameters[1]);
*/
    background_parameters.emplace_back("q1","q1",0.5,0.,1.);
    background_parameters.emplace_back("q2","q2",0.5,0.,1.);
    background_parameters.emplace_back("q3","q3",0.5,0.,1.);
    background_parameters.emplace_back("q4","q4",0.5,0.,1.);
    background_parameters.emplace_back("q5","q5",0.5,0.,1.);
    background_parameters.emplace_back("q6","q6",0.5,0.,1.);

   background_function = std::make_shared<RooBernstein>("lambda_bkg","background",*mass,RooArgList(background_parameters.begin(),background_parameters.end()));
/*
    add_signal_parameter("lambda_x0","x0",1.113,1.11,1.12);
    add_signal_parameter("lambda_sigmaL","sigma_L",0.01,0.0001,0.2);
    add_signal_parameter("lambda_sigmaR","sigma_R",0.01,0.0001,0.2);
    add_signal_parameter("lambda_alphaL","alpha_L",0.01,0.0001,100.);
    add_signal_parameter("lambda_nL","n_L",1.,0.0001,5.);
    add_signal_parameter("lambda_alphaR","alpha_R",0.01,0.0001,100.);
    add_signal_parameter("lambda_nR","n_R",1.,0.0001,5.);

    signal_function = std::make_shared<RooCrystalBall>("lambda_signal","signal",*mass,signal_parameters[0],signal_parameters[1],signal_parameters[2],signal_parameters[3],signal_parameters[4],signal_parameters[5],signal_parameters[6]);
*/
/*
    add_background_parameter("lambda_k","k",-1.,-10.,10.);

    //background_function = std::shared_ptr<RooAbsPdf>(RooFit::bindPdf(fThresholdBackground.get(),*mass));
    background_function = std::make_shared<RooExponential>("lambda_background","background",*mass,background_parameters[0]);
*/

    left_sideband = {1.09,1.104};
    right_sideband = {1.124,1.15};

  }

  double minMassThreshold(const double mother_pt)
  {
    const double lambda_mass = 1.115;
    const double pi_mass = 0.4976;
    const double p_mass = 0.938;

    // min mass for daughters given mother pT
    // first, CM frame parameters
    const double mother_gamma = sqrt(pow(mother_pt/lambda_mass,2.)+1.);
    const double cmframe_pt = 0.1;
    const double pi_gamma_cm = sqrt(pow(cmframe_pt/pi_mass,2.)+1.);
    const double p_gamma_cm = sqrt(pow(cmframe_pt/p_mass,2.)+1.);
    const double pi_E_cm = pi_gamma_cm*pi_mass;
    const double p_E_cm = p_gamma_cm*p_mass;
    // then, boost to lab frame with pion (lower-pT particle from kinematics) anti-aligned with mother pT
    return 0.;
  }

  std::shared_ptr<RooGaussian> signal_gaus1;
  std::shared_ptr<RooGaussian> signal_gaus2;
  std::shared_ptr<RooGaussian> signal_gaus3;
  std::shared_ptr<RooRealVar> n_gaus1;
  std::shared_ptr<RooRealVar> n_gaus2;
  std::shared_ptr<RooRealVar> n_gaus3;

};

#endif
