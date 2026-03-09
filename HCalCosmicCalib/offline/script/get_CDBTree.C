#include <cdbobjects/CDBTTree.h>
#include <calobase/TowerInfoDefs.h>
#include <iostream>
#include <fstream>
#include <sstream>

R__LOAD_LIBRARY(libcdbobjects.so)
R__LOAD_LIBRARY(libcalo_io.so)

void form_CDBTree(std::string CDB_name, std::string fieldname, float calibration_factor[24][64]) {
  CDBTTree* cdbttree = new CDBTTree(CDB_name);
  std::string m_fieldname = fieldname;
  for (int ieta = 0; ieta < 24; ++ieta) {
    for (int iphi = 0; iphi < 64; ++iphi) {
      unsigned int key = TowerInfoDefs::encode_hcal(ieta, iphi);
      cdbttree->SetFloatValue(key, fieldname, calibration_factor[ieta][iphi]);
    }
  }
  cdbttree->Commit();
  cdbttree->WriteCDBTTree();
  delete cdbttree;
}

void get_CDBTree(int min_run, int max_run) {
  std::ifstream ohcal_datafile("output/ohcal_calibfactor_" + std::to_string(min_run) + "_" + std::to_string(max_run) + ".txt");
  if (!ohcal_datafile.is_open()) {
    std::cerr << "Error opening file for ohcal_calibfactor.txt" << std::endl;
    return;
  }
  std::string line;
  int ieta, iphi;
  float calib;
  float ohcal_calibfactor[24][64];
  while (std::getline(ohcal_datafile, line)) {
    std::istringstream iss(line);
    iss >> ieta >> iphi >> calib;
    ohcal_calibfactor[ieta][iphi] = calib;
  }
  ohcal_datafile.close();
  form_CDBTree("output/ohcal_CDBTTree_" + std::to_string(min_run) + "_" + std::to_string(max_run) + ".root", "HCALOUT_calib_ADC_to_ETower", ohcal_calibfactor);

  std::ifstream ihcal_datafile("output/ihcal_calibfactor_" + std::to_string(min_run) + "_" + std::to_string(max_run) + ".txt");
  if (!ihcal_datafile.is_open()) {
    std::cerr << "Error opening file for ihcal_calibfactor.txt " << std::endl;
    return;
  }
  float ihcal_calibfactor[24][64];
  while (std::getline(ihcal_datafile, line)) {
    std::istringstream iss(line);
    iss >> ieta >> iphi >> calib;
    ihcal_calibfactor[ieta][iphi] = calib;
  }
  ihcal_datafile.close();
  form_CDBTree("output/ihcal_CDBTTree_" + std::to_string(min_run) + "_" + std::to_string(max_run) + ".root", "HCALIN_calib_ADC_to_ETower", ihcal_calibfactor);
}
