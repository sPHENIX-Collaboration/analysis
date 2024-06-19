//
// Runs module for MBD Laser
//
#include <TSystem.h>
#include <Fun4AllServer.h>
#include <Fun4AllDstInputManager.h>
#include <recoConsts.h>
#include "MbdLaser.h"

#if defined(__CLING__)
R__LOAD_LIBRARY(libmbd_io.so)
R__LOAD_LIBRARY(libcalo_io.so)
R__LOAD_LIBRARY(libcalotrigger_io.so)
R__LOAD_LIBRARY(libglobalvertex_io.so)
R__LOAD_LIBRARY(libcentrality_io.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libMBDStudy.so)
#endif

void Run_MbdLaser(const char *dst_mbd_file = "DST_UNCALMBD-00040000-0000.root", const char *outFile = "LASER-00040000-0000.root", int nEvents = 0)
{
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

  MbdLaser *ana = new MbdLaser();
  ana->set_savefile( outFile );
  se->registerSubsystem( ana );

  ///////////////////////////////////////////
  //
  //- Analyze the Data.
  //
  //////////////////////////////////////////

  Fun4AllInputManager *dst_mbd_in = new Fun4AllDstInputManager( "MBDLASER", "DST", "TOP");
  se->registerInputManager( dst_mbd_in );

  TString dst_mbd_fname = dst_mbd_file;
  if ( dst_mbd_fname.EndsWith(".root") )
  {
    dst_mbd_in->AddFile( dst_mbd_file );
  }
  else if ( dst_mbd_fname.EndsWith(".list") )
  {
    //you can put a list of files as well 
    cout << "Using list file " << dst_mbd_file << endl;
    dst_mbd_in->AddListFile( dst_mbd_file );
  }

  se->run(nEvents);
  se->End();
  delete se;
}
