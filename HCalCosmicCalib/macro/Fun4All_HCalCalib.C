#ifndef FUN4ALL_HCALCALIB_C
#define FUN4ALL_HCALCALIB_C

#include <caloreco/CaloTowerBuilder.h>
#include <caloreco/CaloWaveformProcessing.h>
#include <caloreco/CaloGeomMapping.h>

#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>
#include <ffamodules/CDBInterface.h>

#include <fun4allraw/Fun4AllPrdfInputManager.h>

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

void Fun4All_HCalCalib(int nevents = 0, const std::string &fname = "/sphenix/user/hanpuj/HCalCosmicCalib/prdf_file/cosmics-00028508-0000.prdf") {
    Fun4AllServer *se = Fun4AllServer::instance();
    se->Verbosity(0);
    recoConsts *rc = recoConsts::instance();

    //===============
    // conditions DB flags
    //===============
    rc->set_StringFlag("CDB_GLOBALTAG", "MDC2");
    rc->set_uint64Flag("TIMESTAMP", 6);

    Fun4AllInputManager *in = new Fun4AllPrdfInputManager("in");
    in->fileopen(fname);
    se->registerInputManager(in);

    CaloTowerBuilder *ca2 = new CaloTowerBuilder();
    ca2->set_detector_type(CaloTowerDefs::HCALIN);
    ca2->set_nsamples(12);
    ca2->set_processing_type(CaloWaveformProcessing::TEMPLATE);//TEMPLATE
    ca2->set_builder_type(CaloTowerDefs::kWaveformTowerv2);
    ca2->set_outputNodePrefix("TOWERSV2_");
    ca2->set_softwarezerosuppression(true, 200);
    se->registerSubsystem(ca2);

    CaloTowerBuilder *ca3 = new CaloTowerBuilder();
    ca3->set_detector_type(CaloTowerDefs::HCALOUT);
    ca3->set_nsamples(12);
    ca3->set_processing_type(CaloWaveformProcessing::TEMPLATE);//TEMPLATE
    ca3->set_builder_type(CaloTowerDefs::kWaveformTowerv2);
    ca3->set_outputNodePrefix("TOWERSV2_");
    ca3->set_softwarezerosuppression(true, 200);
    se->registerSubsystem(ca3);

    HCalCalibTree *wt2 = new HCalCalibTree("HCalCalib_TREE_2","ihcal_hist.root", "TOWERSV2_");
    wt2->Detector("HCALIN");
    se->registerSubsystem(wt2);

    HCalCalibTree *wt3 = new HCalCalibTree("HCalCalib_TREE_3","ohcal_hist.root", "TOWERSV2_");
    wt3->Detector("HCALOUT");
    se->registerSubsystem(wt3);

    se->run(nevents);
    CDBInterface::instance()->Print(); // print used DB files
    se->End();
    se->PrintTimer();
    gSystem->Exit(0);
}

#endif
