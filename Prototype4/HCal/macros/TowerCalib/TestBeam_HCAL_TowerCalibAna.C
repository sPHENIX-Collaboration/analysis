#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <proto4showercalib/Proto4ShowerCalib.h>
R__LOAD_LIBRARY(/direct/phenix+u/xusun/WorkSpace/sPHENIX/install/lib/libProto4HCalTowerCalib.so)
#endif

int TestBeam_HCAL_TowerCalibAna(const int nEvents = 50000, const int colID = 0, const string det = "HCALOUT")
{
  gSystem->Load("/direct/phenix+u/xusun/WorkSpace/sPHENIX/install/lib/libProto4HCalTowerCalib.so");

  bool _is_sim = false;

  std::string outputfile;
  if(_is_sim) outputfile = Form("/sphenix/user/xusun/Simulation/TowerCalibAna/Proto4TowerInfoSIM_%s_%d.root",det.c_str(),colID);
  if(!_is_sim) outputfile = Form("/sphenix/user/xusun/TestBeam/TowerCalibAna/Proto4TowerInfoRAW_%s_%d.root",det.c_str(),colID);

  Proto4TowerCalib* hcal_ana = new Proto4TowerCalib(outputfile.c_str());
  hcal_ana->is_sim(_is_sim);
  hcal_ana->set_det(det);
  hcal_ana->set_colID(colID);
  hcal_ana->InitAna();
  hcal_ana->MakeAna();
  hcal_ana->FinishAna();

  std::cout << "All done" << std::endl;

  return 0;
}
