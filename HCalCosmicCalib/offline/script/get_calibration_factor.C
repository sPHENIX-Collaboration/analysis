#include <iostream>
#include <fstream>
#include <TFile.h>
#include <TMath.h>

float stringtodouble(std::string str, bool& havenan) {
  if (str == "nan" || str == "NaN" || str == "NAN") {
    havenan = true;
    return 999999;
  } else {
    return std::stod(str);
  }
}

void reading_simulation_data(std::ifstream& simulation_data, float hcal_sim_mpv[24][64], float hcal_sim_mpverror[24][64], float hcal_sim_chi2[24][64]) {
  int ieta, iphi;
  int ieta_check = 0;
  int iphi_check = 0;
  std::string mpv_str, mpv_error_str, chi2_str, drop1_str, drop2_str;
  std::string line;
  while (std::getline(simulation_data, line)) {
    std::istringstream iss(line);

    if (!(iss >> ieta >> iphi)) {
      std::cerr << "Error reading ieta_check and iphi_check." << std::endl;
      return;
    }
    if (ieta != ieta_check || iphi != iphi_check) {
      std::cerr << "Error reading sim ieta_check and iphi_check. Mismatch: " << ieta << " - " << iphi << "    " << ieta_check << " - " << iphi_check << std::endl;
      return;
    }
    if (!(iss >> mpv_str >> mpv_error_str >> drop1_str >> drop2_str >> chi2_str)) {
      std::cerr << "Error reading data fields." << std::endl;
      return;
    }

    bool havenan = false;
    hcal_sim_mpv[ieta_check][iphi_check] = stringtodouble(mpv_str, havenan);
    hcal_sim_mpv[ieta_check][iphi_check] /= 32.;// Convert to low gain.
    hcal_sim_mpverror[ieta_check][iphi_check] = stringtodouble(mpv_error_str, havenan);
    hcal_sim_mpverror[ieta_check][iphi_check] /= 32.;// Convert to low gain.
    hcal_sim_chi2[ieta_check][iphi_check] = stringtodouble(chi2_str, havenan);
    if (havenan) {
      hcal_sim_chi2[ieta_check][iphi_check] = 999999;
      std::cout << "Reading Nan from Tower " << ieta_check << " - " << iphi_check << std::endl;
    }

    iphi_check = iphi+1;
    if (iphi_check == 64) {
      ieta_check = ieta+1;
      iphi_check = 0;
    }
  }
}

