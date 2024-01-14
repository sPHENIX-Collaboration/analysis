#ifndef FUN4ALL_HCALCALIB_C
#define FUN4ALL_HCALCALIB_C

#include <caloreco/CaloTowerBuilder.h>
#include <caloreco/CaloWaveformProcessing.h>
#include <caloreco/CaloGeomMapping.h>

#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>
#include <ffamodules/CDBInterface.h>

#include <fun4all/Fun4AllDstInputManager.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/SubsysReco.h>

#include <phool/recoConsts.h>

#include <cosmictree/HCalCalibTree.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libHCalCalibTree.so)

void Fun4All_HCalCalib_sim(int nevents = 500, const char *filelist = "dst_calo_cluster.list") {
    Fun4AllServer *se = Fun4AllServer::instance();
    se->Verbosity(0);
    recoConsts *rc = recoConsts::instance();

    //===============
    // conditions DB flags
    //===============
    rc->set_StringFlag("CDB_GLOBALTAG", "MDC2");
    rc->set_uint64Flag("TIMESTAMP", 6);

    Fun4AllInputManager *in = new Fun4AllDstInputManager("in");
    in->AddListFile(filelist,1);
    se->registerInputManager(in);

    HCalCalibTree *wt2 = new HCalCalibTree("HCalCalib_TREE_2","ihcal_hist_sim.root", "TOWERINFO_SIM_");
    wt2->Detector("HCALIN");
    se->registerSubsystem(wt2);

    HCalCalibTree *wt3 = new HCalCalibTree("HCalCalib_TREE_3","ohcal_hist_sim.root", "TOWERINFO_SIM_");
    wt3->Detector("HCALOUT");
    se->registerSubsystem(wt3);

    se->run(nevents);
    CDBInterface::instance()->Print(); // print used DB files
    se->End();
    se->PrintTimer();
    gSystem->Exit(0);
}

#endif