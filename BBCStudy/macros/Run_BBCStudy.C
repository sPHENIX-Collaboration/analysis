//
// Runs FunAll module for BBC study
//
#include <TSystem.h>
#include <Fun4AllServer.h>
#include <Fun4AllDstInputManager.h>
#include <recoConsts.h>
#include <BBCStudy.h>
#include <g4bbc/BbcDigitization.h>

R__LOAD_LIBRARY(libg4bbc.so)
R__LOAD_LIBRARY(libBBCStudy.so)

void Run_BBCStudy(int nEvents = 1000,
	const char *dst_bbc_file = "DST_BBC_G4HIT_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000040-00000.root",
	const char *dst_truth_file = "DST_TRUTH_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000040-00000.root",
        const char *outFile = "outfile.root")
{
  gSystem->Load("libfun4all");
  gSystem->Load("libphg4hit.so");
  gSystem->Load("libg4detectors.so");
  gSystem->Load("libg4decayer.so");
  gSystem->Load("libg4eval.so");   
  gSystem->Load("libg4bbc.so");   
  gSystem->Load("libg4bbc_io.so");   

  ///////////////////////////////////////////
  //
  //- recoConsts setup
  //
  ///////////////////////////////////////////
  recoConsts *rc = recoConsts::instance();

  ///////////////////////////////////////////
  //
  //- Make the Server
  //
  //////////////////////////////////////////
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);

  BbcDigitization *bbcg4reco = new BbcDigitization();
  se->registerSubsystem( bbcg4reco );

  BBCStudy *ana = new BBCStudy();
  //ana->set_flag( BBCStudy::TRUTH, true);
  //ana->set_flag( BBCStudy::HIST, true);
  //ana->set_flag( BBCStudy::SF, true);
  //ana->set_flag( BBCStudy::ALL, true);
  ana->set_savefile( outFile );
  se->registerSubsystem( ana );

  ///////////////////////////////////////////
  //
  //- Analyze the Data.
  //
  //////////////////////////////////////////

  Fun4AllInputManager *dst_bbc_in = new Fun4AllDstInputManager( "BBCStudyBBC", "DST", "TOP");
  se->registerInputManager( dst_bbc_in );
  se->fileopen( dst_bbc_in->Name(), dst_bbc_file );
  //you can put a list of files as well 
  //dstin->AddListFile( input_file );   
  Fun4AllInputManager *dst_truth_in = new Fun4AllDstInputManager( "BBCStudyTruth", "DST", "TOP");
  se->registerInputManager( dst_truth_in );
  se->fileopen( dst_truth_in->Name(), dst_truth_file );

  se->run(nEvents);
  se->End();
  delete se;
}
