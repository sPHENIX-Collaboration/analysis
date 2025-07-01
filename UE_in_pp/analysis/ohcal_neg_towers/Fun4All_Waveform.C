#ifndef FUN4ALL_WAVEFORM_C
#define FUN4ALL_WAVEFORM_C

#include <caloreco/CaloTowerBuilder.h>
#include <caloreco/CaloWaveformProcessing.h>
#include <caloreco/CaloGeomMapping.h>
#include <caloreco/CaloTowerDefs.h>

#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>
#include <ffamodules/CDBInterface.h>

#include <fun4allraw/Fun4AllPrdfInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/SubsysReco.h>
#include <ffamodules/CDBInterface.h>
#include <fun4all/Fun4AllUtils.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>

#include <phool/recoConsts.h>
#include <phool/PHRandomSeed.h>

#include <cosmictree/WaveformTree.h>

#include <Calo_Calib.C>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libWaveformTree.so)
R__LOAD_LIBRARY(libg4dst.so)

void Fun4All_Waveform(int nevents = 100000, 
    const std::string &fname1 = "dst_triggered_event_run2pp-00049219.list", 
    const std::string &fname2 = "dst_calofitting_run2pp-00049219.list", 
    const std::string &fname3 = "dst_jet_run2pp-00049219.list", 
    const std::string &fname4 = "dst_jetcalo_run2pp-00049219.list", 
    const std::string &htfile = "neg_ohcal_towers_49219_0_httree.root", 
    const std::string &outfile = "hot_tower_waveforms_ohcal_49219_0.root")
{
    Fun4AllServer *se = Fun4AllServer::instance();
    se->Verbosity(0);
    recoConsts *rc = recoConsts::instance();

    //===============
    // conditions DB flags
    //===============
    rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");
    rc->set_uint64Flag("TIMESTAMP", 49219);

    Fun4AllInputManager *in1 = new Fun4AllDstInputManager("DSTtriggered");
    in1->AddListFile(fname1,1);
    se->registerInputManager(in1);

    Fun4AllInputManager *in2 = new Fun4AllDstInputManager("DSTcalofitting");
    in2->AddListFile(fname2,1);
    //se->registerInputManager(in2);

    Fun4AllInputManager *in3 = new Fun4AllDstInputManager("DSTjet");
    in3->AddListFile(fname3,1);
    se->registerInputManager(in3);

    Fun4AllInputManager *in4 = new Fun4AllDstInputManager("DSTjetcalo");
    in4->AddListFile(fname4,1);
    se->registerInputManager(in4);

    Fun4AllInputManager *intrue2 = new Fun4AllRunNodeInputManager("DST_GEO");
    std::string geoLocation = CDBInterface::instance()->getUrl("calo_geo");
    intrue2->AddFile(geoLocation);
    se->registerInputManager(intrue2);

    CaloTowerBuilder *ca2 = new CaloTowerBuilder();
    ca2->set_detector_type(CaloTowerDefs::HCALOUT);
    ca2->set_processing_type(CaloWaveformProcessing::TEMPLATE);
    ca2->set_builder_type(CaloTowerDefs::kPRDFWaveform);
    ca2->set_offlineflag(true);
    ca2->set_nsamples(12);
    ca2->set_outputNodePrefix("WAVEFORMS_");
    se->registerSubsystem(ca2);

    Process_Calo_Calib();

    WaveformTree *wt = new WaveformTree("WaveformTree",htfile, outfile);
    wt->Detector("HCALOUT");
    se->registerSubsystem(wt);

    //Fun4AllDstOutputManager *out = new Fun4AllDstOutputManager("DSTOUT", "outtest.root");
    //se->registerOutputManager(out);

    se->run(nevents);
    CDBInterface::instance()->Print(); // print used DB files
    se->End();
    se->PrintTimer();
    gSystem->Exit(0);
}

#endif
