#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <proto4showercalib/Proto4ShowerCalib.h>
R__LOAD_LIBRARY(/direct/phenix+u/xusun/WorkSpace/sPHENIX/install/lib/libProto4HCalShowerCalib.so)
#endif

int TestBeam_HCAL_ShowerCalibAna(const int nEvents = 10000000, const string runID = "1087") // -5 GeV meson
// int TestBeam_HCAL_ShowerCalibAna(const int nEvents = 10000000, const string runID = "0422") // -8 GeV meson
// int TestBeam_HCAL_ShowerCalibAna(const int nEvents = 10000000, const string runID = "0571") // -12 GeV meson
// int TestBeam_HCAL_ShowerCalibAna(const int nEvents = 10000000, const string runID = "0821") // 60 GeV proton
// int TestBeam_HCAL_ShowerCalibAna(const int nEvents = 10000000, const string runID = "0498") // 120 GeV proton
{
  gSystem->Load("/direct/phenix+u/xusun/WorkSpace/sPHENIX/install/lib/libProto4HCalShowerCalib.so");

  bool _is_sim = false;

  std::string outputfile;
  if(_is_sim) outputfile = Form("/sphenix/user/xusun/Simulation/ShowerCalibAna/Proto4ShowerInfoSIM_%s.root",runID.c_str());
  if(!_is_sim) outputfile = Form("/sphenix/user/xusun/TestBeam/ShowerCalibAna/Proto4ShowerInfoRAW_%s.root",runID.c_str());

  Proto4ShowerCalib* hcal_ana = new Proto4ShowerCalib(outputfile.c_str());
  hcal_ana->is_sim(_is_sim);
  hcal_ana->set_runID(runID);
  hcal_ana->InitAna();
  hcal_ana->MakeAna();
  hcal_ana->FinishAna();

  std::cout << "All done" << std::endl;

  return 0;
}
