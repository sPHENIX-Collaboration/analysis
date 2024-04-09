#ifndef FUN4ALL_WAVEFIT_C
#define FUN4ALL_WAVEFIT_C

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
#include "/sphenix/u/bseidlitz/work/waveform_cemc_gen_template/source/BeamWaveform.h"

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libBeamWaveform.so)

void Fun4All_WaveFit(int nevents = 10, const std::string &fname = "/sphenix/lustre01/sphnxpro/commissioning/aligned_prdf/beam-00021796-0076.prdf")
{
    Fun4AllServer *se = Fun4AllServer::instance();
    se->Verbosity(0);
    recoConsts *rc = recoConsts::instance();

    //===============
    // conditions DB flags
    //===============
    rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2023");
    rc->set_uint64Flag("TIMESTAMP", 0);
    
    Fun4AllInputManager *in = new Fun4AllPrdfInputManager("in");
    in->fileopen(fname);
    se->registerInputManager(in);
 
    BeamWaveform *bw = new BeamWaveform("BeamWaveform");
    bw->SetDetector("CEMC");
    bw->SetPacketRange(6001, 6128);
//    bw->SetPacketRange(8003,8006);
    se->registerSubsystem(bw);

    se->run(nevents);
    se->End();
    se->PrintTimer();
    gSystem->Exit(0);
}
#endif
