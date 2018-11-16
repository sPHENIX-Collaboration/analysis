int TestBeam_HCAL_TowerCalibAna(const int nEvents = 1000, const int colID = 0, const string det = "HCALIN")
{
  gSystem->Load("libProto4_HCalTowerCalib.so");

  bool _is_sim = false;

  std::string outputfile;
  if(_is_sim) outputfile = Form("/sphenix/user/xusun/software/data/cosmic/TowerCalibAna/Proto4TowerInfoSIM_%s_%d.root",det.c_str(),colID);
  if(!_is_sim) outputfile = Form("/sphenix/user/xusun/software/data/cosmic/TowerCalibAna/Proto4TowerInfoRAW_%s_%d.root",det.c_str(),colID);

  Proto4TowerCalib* hcal_ana = new Proto4TowerCalib(outputfile.c_str());
  hcal_ana->is_sim(_is_sim);
  hcal_ana->set_det(det);
  hcal_ana->set_colID(colID);
  hcal_ana->InitAna();
  hcal_ana->MakeAna();
  hcal_ana->FinishAna();

  std::cout << "All done" << std::endl;
  gSystem->Exit(0);
}
