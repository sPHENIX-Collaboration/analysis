#include "CaloTemplateFit.h"

#include "PROTOTYPE4_FEM.h"
#include "RawTower_Prototype4.h"

#include <calobase/RawTower.h>  // for RawTower
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerDefs.h>  // for keytype

#include <phparameter/PHParameters.h>  // for PHParameters

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/SubsysReco.h>  // for SubsysReco

#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>    // for PHIODataNode
#include <phool/PHNode.h>          // for PHNode
#include <phool/PHNodeIterator.h>  // for PHNodeIterator
#include <phool/PHObject.h>        // for PHObject
#include <phool/getClass.h>

#include <boost/format.hpp>

#include <cassert>
#include <cmath>    // for NAN, isnan
#include <cstdlib>  // for exit
#include <iostream>
#include <limits>     // for numeric_limits
#include <map>        // for map, _Rb_tree_iter...
#include <stdexcept>  // for runtime_error
#include <string>
#include <utility>  // for pair
#include <vector>   // for vector
#include <TProfile.h>

#include <pthread.h>
#include <TFile.h>
#include <ROOT/TThreadExecutor.hxx>
#include <ROOT/TThreadedObject.hxx>
#include <iostream>
#include "TThread.h"
#include "TMutex.h"
#include "TMath.h"

#include "Math/WrappedTF1.h"
#include "Math/WrappedMultiTF1.h"
#include "Fit/BinData.h"
#include "Fit/UnBinData.h"
#include "HFitInterface.h"
#include "Fit/Fitter.h"
#include "Fit/Chi2FCN.h"
#include <pthread.h>
#include "Math/Functor.h" 
#include <TH1F.h>
using namespace std;





double CaloTemplateFit::template_function(double *x, double *par)
{
  Double_t v1 = par[0]*h_template->Interpolate(x[0]-par[1])+par[2];
  // Double_t v1 = par[0]*TMath::Power(x[0],par[1])+par[2];
  return v1;
}




std::vector<std::vector<float>> CaloTemplateFit::calo_processing_perchnl(std::vector<std::vector<float>> chnlvector)
{
  ROOT::TThreadExecutor t(_nthreads);
  auto func = [&](std::vector<float> &v) {
    int size1 = v.size()-1;
    auto h = new TH1F(Form("h_%d",(int)round(v.at(size1))),"",size1,0,size1);
    float maxheight = 0;
    int maxbin = 0;
    for (int i = 0; i < size1;i++)
      {
	h->SetBinContent(i+1,v.at(i));
	if (v.at(i)>maxheight)
	  {
	    maxheight = v.at(i);
	    maxbin = i;
	  }
      }
    float pedestal = 1500;
    if (maxbin > 4)
      {
	pedestal=  0.5* ( v.at(maxbin - 4) + v.at(maxbin-5));
      }
    else if (maxbin > 3)
      {
	pedestal=( v.at(maxbin - 4));
      }
    else 
      {
	pedestal = 0.5* ( v.at(size1-3) + v.at(size1-2)); 
      }
    auto f = new TF1(Form("f_%d",(int)round(v.at(size1))),template_function,0,31,3);
    ROOT::Math::WrappedMultiTF1 * fitFunction = new ROOT::Math::WrappedMultiTF1(*f, 3 );
    ROOT::Fit::BinData data(v.size()-1,1);
    ROOT::Fit::FillData(data,h);
    ROOT::Fit::Chi2Function *EPChi2 = new ROOT::Fit::Chi2Function(data, *fitFunction);
    ROOT::Fit::Fitter *fitter = new ROOT::Fit::Fitter();
    fitter->Config().MinimizerOptions().SetMinimizerType("GSLMultiFit");
    double params[] = {static_cast<double>(maxheight),static_cast<double>(maxbin-5),static_cast<double>(pedestal)};
    fitter->Config().SetParamsSettings(3,params);
    fitter->FitFCN(*EPChi2,0,data.Size(),true);
    for (int i =0;i<3;i++)
      {
	v.push_back(f->GetParameter(i));
      }
    h->Delete();
    f->Delete();
    delete fitFunction;
    delete fitter;
    delete EPChi2;
    // v.clear();
  };
  
  t.Foreach(func, chnlvector);
  
  int size3 = chnlvector.size();
  std::vector<std::vector<float>> fit_params;
  std::vector<float> fit_params_tmp;
  for (int i = 0; i < size3;i++)
    {
      std::vector<float> tv = chnlvector.at(i);
      int size2 = tv.size();
      for (int q = 3; q > 0;q--)
	{
	  fit_params_tmp.push_back(tv.at(size2-q));
	}
      fit_params.push_back(fit_params_tmp);
      fit_params_tmp.clear();
    }


  chnlvector.clear();
  return fit_params;
  fit_params.clear();
}




