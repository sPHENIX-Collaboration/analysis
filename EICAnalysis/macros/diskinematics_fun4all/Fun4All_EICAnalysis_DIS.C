#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDummyInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllNoSyncDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <phhepmc/Fun4AllHepMCInputManager.h>
#include <phool/recoConsts.h>
#include <eicana/DISKinematicsReco.h>
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libphhepmc.so)
R__LOAD_LIBRARY(libeicana.so)
#endif

using namespace std;

int Fun4All_EICAnalysis_DIS(
			    const int nEvents = 100,
			    const char * inputFile = "/sphenix/user/baschmoo/standard/macros/macros/g4simulations/G4EICDetector.root",
			    const char * outputFile = "output.root"
			    )
{
  
  bool readdst = true;
  bool readhepmc = false;

  //---------------
  // Load libraries
  //---------------

  gSystem->Load("libfun4all.so");
  gSystem->Load("libphhepmc.so");
  gSystem->Load("libeicana.so");

  //---------------
  // Fun4All server
  //---------------

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);
  // just if we set some flags somewhere in this macro
  recoConsts *rc = recoConsts::instance();

  //--------------
  // Analysis modules
  //--------------

  DISKinematicsReco *ana = new DISKinematicsReco(outputFile);
  ana->set_do_process_truth( true );
  ana->set_do_process_geant4_cluster( true );
  se->registerSubsystem( ana );

  //--------------
  // IO management
  //--------------

  /* Read DST Files */
  if ( readdst )
    {
      Fun4AllInputManager *hitsin = new Fun4AllDstInputManager("DSTin");
      hitsin->fileopen(inputFile);
      se->registerInputManager(hitsin);
    }
  /* Read HepMC ASCII files */
  else if ( readhepmc )
    {
      Fun4AllInputManager *in = new Fun4AllHepMCInputManager( "DSTIN");
      se->registerInputManager( in );
      se->fileopen( in->Name().c_str(), inputFile );
    }


  //-----------------
  // Event processing
  //-----------------
  if (nEvents < 0)
    {
      return 0;
    }
  // if we run the particle generator and use 0 it'll run forever
  if (nEvents == 0 && !readdst && !readhepmc)
    {
      cout << "using 0 for number of events is a bad idea when using particle generators" << endl;
      cout << "it will run forever, so I just return without running anything" << endl;
      return 0;
    }
  
  se->run(nEvents);

  //-----
  // Exit
  //-----
  
  se->End();
  std::cout << "All done" << std::endl;
  delete se;
  gSystem->Exit(0);
  return 0;
}
