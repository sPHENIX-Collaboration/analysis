#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4allraw/Fun4AllPrdfInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <phool/recoConsts.h>


#include <caloreco/CaloTowerBuilder.h>

#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>
#include <ffamodules/CDBInterface.h>

#include <templatecreation/TemplateCreation.h>

#include <fun4all/Fun4AllDstOutputManager.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libTemplateCreation.so)

#endif

void Fun4All_Templates(const char *fname = "full-00005521-0000.prdf")
{
  gSystem->Load("libg4dst");
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);
  recoConsts *rc = recoConsts::instance();

 //===============
  // conditions DB flags
  //===============
  rc->set_StringFlag("CDB_GLOBALTAG","MDC2");
  rc->set_uint64Flag("TIMESTAMP",6);



   TemplateCreation *templatecreation = new TemplateCreation();
   se->registerSubsystem(templatecreation);

  Fun4AllInputManager *in = new Fun4AllPrdfInputManager("in");
  in->fileopen(fname);
  se->registerInputManager(in);




  se->skip(10);
  se->run(0);
  se->End();
  se->PrintTimer();
  gSystem->Exit(0);
}
