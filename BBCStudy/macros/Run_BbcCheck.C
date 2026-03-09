//
// Runs module for BBC study
//
#include <TSystem.h>
#include <Fun4AllServer.h>
#include <Fun4AllDstInputManager.h>
#include <recoConsts.h>
#include "BbcCheck.h"
//#include "get_runstr.h"

#if defined(__CLING__)
R__LOAD_LIBRARY(libmbd_io.so)
R__LOAD_LIBRARY(libcalo_io.so)
R__LOAD_LIBRARY(libcalotrigger_io.so)
R__LOAD_LIBRARY(libglobalvertex_io.so)
R__LOAD_LIBRARY(libcentrality_io.so)
R__LOAD_LIBRARY(libzdcinfo_io.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libBBCStudy.so)
#endif

void Run_BbcCheck(int nEvents = 1000,
	const char *dst_bbc_file = "DST_BBC_G4HIT_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000040-00000.root",
        const char *outFile = "outfile.root")
{
#ifdef __CINT__
  gSystem->Load("libfun4all");
  gSystem->Load("libmbd_io.so");   
  gSystem->Load("libglobalvertex_io.so");   
  cout << "CINT" << endl;
#endif

  ///////////////////////////////////////////
  //
  //- recoConsts setup
  //
  ///////////////////////////////////////////
  recoConsts *rc = recoConsts::instance();
  /*
  int run = get_runnumber(dst_bbc_file);
  cout << run << endl;
  rc->set_IntFlag("RUNNUMBER",run);
  */

  ///////////////////////////////////////////
  //
  //- Make the Server
  //
  //////////////////////////////////////////
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);

  BbcCheck *ana = new BbcCheck();
  ana->set_savefile( outFile );
  se->registerSubsystem( ana );

  ///////////////////////////////////////////
  //
  //- Analyze the Data.
  //
  //////////////////////////////////////////

  Fun4AllInputManager *dst_bbc_in = new Fun4AllDstInputManager( "BBCCHECK", "DST", "TOP");
  se->registerInputManager( dst_bbc_in );

  TString dst_bbc_fname = dst_bbc_file;
  if ( dst_bbc_fname.EndsWith(".root") )
  {
    dst_bbc_in->AddFile( dst_bbc_file );
  }
  else
  {
    //you can put a list of files as well 
    cout << "Using list file " << dst_bbc_file << endl;
    dst_bbc_in->AddListFile( dst_bbc_file );
  }

  se->run(nEvents);
  se->End();
  delete se;
}