void get_calibration_factor(int min_run, int max_run) {
  static const int n_etabin = 24;
  static const int n_phibin = 64;
  
  // Read simulation.
  std::ifstream simulation_ohcaldata("script/fittingresult_ohcal.txt");
  std::ifstream simulation_ihcaldata("script/fittingresult_ihcal.txt");
  float ohcal_sim_mpv[n_etabin][n_phibin], ohcal_sim_mpverror[n_etabin][n_phibin], ohcal_sim_chi2[n_etabin][n_phibin];
  float ihcal_sim_mpv[n_etabin][n_phibin], ihcal_sim_mpverror[n_etabin][n_phibin], ihcal_sim_chi2[n_etabin][n_phibin];
  //std::cout << "Reading ohcal simulation data" << std::endl;
  reading_simulation_data(simulation_ohcaldata, ohcal_sim_mpv, ohcal_sim_mpverror, ohcal_sim_chi2);
  //std::cout << "Reading ihcal simulation data" << std::endl;
  reading_simulation_data(simulation_ihcaldata, ihcal_sim_mpv, ihcal_sim_mpverror, ihcal_sim_chi2);

  // Read precalib.
  std::ifstream precalib_ohcaldata("script/ohcal_precalib.txt");
  std::ifstream precalib_ihcaldata("script/ihcal_precalib.txt");
  float ohcal_precalib[n_etabin][n_phibin], ihcal_precalib[n_etabin][n_phibin];
  int ieta_check, iphi_check;
  int ieta, iphi;
  std::string mpv_str, mpv_error_str, chi2_str, drop1_str, drop2_str;
  for (int ieta = 0; ieta < n_etabin; ++ieta) {
    for (int iphi = 0; iphi < n_phibin; ++iphi) {
      precalib_ohcaldata >> ieta_check >> iphi_check >> ohcal_precalib[ieta][iphi];
      if (ieta_check != ieta || iphi_check != iphi) {
        std::cerr << "Error reading ohcal precalib data" << std::endl;
        return;
      }
      precalib_ihcaldata >> ieta_check >> iphi_check >> ihcal_precalib[ieta][iphi];
      if (ieta_check != ieta || iphi_check != iphi) {
        std::cerr << "Error reading ihcal precalib data" << std::endl;
        return;
      }
    }
  }

  // Read single fit result.
  float ohcal_data_mpv[n_etabin][n_phibin];
  std::ifstream ohcalfile("output/ohcal_mpv_" + std::to_string(min_run) + "_" + std::to_string(max_run) + ".txt");
  if (ohcalfile.is_open()) {
    std::string singleline;
    int single_ieta, single_iphi;
    std::string single_mpv_str;
    while (std::getline(ohcalfile, singleline)) {
      std::istringstream single_iss(singleline);
      single_iss >> single_ieta >> single_iphi >> single_mpv_str;
      bool havenan = false;
      ohcal_data_mpv[single_ieta][single_iphi] = stringtodouble(single_mpv_str, havenan);
    }
  }
  ohcalfile.close();

  float ihcal_data_mpv[n_etabin][n_phibin];
  std::ifstream ihcalfile("output/ihcal_mpv_" + std::to_string(min_run) + "_" + std::to_string(max_run) + ".txt");
  if (ihcalfile.is_open()) {
    std::string singleline;
    int single_ieta, single_iphi;
    std::string single_mpv_str;
    while (std::getline(ihcalfile, singleline)) {
      std::istringstream single_iss(singleline);
      single_iss >> single_ieta >> single_iphi >> single_mpv_str;
      bool havenan = false;
      ihcal_data_mpv[single_ieta][single_iphi] = stringtodouble(single_mpv_str, havenan);
    }
  }
  ihcalfile.close();

  // Calculate calibration factor.
  float ohcal_newcalib[n_etabin][n_phibin];
  ofstream ohcal_outputfile("output/ohcal_calibfactor_" + std::to_string(min_run) + "_" + std::to_string(max_run) + ".txt");
  for (int ieta = 0; ieta < n_etabin; ++ieta) {
    for (int iphi = 0; iphi < n_phibin; ++iphi) {
      // Convert to raw data.
      ohcal_newcalib[ieta][iphi] = ohcal_data_mpv[ieta][iphi]/(float)ohcal_precalib[ieta][iphi];
      // Convert to low gain.
      ohcal_newcalib[ieta][iphi] /= 32.;
      // Get ADC/GeV.
      ohcal_newcalib[ieta][iphi] /= ohcal_sim_mpv[ieta][iphi];
      // Get GeV/ADC.
      if (ohcal_newcalib[ieta][iphi] != 0) {
        ohcal_newcalib[ieta][iphi] = 1/(float)ohcal_newcalib[ieta][iphi];
      }
  
      ohcal_outputfile << ieta << " " << iphi << " " << ohcal_newcalib[ieta][iphi] << std::endl;
    }
  }
  ohcal_outputfile.close();

  float ihcal_newcalib[n_etabin][n_phibin];
  ofstream ihcal_outputfile("output/ihcal_calibfactor_" + std::to_string(min_run) + "_" + std::to_string(max_run) + ".txt");
  for (int ieta = 0; ieta < n_etabin; ++ieta) {
    for (int iphi = 0; iphi < n_phibin; ++iphi) {
      // Convert to raw data.
      ihcal_newcalib[ieta][iphi] = ihcal_data_mpv[ieta][iphi]/(float)ihcal_precalib[ieta][iphi];
      // Convert to low gain.
      ihcal_newcalib[ieta][iphi] /= 32.;
      // Get ADC/GeV.
      ihcal_newcalib[ieta][iphi] /= ihcal_sim_mpv[ieta][iphi];
      // Get GeV/ADC.
      if (ihcal_newcalib[ieta][iphi] != 0) {
        ihcal_newcalib[ieta][iphi] = 1/(float)ihcal_newcalib[ieta][iphi];
      }
  
      ihcal_outputfile << ieta << " " << iphi << " " << ihcal_newcalib[ieta][iphi] << std::endl;
    }
  }
  ihcal_outputfile.close();
}