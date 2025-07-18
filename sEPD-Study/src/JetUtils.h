#pragma once

#include <string>
#include <vector>

class JetUtils
{
 public:
  static bool check_bad_jet_eta(double jet_eta, double zvtx, double jet_radius);

  static bool failsLoEmFracETCut(double emFrac, double ET);
  static bool failsHiEmFracETCut(double emFrac, double ET);

  static std::vector<std::string> split(const std::string &s, char delimiter);

  static void update_min_max(double val, double &val_min, double &val_max);
  static void update_min_max(int val, int &val_min, int &val_max);

 private:
  static double get_emcal_mineta_zcorrected(double zvtx);
  static double get_emcal_maxeta_zcorrected(double zvtx);
  static double get_ihcal_mineta_zcorrected(double zvtx);
  static double get_ihcal_maxeta_zcorrected(double zvtx);
  static double get_ohcal_mineta_zcorrected(double zvtx);
  static double get_ohcal_maxeta_zcorrected(double zvtx);

  constexpr static double radius_EM = 93.5;
  constexpr static double mineta_EM = -1.13381;
  constexpr static double maxeta_EM = 1.13381;
  constexpr static double minz_EM = -130.23;
  constexpr static double maxz_EM = 130.23;

  constexpr static double radius_IH = 127.503;
  constexpr static double mineta_IH = -1.1;
  constexpr static double maxeta_IH = 1.1;
  constexpr static double minz_IH = -170.299;
  constexpr static double maxz_IH = 170.299;

  constexpr static double radius_OH = 225.87;
  constexpr static double mineta_OH = -1.1;
  constexpr static double maxeta_OH = 1.1;
  constexpr static double minz_OH = -301.683;
  constexpr static double maxz_OH = 301.683;
};
