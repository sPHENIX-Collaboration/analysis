#include "JetUtils.h"
#include <sstream>
#include <cmath>

bool JetUtils::failsLoEmFracETCut(const double emFrac, const double ET)
{
  return emFrac < 0.1 && ET > (50*emFrac+20);
}

bool JetUtils::failsHiEmFracETCut(const double emFrac, const double ET)
{
  return emFrac > 0.9 && ET > (-50*emFrac+70);
}

bool JetUtils::check_bad_jet_eta(const double jet_eta, const double zvtx, const double jet_radius)
{
  double emcal_mineta = get_emcal_mineta_zcorrected(zvtx);
  double emcal_maxeta = get_emcal_maxeta_zcorrected(zvtx);
  double ihcal_mineta = get_ihcal_mineta_zcorrected(zvtx);
  double ihcal_maxeta = get_ihcal_maxeta_zcorrected(zvtx);
  double ohcal_mineta = get_ohcal_mineta_zcorrected(zvtx);
  double ohcal_maxeta = get_ohcal_maxeta_zcorrected(zvtx);
  double minlimit = emcal_mineta;
  minlimit = std::max(ihcal_mineta, minlimit);
  minlimit = std::max(ohcal_mineta, minlimit);
  double maxlimit = emcal_maxeta;
  maxlimit = std::min(ihcal_maxeta, maxlimit);
  maxlimit = std::min(ohcal_maxeta, maxlimit);
  minlimit += jet_radius;
  maxlimit -= jet_radius;
  return jet_eta < minlimit || jet_eta > maxlimit;
}

double JetUtils::get_ohcal_mineta_zcorrected(const double zvtx)
{
  double z = minz_OH - zvtx;
  double eta_zcorrected = std::asinh(z / radius_OH);
  return eta_zcorrected;
}

double JetUtils::get_ohcal_maxeta_zcorrected(const double zvtx)
{
  double z = maxz_OH - zvtx;
  double eta_zcorrected = std::asinh(z / radius_OH);
  return eta_zcorrected;
}

double JetUtils::get_ihcal_mineta_zcorrected(const double zvtx)
{
  double z = minz_IH - zvtx;
  double eta_zcorrected = std::asinh(z / radius_IH);
  return eta_zcorrected;
}

double JetUtils::get_ihcal_maxeta_zcorrected(const double zvtx)
{
  double z = maxz_IH - zvtx;
  double eta_zcorrected = std::asinh(z / radius_IH);
  return eta_zcorrected;
}

double JetUtils::get_emcal_mineta_zcorrected(const double zvtx)
{
  double z = minz_EM - zvtx;
  double eta_zcorrected = std::asinh(z / radius_EM);
  return eta_zcorrected;
}

double JetUtils::get_emcal_maxeta_zcorrected(const double zvtx)
{
  double z = maxz_EM - zvtx;
  double eta_zcorrected = std::asinh(z / radius_EM);
  return eta_zcorrected;
}

std::vector<std::string> JetUtils::split(const std::string &s, const char delimiter) {
    std::vector<std::string> result;

    std::stringstream ss(s);
    std::string temp;

    while(getline(ss,temp,delimiter)) {
        if(!temp.empty()) {
            result.push_back(temp);
        }
    }

    return result;
}

void JetUtils::update_min_max(const double val, double &val_min, double &val_max) {
  val_min = std::min(val_min, val);
  val_max = std::max(val_max, val);
}

void JetUtils::update_min_max(const int val, int &val_min, int &val_max) {
  val_min = std::min(val_min, val);
  val_max = std::max(val_max, val);
}
