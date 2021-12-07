// $Id: $

/*!
 * \file Fun4All_ReadDST_TPCML.C
 * \brief 
 * \author Jin Huang <jhuang@bnl.gov>
 * \version $Revision:   $
 * \date $Date: $
 */

#ifndef MACRO_FUN4ALL_READDST_C
#define MACRO_FUN4ALL_READDST_C

#include <G4_Input.C>
#include <G4_Tracking.C>

#include <tpcmldatainterface/TPCMLDataInterface.h>

#include <TROOT.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/recoConsts.h>

R__LOAD_LIBRARY(libfun4all.so)
// Use libg4eicdst for campaign 2 DSTs
R__LOAD_LIBRARY(libg4dst.so)
// Use libg4dst for campaign 1 DSTs
//R__LOAD_LIBRARY(libg4dst.so)
R__LOAD_LIBRARY(libtpcmldatainterface.so)

int Fun4All_ReadDST_TPCML(const int nEvents = 5,
                          //                          const string &inputFile = "singleElectron.lst"  //
                          //        const string& inputFile = "singlePion.lst"//
                          const string &inputFile = "singlePionNeg.lst"  //
)

{
  //---------------
  // Fun4All server
  //---------------
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);

  // just if we set some flags somewhere in this macro
  recoConsts *rc = recoConsts::instance();

  Input::READHITS = true;
  INPUTREADHITS::filename[0] = "DST_TRUTH_pythia8_CharmD0_3MHz-0000000002-00000.root";
  INPUTREADHITS::filename[1] = "DST_TRKR_G4HIT_pythia8_CharmD0_3MHz-0000000002-00000.root";
  INPUTREADHITS::filename[2] = "DST_TRKR_HIT_pythia8_CharmD0_3MHz-0000000002-00000.root";
//  INPUTREADHITS::filename[3] = "DST_TRUTH_pythia8_CharmD0_3MHz-0000000002-00000.root";

  TPCMLDataInterface *tpcDaqEmu = new TPCMLDataInterface(
      G4MVTX::n_maps_layer + G4INTT::n_intt_layer, G4MVTX::n_maps_layer + G4INTT::n_intt_layer + G4TPC::n_gas_layer - 1);
  tpcDaqEmu->Verbosity(1);
  tpcDaqEmu->setVertexZAcceptanceCut(200);
  se->registerSubsystem(tpcDaqEmu);

  InputManagers();

  se->run(nEvents);

  se->End();

  delete se;
  std::cout << "All done processing" << std::endl;
  gSystem->Exit(0);
  return 0;
}
#endif
