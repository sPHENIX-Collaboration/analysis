#include <GlobalVariables.C>
#include <Trkr_Clustering.C> 
#include <Trkr_RecoInit.C>  //ACT


#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <GlobalVariables.C>  //Enable:INTT
#include <phool/recoConsts.h>
#include <dst_read.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libinttread.so)
R__LOAD_LIBRARY(libdst_read.so)
//void DST_READ(const string &input_file = "/sphenix/tg/tg01/commissioning/INTT/data/dst_files/2024/DST_beam_intt-00041661_no_hot_clustering_z_special_10k.root",int nEvents = 10000)
void DST_READ(const string &input_file = "/sphenix/tg/tg01/commissioning/INTT/data/dst_files/2024/DST_beam_intt-00041651_no_hot_clustering_z_special.root",int nEvents = 0 )
{

  gSystem->ListLibraries();
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);
  recoConsts *rc = recoConsts::instance();

  Enable::INTT = true;

  Enable::CDB = true;
  rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2023");
  rc->set_uint64Flag("TIMESTAMP", 6);
  std::string geofile = CDBInterface::instance()->getUrl("Tracking_Geometry");


  ACTSGEOM::ActsGeomInit();
	
  Fun4AllDstInputManager *in = new Fun4AllDstInputManager("Dst");
  in->fileopen(input_file.c_str());
  se->registerInputManager(in);

 // InttAna *inttana = new InttAna();
//  se->registerSubsystem(inttana);
 
  dst_read *read = new dst_read("dst_read","data/test602_v17_41651_all.root");
  se->registerSubsystem(read);

  
  se->run(nEvents);

  se->End();
  delete se;
  gSystem->Exit(0);
  return 0;
}
 
	
