// Include guards
#pragma once
#ifndef MACRO_FUN4ALLEVENTSELECTION_C
#define MACRO_FUN4ALLEVENTSELECTION_C

// Include necessary headers
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllSyncManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>
#include <caloembedding/caloTowerEmbed.h>
#include <jetvalidation/EventSelection.h>

// Load required libraries
R__LOAD_LIBRARY(libg4dst.so)
R__LOAD_LIBRARY(libfun4all.so)

// Define the Fun4All macro
  void Fun4All_EventSelection(
			      //			      const int nEvents = 1000,
			      const char* filelistdata = "dst_data.list",
			      const char* outputFile = "eventselectest.root"
			      ) {
  // Create Fun4All server instance
  Fun4AllServer *se = Fun4AllServer::instance();
  int verbosity = 0; // Adjust verbosity level as needed
  se->Verbosity(verbosity);

  // Initialize constants and configurations
  recoConsts *rc = recoConsts::instance();
  rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2023");
  rc->set_uint64Flag("TIMESTAMP", 23745);

  // Register the EventSelection subsystem
  EventSelection *myEventSelection = new EventSelection();
  se->registerSubsystem(myEventSelection);

  // Set up Fun4All input manager for DST data
  Fun4AllInputManager *inData = new Fun4AllDstInputManager("DSTData", "DST");
  inData->AddListFile(filelistdata);
  se->registerInputManager(inData);

  // Run the analysis
  se->run(1000);
  se->End();

  // Cleanup
  delete se;
  std::cout << "Analysis Completed" << std::endl;
}

#endif // MACRO_FUN4ALLEVENTSELECTION_C
