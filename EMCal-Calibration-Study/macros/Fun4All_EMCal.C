#ifndef FUN4ALL_EMCAL_C
#define FUN4ALL_EMCAL_C

// c++ includes --
#include <iostream>
#include <string>
#include <fstream>

// root includes --
#include <TROOT.h>
#include <TSystem.h>
#include <TFile.h>
#include <TF1.h>

#include <caloreco/CaloTowerCalib.h>
#include <caloreco/CaloTowerStatus.h>
#include <caloreco/RawClusterBuilderTemplate.h>
#include <caloreco/RawClusterPositionCorrection.h>

#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllUtils.h>
#include <fun4all/SubsysReco.h>

#include <phool/recoConsts.h>

#include <ffamodules/CDBInterface.h>

#include <litecaloeval/LiteCaloEval.h>
#include <calib_emc_pi0/pi0EtaByEta.h>

#include <mbd/MbdReco.h>
#include <globalvertex/GlobalVertexReco.h>

using std::cout;
using std::endl;
using std::string;
using std::pair;
using std::ifstream;

R__LOAD_LIBRARY(libcdbobjects)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libLiteCaloEvalTowSlope.so)
R__LOAD_LIBRARY(libcalibCaloEmc_pi0.so)
R__LOAD_LIBRARY(libglobalvertex.so)
R__LOAD_LIBRARY(libg4mbd.so)
R__LOAD_LIBRARY(libmbd_io.so)
R__LOAD_LIBRARY(libmbd.so)

void createLocalEMCalCalibFile(const string &fname, int runNumber);

void Fun4All_EMCal(int nevents = 1e2, const string &fname = "inputdata.txt",int iter = 0, const string &calib_fname="local_calib_copy.root", const string &fieldname = "CEMC_calib_ADC_to_ETower")
{

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);
  CDBInterface::instance()->Verbosity(1);

  recoConsts *rc = recoConsts::instance();

  ifstream file(fname);
  string first_file;
  getline(file, first_file);

  string m_fieldname = fieldname;

  //===============
  // conditions DB flags
  //===============
  pair<int, int> runseg = Fun4AllUtils::GetRunSegment(first_file);
  int runnumber = runseg.first;
  cout << "run number = " << runnumber << endl;

  rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");
  rc->set_uint64Flag("TIMESTAMP",runnumber);// runnumber);

  Fun4AllInputManager *in = new Fun4AllDstInputManager("DST_TOWERS");
  in->AddListFile(fname);
  se->registerInputManager(in);

  string filename = first_file.substr(first_file.find_last_of("/\\") + 1);
  string OutFile = Form("OUTHIST_iter%d_%s",iter , filename.c_str());


  if (iter == 0)
  {
    createLocalEMCalCalibFile(calib_fname.c_str(), runnumber);
    cout << "creating " << calib_fname.c_str() << " and exiting" << endl;
    gSystem->Exit(0);
  }

  /////////////////////
  // mbd/vertex
  // MBD/BBC Reconstruction
  MbdReco *mbdreco = new MbdReco();
  se->registerSubsystem(mbdreco);

  // Official vertex storage
  GlobalVertexReco *gvertex = new GlobalVertexReco();
  se->registerSubsystem(gvertex);



  /////////////////////
  // Geometry
  std::cout << "Adding Geometry file" << std::endl;
  Fun4AllInputManager *intrue2 = new Fun4AllRunNodeInputManager("DST_GEO");
  string geoLocation = CDBInterface::instance()->getUrl("calo_geo");
  intrue2->AddFile(geoLocation);
  se->registerInputManager(intrue2);


  ////////////////////
  // Calibrate towers
  CaloTowerStatus *statusEMC = new CaloTowerStatus("CEMCSTATUS");
  statusEMC->set_detector_type(CaloTowerDefs::CEMC);
  //statusEMC->set_doAbortNoHotMap();
  statusEMC->set_directURL_hotMap("/sphenix/u/bseidlitz/work/forChris/caloStatusCDB_y2/moreMaps/EMCalHotMap_new_2024p006-48837cdb.root");
  se->registerSubsystem(statusEMC);

  CaloTowerCalib *calibEMC = new CaloTowerCalib("CEMCCALIB");
  calibEMC->set_detector_type(CaloTowerDefs::CEMC);
  calibEMC->set_directURL(calib_fname.c_str());
  calibEMC->setFieldName(fieldname);
  se->registerSubsystem(calibEMC);


  //////////////////
  // Clusters
  std::cout << "Building clusters" << std::endl;
  RawClusterBuilderTemplate *ClusterBuilder = new RawClusterBuilderTemplate("EmcRawClusterBuilderTemplate");
  ClusterBuilder->Detector("CEMC");
  ClusterBuilder->set_threshold_energy(0.07);  // for when using basic calibration
  string emc_prof = getenv("CALIBRATIONROOT");
  emc_prof += "/EmcProfile/CEMCprof_Thresh30MeV.root";
  ClusterBuilder->LoadProfile(emc_prof);
  ClusterBuilder->set_UseTowerInfo(1);  // to use towerinfo objects rather than old RawTower
  ClusterBuilder->setOutputClusterNodeName("CLUSTERINFO_CEMC");
  ClusterBuilder->set_UseAltZVertex(1);
  se->registerSubsystem(ClusterBuilder);

