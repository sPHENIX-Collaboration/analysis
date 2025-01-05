#include "JetUtils.h"
#include <math.h>
#include <sstream>

using std::stringstream;

bool JetUtils::check_bad_jet_eta(float jet_eta, float zvtx, float jet_radius)
{
  float emcal_mineta = get_emcal_mineta_zcorrected(zvtx);
  float emcal_maxeta = get_emcal_maxeta_zcorrected(zvtx);
  float ihcal_mineta = get_ihcal_mineta_zcorrected(zvtx);
  float ihcal_maxeta = get_ihcal_maxeta_zcorrected(zvtx);
  float ohcal_mineta = get_ohcal_mineta_zcorrected(zvtx);
  float ohcal_maxeta = get_ohcal_maxeta_zcorrected(zvtx);
  float minlimit = emcal_mineta;
  if (ihcal_mineta > minlimit) minlimit = ihcal_mineta;
  if (ohcal_mineta > minlimit) minlimit = ohcal_mineta;
  float maxlimit = emcal_maxeta;
  if (ihcal_maxeta < maxlimit) maxlimit = ihcal_maxeta;
  if (ohcal_maxeta < maxlimit) maxlimit = ohcal_maxeta;
  minlimit += jet_radius;
  maxlimit -= jet_radius;
  return jet_eta < minlimit || jet_eta > maxlimit;
}

float JetUtils::get_ohcal_mineta_zcorrected(float zvtx)
{
  float z = minz_OH - zvtx;
  float eta_zcorrected = asinh(z / (float) radius_OH);
  return eta_zcorrected;
}

float JetUtils::get_ohcal_maxeta_zcorrected(float zvtx)
{
  float z = maxz_OH - zvtx;
  float eta_zcorrected = asinh(z / (float) radius_OH);
  return eta_zcorrected;
}

float JetUtils::get_ihcal_mineta_zcorrected(float zvtx)
{
  float z = minz_IH - zvtx;
  float eta_zcorrected = asinh(z / (float) radius_IH);
  return eta_zcorrected;
}

float JetUtils::get_ihcal_maxeta_zcorrected(float zvtx)
{
  float z = maxz_IH - zvtx;
  float eta_zcorrected = asinh(z / (float) radius_IH);
  return eta_zcorrected;
}

float JetUtils::get_emcal_mineta_zcorrected(float zvtx)
{
  float z = minz_EM - zvtx;
  float eta_zcorrected = asinh(z / (float) radius_EM);
  return eta_zcorrected;
}

float JetUtils::get_emcal_maxeta_zcorrected(float zvtx)
{
  float z = maxz_EM - zvtx;
  float eta_zcorrected = asinh(z / (float) radius_EM);
  return eta_zcorrected;
}

vector<string> JetUtils::split(const string &s, const char delimiter) {
    vector<string> result;

    stringstream ss(s);
    string temp;

    while(getline(ss,temp,delimiter)) {
        if(!temp.empty()) {
            result.push_back(temp);
        }
    }

    return result;
}
