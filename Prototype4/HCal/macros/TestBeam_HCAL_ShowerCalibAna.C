int TestBeam_HCAL_ShowerCalibAna(const int nEvents = 1000000)
{
  gSystem->Load("libProto4_HCalShowerCalib.so");

  bool _is_sim = false;

  std::string outputfile;
  if(_is_sim) outputfile = Form("/sphenix/user/xusun/software/data/beam/ShowerCalibAna/Proto4ShowerInfoSIM.root");
  if(!_is_sim) outputfile = Form("/sphenix/user/xusun/software/data/beam/ShowerCalibAna/Proto4ShowerInfoRAW.root");

  Proto4ShowerCalib* hcal_ana = new Proto4ShowerCalib(outputfile.c_str());
  hcal_ana->is_sim(_is_sim);
  hcal_ana->InitAna();
  hcal_ana->MakeAna();
  hcal_ana->FinishAna();

  std::cout << "All done" << std::endl;
  gSystem->Exit(0);
}
