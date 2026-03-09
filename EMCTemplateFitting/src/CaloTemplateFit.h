// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef PROTOTYPE4_CALOTEMPLATEFIT_H
#define PROTOTYPE4_CALOTEMPLATEFIT_H

//* Unpacks raw HCAL PRDF files *//
// Abhisek Sen

#include <fun4all/SubsysReco.h>

#include <phparameter/PHParameters.h>

#include <string>
#include<TProfile.h>
#include<TH1.h>
#include <TThread.h>
#include <ROOT/TThreadExecutor.hxx>
#include <ROOT/TThreadedObject.hxx>
#include "TThread.h"

class PHCompositeNode;
class RawTowerContainer;

class CaloTemplateFit : public SubsysReco
{
 public:
  CaloTemplateFit(const std::string &name);

  int InitRun(PHCompositeNode *topNode);

  int process_event(PHCompositeNode *topNode);

  void CreateNodeTree(PHCompositeNode *topNode);

  std::string get_calib_tower_node_prefix() const
  {
    return _calib_tower_node_prefix;
  }

  void set_calib_tower_node_prefix(const std::string &calibTowerNodePrefix)
  {
    _calib_tower_node_prefix = calibTowerNodePrefix;
  }

  std::string get_raw_tower_node_prefix() const
  {
    return _raw_tower_node_prefix;
  }

  void set_raw_tower_node_prefix(const std::string &rawTowerNodePrefix)
  {
    _raw_tower_node_prefix = rawTowerNodePrefix;
  }

  void set_nthreads(int nthreads)
  {
    _nthreads= nthreads;
  }
  void set_nsamples(int nsamples)
  {
    _nsamples= nsamples;
  }
  void set_templatefile (const std::string &templatename)
  {
    template_input_file =templatename;
  }


  //! Get the parameters for readonly
  const PHParameters &GetCalibrationParameters() const { return _calib_params; } 

  //! Get the parameters for update. Useful fields are listed in
  //! SetDefaultParameters();
  PHParameters &GetCalibrationParameters() { return _calib_params; } 

  //! Overwrite the parameter. Useful fields are listed in
  //! SetDefaultParameters();
   void SetCalibrationParameters(const PHParameters &calib_params) 
   { 
     _calib_params = calib_params; 
   } 

  enum FitMethodType
  {
    //! single power-low-exp fit, PROTOTYPE4_FEM::SampleFit_PowerLawExp()
    kPowerLawExp,

    //! power-low-double-exp fit, PROTOTYPE4_FEM::SampleFit_PowerLawDoubleExp
    kPowerLawDoubleExp,

    //! power-low-double-exp fit, PROTOTYPE4_FEM::SampleFit_PowerLawDoubleExp,
    //! and constraining all tower take identical shape
    kPowerLawDoubleExpWithGlobalFitConstraint,

    //! just use the peak sample, PROTOTYPE4_FEM::SampleFit_PeakSample()
    kPeakSample

  };

  void SetFitType(FitMethodType t) { _fit_type = t; } 

 private:
  RawTowerContainer *_calib_towers;
  RawTowerContainer *_raw_towers;

  std::string detector;
  std::string RawTowerNodeName;
  std::string CaliTowerNodeName;

  std::string _calib_tower_node_prefix;
  std::string _raw_tower_node_prefix;


  int _nthreads;

  int _nsamples;


  static TProfile* h_template; 
  static double template_function(double *x, double *par);
  std::vector<std::vector<float>>  calo_processing_perchnl(std::vector<std::vector<float>> chnlvector);
  std::vector<float>  calo_processing_singlethread(std::vector<float> chnlvector);
  /* ROOT::TThreadedObject<TF1> testfit; */
  std::string template_input_file;

  TH1F* h_data;
  TF1* f_fit;

  PHParameters _calib_params;

  FitMethodType _fit_type; 

  //! load the default parameter to param
  void SetDefaultParameters(PHParameters &param);
};

#endif
