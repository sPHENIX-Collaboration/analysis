#include "sPHElectronv1.h"

#include <trackbase_historic/SvtxTrack.h>

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

  return *this;
}

void sPHElectronv1::identify(std::ostream& os) const
{
  os << " sPHElectronv1 object" << std::endl;
  return;
}

int sPHElectronv1::isValid() const
{
  return 1;
}

