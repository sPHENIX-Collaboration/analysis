#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <phool/recoConsts.h>
#include <fun4all/Fun4AllNoSyncDstInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>



#include <g4centrality/PHG4CentralityReco.h>

#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>
#include <ffamodules/CDBInterface.h>


#include <roofithm/RooFitHM.h>
#include <fun4all/Fun4AllDstOutputManager.h>

#include <QA.C>
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libroofithm.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libg4centrality.so)
#endif

void Fun4All_RFHM(const char *filelistcalo = "/sphenix/u/ecroft/sEPDCalibration/macro/dst_list/dst_calofitting_run2pp-00053818.list", const char *outfile_hist = "RFHM_test.root")
{




    Fun4AllServer *se = Fun4AllServer::instance();
    int verbosity = 0;

    se->Verbosity(verbosity);
    recoConsts *rc = recoConsts::instance();

    //===============
    // conditions DB flags
    //===============

    // global tag
    rc->set_StringFlag("CDB_GLOBALTAG","MDC2");
    // // 64 bit timestamp
    rc->set_uint64Flag("TIMESTAMP",6);



    Fun4AllInputManager *in2 = new Fun4AllDstInputManager("DSTcalofitting");
    in2->AddListFile(filelistcalo,1);
    se->registerInputManager(in2);


    RooFitHM *rfhm = new RooFitHM("RooFitTest");

    se->registerSubsystem(rfhm);

    se->run(1);
    se->End();


    QAHistManagerDef::saveQARootFile(outfile_hist);
    delete se;
}
