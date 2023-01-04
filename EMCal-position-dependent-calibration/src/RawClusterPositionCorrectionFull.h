#ifndef CALORECO_RAWCLUSTERPOSITIONCORRECTIONFULL_H
#define CALORECO_RAWCLUSTERPOSITIONCORRECTIONFULL_H

#include <fun4all/SubsysReco.h>

#include <phparameter/PHParameters.h>

#include <string>
#include <vector>
#include <math.h>

class PHCompositeNode;
class RawClusterContainer;

class RawClusterPositionCorrectionFull : public SubsysReco
{
 public:
  explicit RawClusterPositionCorrectionFull(const std::string &name);

  int InitRun(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int End(PHCompositeNode *topNode) override;

  void CreateNodeTree(PHCompositeNode *topNode);

  const PHParameters &Get_eclus_CalibrationParameters() const
  {
    return _eclus_calib_params;
  }
  PHParameters &Get_eclus_CalibrationParameters()
  {
    return _eclus_calib_params;
  }
  const PHParameters &Get_eore_CalibrationParameters() const
  {
    return _ecore_calib_params;
  }
  PHParameters &Get_ecore_CalibrationParameters()
  {
    return _ecore_calib_params;
  }

  void Set_eclus_CalibrationParameters(const PHParameters &calib_params)
  {
    _eclus_calib_params = calib_params;
  }
  void Set_ecore_CalibrationParameters(const PHParameters &calib_params)
  {
    _ecore_calib_params = calib_params;
  }

 private:
  PHParameters _eclus_calib_params;
  PHParameters _ecore_calib_params;
  void SetDefaultParameters(PHParameters &param);
  RawClusterContainer *_recalib_clusters;

  std::string _det_name;

  const double cluster_eta_max           = 1.152;
  const double sector_phi_boundary_low   = -M_PI / 32.;
  const double sector_phi_boundary_high  = M_PI / 32.;
  const double sector_phi_boundary_width = sector_phi_boundary_high - sector_phi_boundary_low;

  int bins_eta, bins_phi;
  std::vector<double> binvals_eta;
  std::vector<double> binvals_phi;
  std::vector<std::vector<double> > eclus_calib_constants;
  std::vector<std::vector<double> > ecore_calib_constants;
};

#endif
