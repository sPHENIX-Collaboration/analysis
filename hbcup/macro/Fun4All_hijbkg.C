#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

#include <boost/algorithm/string/predicate.hpp>

#include <hbcup/AnaUPC.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libhbcup.so)
R__LOAD_LIBRARY(libg4dst.so)

#endif


void Fun4All_hijbkg(const int nEvents = 0, const std::string& input_fname = "DST_TRACKS_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000019-00000.root", const std::string& sysName = "ANAUPC_HIJBKG")
{

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);

  recoConsts *rc = recoConsts::instance();
  //rc->set_IntFlag("RUNNUMBER",62);

  std::string outfname = std::regex_replace(input_fname,regex("DST_TRACKS"),"anaupc");
  AnaUPC *anaupc = new AnaUPC("anaupc", outfname);
  anaupc->Verbosity(0);
  anaupc->analyzeTracks(true);
  //anaupc->analyzeClusters(true);
  anaupc->analyzeTruth(true);
  se->registerSubsystem(anaupc);

  // DST_TRACK
  Fun4AllInputManager *in1 = new Fun4AllDstInputManager("DSTin1");

  // DST_TRUTH
  Fun4AllInputManager *in2 = new Fun4AllDstInputManager("DSTin2");

  if (boost::algorithm::ends_with(input_fname, ".root"))
  {
    in1->AddFile( input_fname );
    std::string dst_fname2 = std::regex_replace(input_fname,regex("DST_TRACKS"),"DST_TRUTH");
    in2->AddFile( dst_fname2 );
  }
  else if (boost::algorithm::ends_with(input_fname, ".list"))
  {
    in1->AddListFile( input_fname );
    std::string list_fname2 = std::regex_replace(input_fname,regex("dst_tracks"),"dst_truth");
    in2->AddListFile( list_fname2 );
  }
  else
  {
    std::cerr << "ERROR, unknown file type " << input_fname << std::endl;
    gSystem->Exit( 0 );
  }


  se->registerInputManager( in1 );
  se->registerInputManager( in2 );
  se->run( nEvents );
  se->End();
  std::cout << "All done!" << endl;
  gSystem->Exit( 0 );
  
}