std::vector<float> CaloTemplateFit::calo_processing_singlethread(std::vector<float> chnlvector)
{
  int size1 = chnlvector.size();
  float maxheight = 0;
  int maxbin = 0;
  for (int i = 0; i < size1;i++)
    {
      h_data->SetBinContent(i+1,chnlvector.at(i));
      if (chnlvector.at(i)>maxheight)
	{
	  maxheight = chnlvector.at(i);
	  maxbin = i;
	}
    }
  float pedestal = 1500;
  if (maxbin > 4)
    {
      pedestal=  0.5* ( chnlvector.at(maxbin - 4) + chnlvector.at(maxbin-5));
    }
  else if (maxbin > 3)
    {
      pedestal=( chnlvector.at(maxbin - 4));
    }
  else 
    {
      pedestal = 0.5* ( chnlvector.at(size1-3) + chnlvector.at(size1-2)); 
    }
  ROOT::Math::WrappedMultiTF1 fitFunction(*f_fit, 3 );
  ROOT::Fit::BinData data(chnlvector.size()-1,1);
  ROOT::Fit::FillData(data,h_data);
  ROOT::Fit::Chi2Function EPChi2(data, fitFunction);
  ROOT::Fit::Fitter fitter;
  fitter.Config().MinimizerOptions().SetMinimizerType("GSLMultiFit");
  double params[] = {static_cast<double>(maxheight),static_cast<double>(maxbin-5),static_cast<double>(pedestal)};
  fitter.Config().SetParamsSettings(3,params);
  fitter.FitFCN(EPChi2,0,data.Size(),true);
  
  std::vector<float> fit_params;
  for (int q = 0; q < 3;q++)
    {
      fit_params.push_back(f_fit->GetParameter(q));
    }
  return fit_params;
  
  
}

//TProfile for the template
TProfile* CaloTemplateFit::h_template = nullptr;



//____________________________________
CaloTemplateFit::CaloTemplateFit(const std::string &name)
  : SubsysReco(string("CaloCalibration_") + name)
  , _calib_towers(nullptr)
  , _raw_towers(nullptr)
  , detector(name)
  , _calib_tower_node_prefix("CALIB")
  , _raw_tower_node_prefix("RAW")
  , _nthreads(1)
  , _nsamples(0)
  , template_input_file("/gpfs/mnt/gpfs02/sphenix/user/trinn/fitting_algorithm_playing/prdfcode/prototype/offline/packages/Prototype4/templates.root")
 , _calib_params(name)
  , _fit_type(kPowerLawDoubleExpWithGlobalFitConstraint)
{
  SetDefaultParameters(_calib_params);
}

