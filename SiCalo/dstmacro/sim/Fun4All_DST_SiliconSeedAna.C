#ifndef MACRO_FUN4ALLG4SPHENIX_C
#define MACRO_FUN4ALLG4SPHENIX_C

#include <sys/stat.h>
#include <limits.h>     // PATH_MAX
#include <unistd.h>


#include <GlobalVariables.C>
#include <G4_Input.C> // global variable "Input"
#include <G4_ActsGeom.C> // global variable "ACTSGEOM"

//--#include <Trkr_QA.C> // Simulation QA


#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>
#include <ffamodules/CDBInterface.h>

#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>

#include <phool/recoConsts.h>


#include <siliconseedsana/SiliconSeedsAna.h>
#pragma GCC diagnostic push

#pragma GCC diagnostic ignored "-Wundefined-internal"

#pragma GCC diagnostic pop

R__LOAD_LIBRARY(libfun4all.so)
//R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libsiliconseedsana.so)


bool useTopologicalCluster = false;

int Fun4All_DST_SiliconSeedAna(
   const int nEvents = 100, //0,
//   const string in_dst = "/sphenix/tg/tg01/commissioning/INTT/work/mahiro/SIliconCalo/MC/dst_e-/condor/"
//                        "DST_SiliconOnly_single_e-_vtxfixed_Caloevents_reconstructedInfo_0.root",
   const string in_dst = "/sphenix/tg/tg01/commissioning/INTT/work/mahiro/SIliconCalo/MC/dst/condor/"
                         "DST_SiliconOnly_PHYTIA_Info_1.root",
//   const string out_root = "/sphenix/tg/tg01/commissioning/INTT/work/hachiya/SiCaloTrack/data/ana_em/ana_em_0.root",
   const string out_root = "ana_0.root",
   const int startnumber = 0
)
{

  std::cout << "Input DST: " << in_dst      <<std::endl;
  std::cout << "out  ROOT: " << out_root    <<std::endl;
  std::cout << "Nevent   : " << nEvents     <<std::endl;
  std::cout << "#start   : " << startnumber <<std::endl;


  Fun4AllServer *se = Fun4AllServer::instance();
  //se->Verbosity(10);


  recoConsts *rc = recoConsts::instance();
  Input::VERBOSITY = 0;

  // Tracking setup
  Enable::CDB = true;
  rc->set_StringFlag("CDB_GLOBALTAG", CDB::global_tag);
  rc->set_uint64Flag("TIMESTAMP", CDB::timestamp);


  // this assume Geometory data is in the DST
  ACTSGEOM::ActsGeomInit();


  //--input
  Fun4AllInputManager *in = new Fun4AllDstInputManager("DST");
  in->fileopen(in_dst.c_str());
  //in->Verbosity(2);
  se->registerInputManager(in);

//  Tracking_QA();
  

  auto siana = new SiliconSeedsAna("SiliconSeedsAna");
  siana->setMC(true);
  siana->setVtxSkip(true);
  siana->setTopoCluster(useTopologicalCluster);
  siana->setEMCalClusterContainerName("CLUSTERINFO_CEMC"); // for PYTHIA sim
  //siana->setOutputFileName("siana.root");
  siana->setOutputFileName(out_root.c_str());

  siana->setStartEventNumber(startnumber);

  se->registerSubsystem(siana);

  se->run(nEvents);


  //-----
  // Exit
  //-----

  //  CDBInterface::instance()->Print(); // print used DB files
  se->End();
  std::cout << "All done" << std::endl;
  delete se;

  gSystem->Exit(0);
  return 0;
}
#endif
