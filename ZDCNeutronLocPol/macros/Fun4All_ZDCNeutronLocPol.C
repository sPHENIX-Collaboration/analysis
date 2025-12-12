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

////// fname here based on the DST from Run24 production. Need to be changed based on user's need. Make sure the ana build version of the production
////// seb20 includes the ZDC/SMD and GL1 packet information. different seb is not intended to use for ZDCNeutronLocPol analysis.
void Fun4All_ZDCNeutronLocPol(const std::string &fname = "DST_TRIGGERED_EVENT_seb20_run2pp_ana502_nocdbtag_v001-00047287-00000.root", const std::string &outname = "zdcneutronlocpol_47287_0000.root", int startEvent = 0, int nEvents = 100000)
{

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);


  Fun4AllInputManager *in1 = new Fun4AllDstInputManager("in1");
  in1->AddFile(fname);
  se->registerInputManager(in1);
  
  ZDCNeutronLocPol *zdcneutronlocpol = new ZDCNeutronLocPol();
  zdcneutronlocpol->setFileName(outname);
  zdcneutronlocpol->setGainMatch("/sphenix/tg/tg01/coldqcd/jaein213/analysis/ZDCNeutronLocPol/SMDrelativegains.dat");
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
