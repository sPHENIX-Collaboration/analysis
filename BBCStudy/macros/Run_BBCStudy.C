//
// Runs FunAll module for BBC study
//
#include <TSystem.h>
#include <Fun4AllServer.h>
#include <Fun4AllDstInputManager.h>
#include <recoConsts.h>
#include <BBCStudy.h>

R__LOAD_LIBRARY(libBBCStudy.so)

void Run_BBCStudy(int nEvents = 1000,
	const char *input_file = "/gpfs02/phenix/prod/sPHENIX/sunyrefnewinnerhcal/fieldmap/G4Hits_sPHENIX_mu-_eta0.30_40GeV.root",
        const char *outFile = "outfile.root")
{
  gSystem->Load("libfun4all");
  gSystem->Load("libphg4hit.so");
  gSystem->Load("libg4detectors.so");
  gSystem->Load("libg4decayer.so");
  gSystem->Load("libg4eval.so");   

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

  Fun4AllInputManager *dstin = new Fun4AllDstInputManager( "BBCStudy", "DST", "TOP");
  se->registerInputManager( dstin );
  se->fileopen( dstin->Name(), input_file );
  //you can put a list of files as well 
  //dstin->AddListFile( input_file );   

  se->run(nEvents);
  se->End();
  delete se;
}