//_____________________________________
int CaloTemplateFit::InitRun(PHCompositeNode *topNode)
{
  CreateNodeTree(topNode);

  if (Verbosity())
  {
    std::cout << Name() << "::" << detector << "::" << __PRETTY_FUNCTION__
              << " - print calibration parameters: " << endl;
    _calib_params.Print();
  }

  
  TFile* fin = TFile::Open(template_input_file.c_str());
  assert(fin);
  assert(fin->IsOpen());
  h_template = static_cast<TProfile*>(fin->Get("hp_electrons_fine_emcal_36_8GeV"));
  h_template->SetDirectory(0);
  fin->Close();
  delete fin;


  h_data = new TH1F("h_data","",32,0,32);
  f_fit = new TF1("f_fit",template_function,0,31,3);


  ROOT::EnableThreadSafety();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________
int CaloTemplateFit::process_event(PHCompositeNode *topNode)
{
  if (Verbosity())
  {
    std::cout << Name() << "::" << detector << "::" << __PRETTY_FUNCTION__
              << "Process event entered" << std::endl;
  }

  map<int, double> parameters_constraints;

  if ( _raw_towers->size() > 1)
    {
      if (Verbosity())
	{
	  std::cout
	    << Name() << "::" << detector << "::" << __PRETTY_FUNCTION__
	    << "Extract global fit parameter for constraining individual fits"
	    << std::endl;
	}
      // signal template
      vector<float> waveforms;
      vector<vector<float>> waveforms2;
      int count = 0;
      RawTowerContainer::Range begin_end = _raw_towers->getTowers();
      RawTowerContainer::Iterator rtiter;
      for (rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter)
	{
	  RawTower_Prototype4 *raw_tower =
	    dynamic_cast<RawTower_Prototype4 *>(rtiter->second);
	  assert(raw_tower);
	  ++count;

	  for (int i = 0; i < RawTower_Prototype4::NSAMPLES; i++)
	    {
	      if (i >= _nsamples && _nsamples != 0){continue;}
	      waveforms.push_back(raw_tower->get_signal_samples(i));
	    }
	    
	  // std::cout << waveforms.size() << std::endl;
	  waveforms.push_back(count);
	  waveforms2.push_back(waveforms);
	  waveforms.clear();
	}
       std::vector< std::vector<float>> pulse_quantification = calo_processing_perchnl(waveforms2);
    
       int towernumber = 0;

       for (rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter)
	 {
	   // RawTowerDefs::keytype key = rtiter->first;
	   RawTower_Prototype4 *raw_tower =
	     dynamic_cast<RawTower_Prototype4 *>(rtiter->second);
	   assert(raw_tower);
	   // store the result - raw_tower
	   if (std::isnan(raw_tower->get_energy()))
	     {
	       // Raw tower was never fit, store the current fit
	       std::vector<float> values = pulse_quantification.at(towernumber);
	       raw_tower->set_energy(values.at(0));
	       raw_tower->set_time(values.at(1));
	       values.clear();
	     }
	   towernumber++;
	 }
       pulse_quantification.clear();
       waveforms2.clear();
       // std::cout <<waveforms.size() << " , " <<  waveforms2.size() << " , "<< pulse_quantification.size() << std::endl;
   }


    /*
    if (count > 0)
    {
      for (int i = 0; i < RawTower_Prototype4::NSAMPLES; i++)
      {
        vec_signal_samples[i] /= count;
      }

      double peak = NAN;
      double peak_sample = NAN;
      double pedstal = NAN;
      map<int, double> parameters_io;

      PROTOTYPE4_FEM::SampleFit_PowerLawDoubleExp(vec_signal_samples, peak,
                                                  peak_sample, pedstal,
                                                  parameters_io, Verbosity());
      //    std::map<int, double> &parameters_io,  //! IO for fullset of
      //    parameters. If a parameter exist and not an NAN, the fit parameter
      //    will be fixed to that value. The order of the parameters are
      //    ("Amplitude", "Sample Start", "Power", "Peak Time 1", "Pedestal",
      //    "Amplitude ratio", "Peak Time 2")

      parameters_constraints[1] = parameters_io[1];
      parameters_constraints[2] = parameters_io[2];
      parameters_constraints[3] = parameters_io[3];
      parameters_constraints[5] = parameters_io[5];
      parameters_constraints[6] = parameters_io[6];

      //      //special constraint if Peak Time 1 == Peak Time 2
      //      if (abs(parameters_constraints[6] - parameters_constraints[3]) <
      //      0.1)
      //      {
      //        const double average_peak_time = (parameters_constraints[6] +
      //        parameters_constraints[3]) / 2.;
      //
      //        std::cout << Name() << "::" << detector << "::" <<
      //        __PRETTY_FUNCTION__
      //                  << ": two shaping time are too close "
      //                  << parameters_constraints[3] << " VS " <<
      //                  parameters_constraints[6]
      //                  << ". Use average peak time instead: " <<
      //                  average_peak_time
      //                  << std::endl;
      //
      //        parameters_constraints[6] = average_peak_time;
      //        parameters_constraints[3] = average_peak_time;
      //        parameters_constraints[5] = 0;
      //      }
    }
    else
    {
      std::cout << Name() << "::" << detector << "::" << __PRETTY_FUNCTION__
                << ": Failed to build signal template! Fit each channel "
                   "individually instead"
                << std::endl;
    }
  }
    */
  /*
  const double calib_const_scale =
      _calib_params.get_double_param("calib_const_scale");
  const bool use_chan_calibration =
      _calib_params.get_int_param("use_chan_calibration") > 0;

  RawTowerContainer::Range begin_end = _raw_towers->getTowers();
  RawTowerContainer::Iterator rtiter;
  for (rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter)
  {
    RawTowerDefs::keytype key = rtiter->first;
    RawTower_Prototype4 *raw_tower =
        dynamic_cast<RawTower_Prototype4 *>(rtiter->second);
    assert(raw_tower);

    double calibration_const = calib_const_scale;

    if (use_chan_calibration)
    {
      // channel to channel calibration.
      const int column = raw_tower->get_column();
      const int row = raw_tower->get_row();

      assert(column >= 0);
      assert(row >= 0);

      string calib_const_name(
          (boost::format("calib_const_column%d_row%d") % column % row).str());

      calibration_const *= _calib_params.get_double_param(calib_const_name);
    }

    vector<double> vec_signal_samples;
    for (int i = 0; i < RawTower_Prototype4::NSAMPLES; i++)
    {
      vec_signal_samples.push_back(raw_tower->get_signal_samples(i));
    }

    double peak = NAN;
    double peak_sample = NAN;
    double pedstal = NAN;

    switch (_fit_type)
    {
    case kPowerLawExp:
      PROTOTYPE4_FEM::SampleFit_PowerLawExp(vec_signal_samples, peak,
                                            peak_sample, pedstal, Verbosity());
      break;

    case kPeakSample:
      PROTOTYPE4_FEM::SampleFit_PeakSample(vec_signal_samples, peak,
                                           peak_sample, pedstal, Verbosity());
      break;

    case kPowerLawDoubleExp:
    {
      map<int, double> parameters_io;

      PROTOTYPE4_FEM::SampleFit_PowerLawDoubleExp(vec_signal_samples, peak,
                                                  peak_sample, pedstal,
                                                  parameters_io, Verbosity());
    }
    break;

    case kPowerLawDoubleExpWithGlobalFitConstraint:
    {
      map<int, double> parameters_io(parameters_constraints);

      PROTOTYPE4_FEM::SampleFit_PowerLawDoubleExp(vec_signal_samples, peak,
                                                  peak_sample, pedstal,
                                                  parameters_io, Verbosity());
    }
    break;
    default:
      cout << __PRETTY_FUNCTION__ << " - FATAL error - unkown fit type "
           << _fit_type << endl;
      exit(3);
      break;
    }

    // store the result - raw_tower
    if (std::isnan(raw_tower->get_energy()))
    {
      // Raw tower was never fit, store the current fit

      raw_tower->set_energy(peak);
      raw_tower->set_time(peak_sample);

      if (Verbosity())
      {
        raw_tower->identify();
      }
    }

    // store the result - calib_tower
    RawTower_Prototype4 *calib_tower = new RawTower_Prototype4(*raw_tower);
    calib_tower->set_energy(peak * calibration_const);
    calib_tower->set_time(peak_sample);

    for (int i = 0; i < RawTower_Prototype4::NSAMPLES; i++)
    {
      calib_tower->set_signal_samples(i, (vec_signal_samples[i] - pedstal) *
                                             calibration_const);
    }

    _calib_towers->AddTower(key, calib_tower);

  }  //  for (rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter)

  if (Verbosity())
  {
    std::cout << Name() << "::" << detector << "::" << __PRETTY_FUNCTION__
              << "input sum energy = " << _raw_towers->getTotalEdep()
              << ", output sum digitalized value = "
              << _calib_towers->getTotalEdep() << std::endl;
  }
*/


  return Fun4AllReturnCodes::EVENT_OK;
}

//_______________________________________
void CaloTemplateFit::CreateNodeTree(PHCompositeNode *topNode)
{
  PHNodeIterator iter(topNode);

  PHCompositeNode *dstNode =
      dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "DST"));
  if (!dstNode)
  {
    std::cerr << Name() << "::" << detector << "::" << __PRETTY_FUNCTION__
              << "DST Node missing, doing nothing." << std::endl;
    throw std::runtime_error(
        "Failed to find DST node in RawTowerCalibration::CreateNodes");
  }

  RawTowerNodeName = "TOWER_" + _raw_tower_node_prefix + "_" + detector;
  _raw_towers =
      findNode::getClass<RawTowerContainer>(dstNode, RawTowerNodeName.c_str());
  if (!_raw_towers)
  {
    std::cerr << Name() << "::" << detector << "::" << __PRETTY_FUNCTION__
              << " " << RawTowerNodeName << " Node missing, doing bail out!"
              << std::endl;
    throw std::runtime_error("Failed to find " + RawTowerNodeName +
                             " node in RawTowerCalibration::CreateNodes");
  }

  // Create the tower nodes on the tree
  PHNodeIterator dstiter(dstNode);
  PHCompositeNode *DetNode = dynamic_cast<PHCompositeNode *>(
      dstiter.findFirst("PHCompositeNode", detector));
  if (!DetNode)
  {
    DetNode = new PHCompositeNode(detector);
    dstNode->addNode(DetNode);
  }

  // Be careful as a previous calibrator may have been registered for this
  // detector
  CaliTowerNodeName = "TOWER_" + _calib_tower_node_prefix + "_" + detector;
  _calib_towers =
      findNode::getClass<RawTowerContainer>(DetNode, CaliTowerNodeName.c_str());
  if (!_calib_towers)
  {
    _calib_towers = new RawTowerContainer(_raw_towers->getCalorimeterID());
    PHIODataNode<PHObject> *towerNode = new PHIODataNode<PHObject>(
        _calib_towers, CaliTowerNodeName.c_str(), "PHObject");
    DetNode->addNode(towerNode);
  }

  // update the parameters on the node tree
  PHCompositeNode *parNode =
      dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "RUN"));
  assert(parNode);
  const string paramnodename = string("Calibration_") + detector;

  //   this step is moved to after detector construction
  //   save updated persistant copy on node tree
  _calib_params.SaveToNodeTree(parNode, paramnodename);
}

void CaloTemplateFit::SetDefaultParameters(PHParameters &param)
{
  param.set_int_param("use_chan_calibration", 0);

  // additional scale for the calibration constant
  // negative pulse -> positive with -1
  param.set_double_param("calib_const_scale", 1);
}