/*
  std::cout << "Applying Position Dependent Correction" << std::endl;
  RawClusterPositionCorrection *clusterCorrection = new RawClusterPositionCorrection("CEMC");
  clusterCorrection->set_UseTowerInfo(1);  // to use towerinfo objects rather than old RawTower
  se->registerSubsystem(clusterCorrection);
*/

  ///////////////////
  // analysis modules
  if (iter==1 || iter==2 || iter==3){
    LiteCaloEval *eval7e = new LiteCaloEval("CEMCEVALUATOR2", "CEMC", OutFile);
    eval7e->CaloType(LiteCaloEval::CEMC);
    eval7e->set_reqMinBias(false);
    eval7e->setInputTowerNodeName("TOWERINFO_CALIB_CEMC");
    se->registerSubsystem(eval7e);
  }

  if (iter>3){
    pi0EtaByEta *ca = new pi0EtaByEta("calomodulename", OutFile);
    ca->set_timing_cut_width(16); // does nothing currently
    ca->apply_vertex_cut(true); // default
    ca->set_vertex_cut(20.);
    ca->set_pt1BaseClusCut(1.3); // default
    ca->set_pt2BaseClusCut(0.7); // default
    ca->set_NclusDeptFac(1.4); // default
    ca->set_RunTowByTow(true);
    ca->set_reqTrig(false);
    ca->set_GlobalVertexType(GlobalVertex::MBD);
    ca->set_requireVertex(true);
    ca->set_calib_fieldname(m_fieldname);
    se->registerSubsystem(ca);
  }

  se->run(nevents);
  se->End();
  se->PrintTimer();
  delete se;

  std::cout << "All done!" << std::endl;
  gSystem->Exit(0);
}




void createLocalEMCalCalibFile(const string &fname, int runNumber)
{
  string default_time_independent_calib = (runNumber >= 66580) ? "CEMC_calib_ADC_to_ETower_default" : "cemc_pi0_twrSlope_v1_default";
  string m_calibName = "getdefault";

  string calibdir = CDBInterface::instance()->getUrl(m_calibName);
  string filePath;

  if (!calibdir.empty())
  {
    filePath = calibdir;
  }
  else
  {
    calibdir = CDBInterface::instance()->getUrl(default_time_independent_calib);

    if (calibdir.empty())
    {
      std::cout << "No EMCal Calibration NOT even a default" << std::endl;
      exit(1);
    }
    filePath = calibdir;
    std::cout << "No specific file for " << m_calibName << " found, using default calib " << default_time_independent_calib << std::endl;
  }

  TFile *f_cdb = new TFile(filePath.c_str());
  f_cdb->Cp(fname.c_str());
  f_cdb->Cp("initial_calib.root");

  std::cout << "created local Calib file for run " << runNumber << " named " << fname << std::endl;

  delete f_cdb;
}
#endif

#ifndef __CINT__
int main(int argc, const char* const argv[])
{
  if (argc > 6)
  {
    cout << "usage: " << argv[0] << " [events] [fname] [iter] [calib_fname] [fieldname]" << endl;
    return 1;
  }

  Int_t events = 1e2;
  string fname = "inputdata.txt";
  Int_t iter = 0;
  string calib_fname = "local_calib_copy.txt";
  string fieldname = "CEMC_calib_ADC_to_ETower";

  if (argc >= 2)
  {
    events = std::atoi(argv[1]);
  }
  if (argc >= 3)
  {
    fname = argv[2];
  }
  if (argc >= 4)
  {
    iter = std::atoi(argv[3]);
  }
  if (argc >= 5)
  {
    calib_fname = argv[4];
  }
  if (argc >= 6)
  {
    fieldname = argv[5];
  }

  Fun4All_EMCal(events, fname, iter, calib_fname, fieldname);

  cout << "done" << endl;
  return 0;
}
#endif
