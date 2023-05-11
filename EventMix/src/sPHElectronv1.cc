#include "sPHElectronv1.h"

#include <trackbase_historic/SvtxTrack.h>
#include <trackbase/TrkrDefs.h>

#include <cmath>
#include <utility>          // for swap

sPHElectronv1::sPHElectronv1()
{
  _id = 99999;
  _charge = 0;
  _px = 0.;
  _py = 0.;
  _pz = 0.;
  _emce = 0.;
  _dphi = 99999.;
  _deta = 99999.;
  _e3x3 = 0.;
  _e5x5 = 0.;

  _chi2 = 99999.;
  _ndf = 0;
  _zvtx = 99999.;
  _dca2d = 99999.;
  _dca2d_error = 99999.;
  _dca3d_xy = 99999.;
  _dca3d_z = 99999.;

  _nmvtx = 0;
  _ntpc = 0;

  _cemc_ecore = 0.;
  _cemc_chi2 = 99999.;
  _cemc_prob = 99999.;
  _cemc_dphi = 99999.;
  _cemc_deta = 99999.;
  _hcalin_e = 99999.;
  _hcalin_dphi = 99999.;
  _hcalin_deta = 99999.;

}

sPHElectronv1::sPHElectronv1(const SvtxTrack* trk) {
  _id = trk->get_id();
  _charge = trk->get_charge();
  _px = trk->get_px(); 
  _py = trk->get_py(); 
  _pz = trk->get_pz(); 
  _dphi = trk->get_cal_dphi(SvtxTrack::CAL_LAYER::CEMC); 
  _deta = trk->get_cal_deta(SvtxTrack::CAL_LAYER::CEMC);
  _emce = trk->get_cal_cluster_e(SvtxTrack::CAL_LAYER::CEMC);
  _e3x3 = trk->get_cal_energy_3x3(SvtxTrack::CAL_LAYER::CEMC);
  _e5x5 = trk->get_cal_energy_5x5(SvtxTrack::CAL_LAYER::CEMC);

  _chi2 = trk->get_chisq();
  _ndf = trk->get_ndf();
  _zvtx = trk->get_z();
  _dca2d = trk->get_dca2d();
  _dca2d_error = trk->get_dca2d_error();
  _dca3d_xy = trk->get_dca3d_xy();
  _dca3d_z = trk->get_dca3d_z();

  _cemc_ecore = 0.;
  _cemc_chi2  = 99999.;
  _cemc_prob  = 99999.;
  _cemc_dphi  = trk->get_cal_dphi(SvtxTrack::CAL_LAYER::CEMC);
  _cemc_deta  = trk->get_cal_deta(SvtxTrack::CAL_LAYER::CEMC);
  _hcalin_e    = trk->get_cal_cluster_e(SvtxTrack::CAL_LAYER::HCALIN);
  _hcalin_dphi = trk->get_cal_dphi(SvtxTrack::CAL_LAYER::HCALIN);
  _hcalin_deta = trk->get_cal_deta(SvtxTrack::CAL_LAYER::HCALIN);

  _nmvtx = 0;
  _ntpc = 0;
  for (SvtxTrack::ConstClusterKeyIter iter = trk->begin_cluster_keys();
     iter != trk->end_cluster_keys();
     ++iter)
  {
    TrkrDefs::cluskey cluster_key = *iter;
    int trackerid = TrkrDefs::getTrkrId(cluster_key);
    if(trackerid==0) _nmvtx++;
    if(trackerid==2) _ntpc++;
  }
}

sPHElectronv1::sPHElectronv1(const sPHElectronv1& electron)
{
  *this = electron;
  return;
}

sPHElectronv1& sPHElectronv1::operator=(const sPHElectronv1& electron)
{
  _id = electron.get_id();
  _charge = electron.get_charge();
  _px = electron.get_px();
  _py = electron.get_py();
  _pz = electron.get_pz();
  _dphi = electron.get_dphi();
  _deta = electron.get_deta();
  _emce = electron.get_emce();
  _e3x3 = electron.get_e3x3();
  _e5x5 = electron.get_e5x5();

  _chi2 = electron.get_chi2();
  _ndf = electron.get_ndf();
  _zvtx = electron.get_zvtx();
  _dca2d = electron.get_dca2d();
  _dca2d_error = electron.get_dca2d_error();
  _dca3d_xy = electron.get_dca3d_xy();
  _dca3d_z = electron.get_dca3d_z();

  _nmvtx = electron.get_nmvtx();
  _ntpc = electron.get_ntpc();

  _cemc_ecore = electron.get_cemc_ecore();
  _cemc_chi2  = electron.get_cemc_chi2();
  _cemc_prob  = electron.get_cemc_prob();
  _cemc_dphi  = electron.get_cemc_dphi();
  _cemc_deta  = electron.get_cemc_deta();
  _hcalin_e    = electron.get_hcalin_e();
  _hcalin_dphi = electron.get_hcalin_dphi();
  _hcalin_deta = electron.get_hcalin_deta();

  return *this;
}

void sPHElectronv1::identify(std::ostream& os) const
{
  os << " sPHElectronv1 object" << std::endl;
  return;
}

int sPHElectronv1::isValid() const
{
  if(_charge!=0) {return 1;} else {return 0;}
}

