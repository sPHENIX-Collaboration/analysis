#ifndef FUN4ALL_ZDCNEUTRONLOCPOL_C
#define FUN4ALL_ZDCNEUTRONLOCPOL_C

#include <zdcneutronlocpol/ZDCNeutronLocPol.h>

#include <fun4allraw/Fun4AllPrdfInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllUtils.h>
#include <fun4all/SubsysReco.h>

#include <phool/recoConsts.h>

#include <iostream>
#include <string>
#include <cstdio> 

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libZDCNeutronLocPol.so)


void Fun4All_ZDCNeutronLocPol(const std::string &fname = "DST_TRIGGERED_EVENT_run2pp_new_2024p001-00042797-0000.root", const std::string &outname = "zdcneutronlocpol_42797_0000.root", int startEvent = 0, int nEvents = 10000)

{

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);


  Fun4AllInputManager *in1 = new Fun4AllDstInputManager("in1");
  in1->AddFile(fname);
  se->registerInputManager(in1);
  
  ZDCNeutronLocPol *zdcneutronlocpol = new ZDCNeutronLocPol();
  zdcneutronlocpol->setFileName(outname);
  zdcneutronlocpol->setGainMatch("/sphenix/user/dloomis/analysis/ZDCNeutronLocPol/SMDrelativegains.dat");
  se->registerSubsystem(zdcneutronlocpol);

  if (startEvent > 0)
  {
    se->fileopen("in1",fname);
    se->skip(startEvent);
  }


  se->run(nEvents);  
  se->End();
  se->PrintTimer();
  delete se;
  std::cout << "All done!" << std::endl;
  gSystem->Exit(0);




}

#endif
