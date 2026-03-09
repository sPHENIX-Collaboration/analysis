#include <MBDinfo.h>
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>

#include <phool/recoConsts.h>

#include <eventplaneinfo/Eventplaneinfo.h>
#include <eventplaneinfo/EventPlaneReco.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libeventplaneinfo.so)
R__LOAD_LIBRARY(libMBDinfo.so)

void Fun4All_EP1(const int nevents = 20, const string &inputcaloList = "dst_calo.list")
{
  
  gSystem->Load("libg4dst.so");
  Fun4AllServer *se = Fun4AllServer::instance();
   
  EventPlaneReco *epreco = new EventPlaneReco();
  epreco->set_mbd_epreco(true);
  epreco->set_MBD_Min_Qcut(0.0); 
  epreco->set_z_vertex_cut(60.0); 
  se->registerSubsystem(epreco);
  
  gSystem->Load("libMBDinfo.so.0.0.0");
  MBDinfo *epana = new MBDinfo();
  epana->set_sim(false);
  se->registerSubsystem(epana);

  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTcalo");
  in->AddListFile(inputcaloList,1);
  se->registerInputManager(in);
 
  
  se->run(nevents);
  se->End();
  gSystem->Exit(0);
 
}
