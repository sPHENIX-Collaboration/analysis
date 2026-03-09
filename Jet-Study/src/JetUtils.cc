#include "JetUtils.h"
#include <math.h>
#include <sstream>
#include <fstream>

using std::stringstream;
using std::cout;
using std::endl;
using std::make_pair;

vector<string> JetUtils::m_triggers = {"Clock"
                                      ,"ZDC South"
                                      ,"ZDC North"
                                      ,"ZDC Coincidence"
                                      ,"HCAL Singles/Coincidence"
                                      ,"Clock2"
                                      ,"unknown6"
                                      ,"unknown7"
                                      ,"MBD S >= 1"
                                      ,"MBD N >= 1"
                                      ,"MBD N&S >= 1"
                                      ,"MBD N&S >= 2"
                                      ,"MBD N&S >= 1, vtx < 10 cm"
                                      ,"MBD N&S >= 1, vtx < 30 cm"
                                      ,"MBD N&S >= 1, vtx < 60 cm"
                                      ,"HCAL Singles + MBD NS >= 1"
                                      ,"Jet 6 GeV + MBD NS >= 1"
                                      ,"Jet 8 GeV + MBD NS >= 1"
                                      ,"Jet 10 GeV + MBD NS >= 1"
                                      ,"Jet 12 GeV + MBD NS >= 1"
                                      ,"Jet 6 GeV"
                                      ,"Jet 8 GeV"
                                      ,"Jet 10 GeV"
                                      ,"Jet 12 GeV"
                                      ,"Photon 2 GeV+ MBD NS >= 1"
                                      ,"Photon 3 GeV + MBD NS >= 1"
                                      ,"Photon 4 GeV + MBD NS >= 1"
                                      ,"Photon 5 GeV + MBD NS >= 1"
                                      ,"Photon 2 GeV"
                                      ,"Photon 3 GeV"
                                      ,"Photon 4 GeV"
                                      ,"Photon 5 GeV"
                                      ,"Jet 6 GeV, MBD N&S >= 1, vtx < 10 cm"
                                      ,"Jet 8 GeV, MBD N&S >= 1, vtx < 10 cm"
                                      ,"Jet 10 GeV, MBD N&S >= 1, vtx < 10 cm"
                                      ,"Jet 12 GeV, MBD N&S >= 1, vtx < 10 cm"
                                      ,"Photon 3 GeV, MBD N&S >= 1, vtx < 10 cm"
                                      ,"Photon 4 GeV, MBD N&S >= 1, vtx < 10 cm"
                                      ,"Photon 5 GeV, MBD N&S >= 1, vtx < 10 cm"
                                      ,"unknown39"
                                      ,"MBD Laser"
                                      ,"None"
                                     };

int JetUtils::readEventList(const string &input, vector<pair<int, int>> &vec, int nEvents, bool verbose)
{
  std::ifstream file(input);

  // Check if the file was successfully opened
  if (!file.is_open())
  {
    cout << "Failed to open file: " << input << endl;
    return 1;
  }

  string line;

  // keep header
  std::getline(file, line);

  // loop over each run
  while (std::getline(file, line))
  {
    std::istringstream lineStream(line);

    int run;
    int event;
    char comma;

    if (lineStream >> run >> comma >> event)
    {
      for (int i = event - nEvents; i <= event + nEvents; ++i)
      {
        vec.push_back(make_pair(run, i));
      }
    }
    else
    {
      cout << "Failed to parse line: " << line << endl;
      return 1;
    }
  }

  // print the events of interest
  if (verbose)
  {
    for (auto p : vec)
    {
      cout << "Run: " << p.first << ", Event: " << p.second << endl;
    }
  }

  // Close the file
  file.close();

  return 0;
}

bool JetUtils::failsLoEmFracETCut(float emFrac, float ET)
{
  return emFrac < 0.1 && ET > (50*emFrac+20);
}

bool JetUtils::failsHiEmFracETCut(float emFrac, float ET)
{
  return emFrac > 0.9 && ET > (-50*emFrac+70);
}

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
