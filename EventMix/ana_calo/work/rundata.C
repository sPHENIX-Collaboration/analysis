#include <GlobalVariables.C>

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4allraw/Fun4AllPrdfInputManager.h>

#include <caloreco/CaloTowerBuilder.h>

#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>

#include <phool/recoConsts.h>

#include </gpfs/mnt/gpfs02/sphenix/user/lebedev/mdc/analysis/EventMix/ana_calo/install/include/sphanalysis_calo/sPHAnalysis_calo.h>

R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libsphanalysis_calo.so)

void rundata()
{

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);

  recoConsts *rc = recoConsts::instance();

  SyncReco *sync = new SyncReco();
  se->registerSubsystem(sync);

  HeadReco *head = new HeadReco();
  se->registerSubsystem(head);

  FlagHandler *flag = new FlagHandler();
  se->registerSubsystem(flag);

  Enable::CDB = true;
  rc->set_StringFlag("CDB_GLOBALTAG",CDB::global_tag);
  rc->set_uint64Flag("TIMESTAMP",CDB::timestamp);

  CaloTowerBuilder *calo = new CaloTowerBuilder();
  calo->set_detector_type(CaloTowerBuilder::CEMC);
  calo->set_nsamples(31);
  calo->set_processing_type(CaloWaveformProcessing::TEMPLATE);
  se->registerSubsystem(calo);

  sPHAnalysis_calo *ana = new sPHAnalysis_calo("sPHAnalysis","test.root");
  ana->set_whattodo(1);
  se->registerSubsystem(ana);

  Fun4AllInputManager *in = new Fun4AllPrdfInputManager("in");
  se->registerInputManager(in);
  //in->AddListFile("pythiajpsi_list9.txt");
  //in->AddFile("/sphenix/lustre01/sphnxpro/commissioning/emcal/calib/calib_seb07-00007202-0000.prdf");
  in->AddFile("/gpfs/mnt/gpfs02/sphenix/user/trinn/comissioning_data_production/full-00009111-0000.prdf");

  se->run(10);
  se->End();
}

