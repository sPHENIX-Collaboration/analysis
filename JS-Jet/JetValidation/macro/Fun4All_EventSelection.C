// Include guards
#pragma once
#ifndef MACRO_FUN4ALLEVENTSELECTION_C
#define MACRO_FUN4ALLEVENTSELECTION_C

// Include necessary headers
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <phool/phool.h>
#include <fun4all/Fun4AllSyncManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>
#include <jetvalidation/EventSelection.h>

// Load required libraries
R__LOAD_LIBRARY(libg4dst.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libJetValidation.so)


// Define the Fun4All macro
  void Fun4All_EventSelection(const char *filelisttruth = "dst_truth_jet.list",
			    const char* outname = "eventselectest.root") {
  // Create Fun4All server instance
  Fun4AllServer *se = Fun4AllServer::instance();
  int verbosity = 0; // Adjust verbosity level as needed
  se->Verbosity(verbosity);

  // Register the EventSelection subsystem
  EventSelection *myEventSelection = new EventSelection("AntiKt_Truth_r04", outname); // Assuming you want to pass a double and an empty string
  se->registerSubsystem(myEventSelection);

  Fun4AllInputManager *intrue = new Fun4AllDstInputManager("DSTtruth");
  intrue->AddListFile(filelisttruth,1);
  se->registerInputManager(intrue);
  /*
  // Set up Fun4All input manager for DST data
  Fun4AllDstInputManager *inData = new Fun4AllDstInputManager("DSTData");
  inData->AddListFile(filelistdata);
  se->registerInputManager(inData);
  
  // Set up Fun4All output manager
  Fun4AllDstOutputManager *outData = new Fun4AllDstOutputManager();
  outData->outfileopen(outname);
  se->registerOutputManager(outData);
*/
  // Run the analysis
  se->run(1000);
  se->End();

  // Cleanup
  delete se;
  std::cout << "Analysis Completed" << std::endl;
}

#endif // MACRO_FUN4ALLEVENTSELECTION_C
