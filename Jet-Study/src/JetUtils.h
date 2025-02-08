#ifndef JETUTILS_H_
#define JETUTILS_H_

#include <iostream>
#include <string>
#include <vector>

using std::string;
using std::vector;
using std::pair;

class JetUtils
{
 public:
  static bool check_bad_jet_eta(float jet_eta, float zvtx, float jet_radius);
  static vector<string> split(const string &s, const char delimiter);
  static int readEventList(const string &input, vector<pair<int, int>> &vec, int nEvents=0, bool verbose=false);

  static vector<string> m_triggers;

  static bool failsLoEmFracETCut(float emFrac, float ET);
  static bool failsHiEmFracETCut(float emFrac, float ET);

 private:
  static float get_emcal_mineta_zcorrected(float zvtx);
  static float get_emcal_maxeta_zcorrected(float zvtx);
  static float get_ihcal_mineta_zcorrected(float zvtx);
  static float get_ihcal_maxeta_zcorrected(float zvtx);
  static float get_ohcal_mineta_zcorrected(float zvtx);
  static float get_ohcal_maxeta_zcorrected(float zvtx);

  constexpr static float radius_EM = 93.5;
  constexpr static float mineta_EM = -1.13381;
  constexpr static float maxeta_EM = 1.13381;
  constexpr static float minz_EM = -130.23;
  constexpr static float maxz_EM = 130.23;

  constexpr static float radius_IH = 127.503;
  constexpr static float mineta_IH = -1.1;
  constexpr static float maxeta_IH = 1.1;
  constexpr static float minz_IH = -170.299;
  constexpr static float maxz_IH = 170.299;

  constexpr static float radius_OH = 225.87;
  constexpr static float mineta_OH = -1.1;
  constexpr static float maxeta_OH = 1.1;
  constexpr static float minz_OH = -301.683;
  constexpr static float maxz_OH = 301.683;
};

#endif  // JETUTILS_H_
