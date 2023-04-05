#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4allraw/Fun4AllPrdfInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>

// #include <calotowerbuilder/CaloTowerBuilder.h>
#include <ledtowerbuilder/LEDTowerBuilder.h>
// #include <runtowerinfo/RunTowerInfo.h>
// #include <caloana/CaloAna.h>

#include <fun4all/Fun4AllDstOutputManager.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libledtowerbuilder.so)

#endif
void Fun4All_LEDTowerBuilder(const int events = 10, const string &fListname = "files/fileList.list", const string &outfile = "data/LEDTowerBuilder.root")

{
  gSystem->Load("libg4dst");
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);


  LEDTowerBuilder *ca = new LEDTowerBuilder(outfile.c_str());
  ca->set_detector_type(LEDTowerBuilder::CEMC);
  se->registerSubsystem(ca);

  //CaloAna *caloana = new CaloAna("CALOANA",outfile);
  //caloana->Detector("HCALOUT");
  //se->registerSubsystem(caloana);


  Fun4AllInputManager *in = new Fun4AllPrdfInputManager("in");
  in->AddListFile(fListname.c_str());
  se->registerInputManager(in);

  se->run(events);
  se->End();
  se->PrintTimer();
  std::cout << "All done!" << std::endl;
    
  gSystem->Exit(0);
}
