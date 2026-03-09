#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <calowaveformsim/GL1TriggerSelect.h>
#include <globalvertex/GlobalVertexReco.h>
#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>
#include <fun4all/Fun4AllUtils.h>
#include <centrality/CentralityReco.h>
#include <centrality/CentralityValid.h>
#include <calotrigger/MinimumBiasClassifier.h>

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <vector>

#include <phool/recoConsts.h>
#include <frog/FROG.h>
#include <calotrigger/TriggerRunInfoReco.h>

#include <caloreco/CaloTowerBuilder.h>
#include <mbd/MbdReco.h>
#include <zdcinfo/ZdcReco.h>

#include <caloreco/CaloTowerCalib.h>

R__LOAD_LIBRARY(libemulatortreemaker.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libcentrality.so)
R__LOAD_LIBRARY(libglobalvertex.so)

R__LOAD_LIBRARY(libcentralityvalid.so)
R__LOAD_LIBRARY(libzdcinfo.so)
R__LOAD_LIBRARY(libmbd.so) 
R__LOAD_LIBRARY(libzdcinfo.so) 

#endif
  
  void Fun4All_CentralityValid(const std::string filename)
{
  gSystem->Load("libFROG");
  gSystem->Load("libg4dst");
  gSystem->Load("libcentrality");
  gSystem->Load("libcentralityvalid");
  gSystem->Load("libcalotrigger");

  CaloTowerDefs::BuilderType buildertype = CaloTowerDefs::kPRDFTowerv4;

  std::pair<int, int> run_seg = Fun4AllUtils::GetRunSegment(filename);
  int runnumber = run_seg.first;
  int segment = run_seg.second;

  int nevents = 0;
  int verbosity = 0;

  const char* env_p = std::getenv("MBD_CENTRALITY_CALIB_PATH");

  if(!env_p)
    {
      std::cout << "no env MBD_CENTRALITY_CALIB_PATH set."<<endl;
      return;
    }


  const char* env_cdb = std::getenv("CDB_TAG");

  if(!env_cdb)
    {
      std::cout << "no env CDB_TAG set."<<endl;
      return;
    }

  char *dir = new char[100];

  sprintf(dir, "output/run%d/mbdana", runnumber);

  
  const char *tree_outfile = Form("%s/%s/mbd_ana_tree_%08d_%05d.root", env_p, dir, runnumber, segment);

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);

  recoConsts *rc = recoConsts::instance();
  rc->set_StringFlag("CDB_GLOBALTAG","ProdA_2024");
  rc->set_uint64Flag("TIMESTAMP",runnumber);

  FROG *fr = new FROG();

  std::string input = fr->location(filename.c_str());
  Fun4AllInputManager *in = new Fun4AllDstInputManager("in");
  in->fileopen(input.c_str());
  in->Verbosity(0);
  se->registerInputManager(in);
  // MBD/BBC Reconstruction
  MbdReco *mbdreco = new MbdReco();
  mbdreco->Verbosity(0);
  se->registerSubsystem(mbdreco);

  GlobalVertexReco* gblvertex = new GlobalVertexReco();
  gblvertex->Verbosity(0);
  se->registerSubsystem(gblvertex);

  CaloTowerBuilder *caZDC = new CaloTowerBuilder("ZDCBUILDER");
  caZDC->set_detector_type(CaloTowerDefs::ZDC);
  caZDC->set_builder_type(buildertype);
  caZDC->set_processing_type(CaloWaveformProcessing::FAST);
  caZDC->set_nsamples(16);
  caZDC->set_offlineflag();
  se->registerSubsystem(caZDC);


  std::cout << "Calibrating ZDC" << std::endl;
  CaloTowerCalib *calibZDC = new CaloTowerCalib("ZDC");
  calibZDC->set_detector_type(CaloTowerDefs::ZDC);
  se->registerSubsystem(calibZDC);  // conditions DB flags



  //ZDC Reconstruction--Calib Info
  ZdcReco *zdcreco = new ZdcReco();
  zdcreco->set_zdc1_cut(0.0);
  zdcreco->set_zdc2_cut(0.0);
  se->registerSubsystem(zdcreco);

  GL1TriggerSelect *gts = new GL1TriggerSelect("GL1TriggerSelect");
  gts->select_trigger(10);
  se->registerSubsystem(gts);

  // Fun4All

  MinimumBiasClassifier *mb = new MinimumBiasClassifier();
  mb->Verbosity(0);
  se->registerSubsystem(mb);

  CentralityReco *cr = new CentralityReco();
  cr->Verbosity(1);
  se->registerSubsystem(cr);
  

  CentralityValid *centralityvalidation = new CentralityValid("CentralityValid","cent_valid.root");
  se->registerSubsystem(centralityvalidation);

  se->run(10);
  se->End();
}
