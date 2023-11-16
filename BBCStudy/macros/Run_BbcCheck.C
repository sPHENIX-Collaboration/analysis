//
// Runs FunAll module for BBC study
//
#include <TSystem.h>
#include <Fun4AllServer.h>
#include <Fun4AllDstInputManager.h>
#include <recoConsts.h>
#include "BbcCheck.h"
#include <g4mbd/MbdDigitization.h>
#include "get_runstr.h"

/*
R__LOAD_LIBRARY(libmbd_io.so)
R__LOAD_LIBRARY(libglobalvertex_io.so)
*/
R__LOAD_LIBRARY(libBBCStudy.so)

void Run_BbcCheck(int nEvents = 1000,
	const char *dst_bbc_file = "DST_BBC_G4HIT_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000040-00000.root",
        const char *outFile = "outfile.root")
{
  gSystem->Load("libfun4all");
  gSystem->Load("libphg4hit.so");
  gSystem->Load("libg4detectors.so");
  gSystem->Load("libg4decayer.so");
  gSystem->Load("libg4eval.so");   
  gSystem->Load("libg4mbd.so");   
  gSystem->Load("libg4mbd_io.so");   
  gSystem->Load("libmbd_io.so");   
  gSystem->Load("libglobalvertex_io.so");   

  ///////////////////////////////////////////
  //
  //- recoConsts setup
  //
  ///////////////////////////////////////////
  recoConsts *rc = recoConsts::instance();
  int run = get_runnumber(dst_bbc_file);
  cout << run << endl;
  rc->set_IntFlag("RUNNUMBER",run);

  ///////////////////////////////////////////
  //
  //- Make the Server
  //
  //////////////////////////////////////////
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);

  BbcCheck *ana = new BbcCheck();
  //ana->set_flag( BbcCheck::TRUTH, true);
  //ana->set_flag( BbcCheck::HIST, true);
  //ana->set_flag( BbcCheck::SF, true);
  //ana->set_flag( BbcCheck::ALL, true);
  ana->set_savefile( outFile );
  se->registerSubsystem( ana );

  ///////////////////////////////////////////
  //
  //- Analyze the Data.
  //
  //////////////////////////////////////////

  Fun4AllInputManager *dst_bbc_in = new Fun4AllDstInputManager( "BBCCHECK", "DST", "TOP");
  se->registerInputManager( dst_bbc_in );
  se->fileopen( dst_bbc_in->Name(), dst_bbc_file );
  //you can put a list of files as well 
  //dstin->AddListFile( input_file );   

  se->run(nEvents);
  se->End();
  delete se;
}
