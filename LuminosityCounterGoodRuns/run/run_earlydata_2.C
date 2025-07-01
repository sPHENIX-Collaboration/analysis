//#ifndef FUN4ALL_YEAR2_C
//#define FUN4ALL_YEAR2_C

#include <Calo_Calib.C>
#include <QA.C>

#include <mbd/MbdReco.h>

#include <epd/EpdReco.h>

#include <zdcinfo/ZdcReco.h>

#include <globalvertex/GlobalVertexReco.h>

#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>

#include <fun4allraw/Fun4AllPrdfInputManager.h>

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllUtils.h>
#include <fun4all/SubsysReco.h>

#include <phool/recoConsts.h>

#include <calotrigger/MinimumBiasClassifier.h>
#include <centrality/CentralityReco.h>

#include <calotreegen/caloTreeGen.h>
#include <calovalid/CaloValid.h>
#include <globalqa/GlobalQA.h>

#include <triggercountmodule/triggercountmodule.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libcalotrigger.so)
R__LOAD_LIBRARY(libcentrality.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libmbd.so)
R__LOAD_LIBRARY(libepd.so)
R__LOAD_LIBRARY(libzdcinfo.so)
R__LOAD_LIBRARY(libglobalvertex.so)
R__LOAD_LIBRARY(libcalovalid.so)
R__LOAD_LIBRARY(libglobalQA.so)
R__LOAD_LIBRARY(libcaloTreeGen.so)
R__LOAD_LIBRARY(libtriggercountmodule.so)

void run_earlydata_2(int nproc = 0, int debug = 0, int rn = 0, int nfile = 0, string dir = ".")
{
  // towerinfov1=kPRDFTowerv1, v2=:kWaveformTowerv2, v3=kPRDFWaveform, v4=kPRDFTowerv4
  CaloTowerDefs::BuilderType buildertype = CaloTowerDefs::kPRDFTowerv4;
  string triggercountfilename = dir+"/trigout/triggercounter_5z_"+to_string(rn)+"_"+to_string(nproc)+".root";
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);

  recoConsts *rc = recoConsts::instance();

  int runnumber = rn;

  // conditions DB flags and timestamp
  rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");
  rc->set_uint64Flag("TIMESTAMP", runnumber);
  CDBInterface::instance()->Verbosity(1);

  FlagHandler *flag = new FlagHandler();
  se->registerSubsystem(flag);

  // MBD/BBC Reconstruction
  MbdReco *mbdreco = new MbdReco();
  se->registerSubsystem(mbdreco);


  // Official vertex storage
  GlobalVertexReco *gvertex = new GlobalVertexReco();
  se->registerSubsystem(gvertex);


  // Geometry
  //std::cout << "Adding Geometry file" << std::endl;
  //Fun4AllInputManager *intrue2 = new Fun4AllRunNodeInputManager("DST_GEO");
  //std::string geoLocation = CDBInterface::instance()->getUrl("calo_geo");
  //intrue2->AddFile(geoLocation);
  //se->registerInputManager(intrue2);

  /////////////////////////////////////////////////////
  // Set status of CALO towers, Calibrate towers,  Cluster
  //Process_Calo_Calib();


  Fun4AllInputManager *in_1 = new Fun4AllDstInputManager("DSTin1");
  string line1;
  line1 = "./dsts/"+to_string(rn)+"/"+to_string(rn)+"_"+to_string(nproc)+".root";
  in_1->AddFile(line1);
  se->registerInputManager( in_1 );
  
  triggercountmodule* trig = new triggercountmodule(triggercountfilename, rn, nproc, nfile, debug);
  se->registerSubsystem(trig);
  ///////////////////////////////////
  // Validation

  // caloTreeGen *caloTree = new caloTreeGen(outfile_tree);
  // //enable subsystems
  // caloTree->doEMCal(1, "TOWERINFO_CALIB_CEMC");
  // caloTree->doClusters(1, "CLUSTERINFO_CEMC");
  // caloTree->doHCals(1, "TOWERINFO_CALIB_HCALOUT", "TOWERINFO_CALIB_HCALIN");
  // caloTree->doZDC(1, "TOWERINFO_CALIB_ZDC");
  // caloTree->doTrig(1, "GL1Packet");

  // //set subsystem cuts
  // caloTree->setEMCalThresh(0.7);
  // caloTree->setClusterThresh(0.5);
  // caloTree->doClusterDetails(1);
  // caloTree->setOHCalThresh(0.5);
  // caloTree->setIHCalThresh(0.1);
  // se->registerSubsystem(caloTree);

  se->run(0);
  se->End();

  se->PrintTimer();
  delete se;
  std::cout << "All done!" << std::endl;
  gSystem->Exit(0);
}

//#endif
