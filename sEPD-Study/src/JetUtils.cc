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

std::pair<double, double> JetUtils::get_valid_eta_range(
    const double zvtx,
    const double jet_radius,
    const double r_em,
    const double r_ih,
    const double r_oh)
{
  double emcal_mineta = std::asinh((minz_EM - zvtx) / r_em);
  double emcal_maxeta = std::asinh((maxz_EM - zvtx) / r_em);
  double ihcal_mineta = std::asinh((minz_IH - zvtx) / r_ih);
  double ihcal_maxeta = std::asinh((maxz_IH - zvtx) / r_ih);
  double ohcal_mineta = std::asinh((minz_OH - zvtx) / r_oh);
  double ohcal_maxeta = std::asinh((maxz_OH - zvtx) / r_oh);

  double minlimit = emcal_mineta;
  minlimit = std::max(ihcal_mineta, minlimit);
  minlimit = std::max(ohcal_mineta, minlimit);
  double maxlimit = emcal_maxeta;
  maxlimit = std::min(ihcal_maxeta, maxlimit);
  maxlimit = std::min(ohcal_maxeta, maxlimit);
  minlimit += jet_radius;
  maxlimit -= jet_radius;
  return {minlimit, maxlimit};
}

bool JetUtils::check_bad_jet_eta(const double jet_eta, const double zvtx, const double jet_radius)
{
  auto [minlimit, maxlimit] = get_valid_eta_range(zvtx, jet_radius);
  return jet_eta < minlimit || jet_eta > maxlimit;
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

void JetUtils::update_min_max(const float val, float &val_min, float &val_max) {
  val_min = std::min(val_min, val);
  val_max = std::max(val_max, val);
}

void JetUtils::update_min_max(const int val, int &val_min, int &val_max) {
  val_min = std::min(val_min, val);
  val_max = std::max(val_max, val);
}
