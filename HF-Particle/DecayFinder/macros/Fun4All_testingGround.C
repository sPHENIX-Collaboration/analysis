// $Id: $

/*!
 * \file Fun4All_KFParticle_advanced.C
 * \brief 
 * \author Cameron Dean <cdean@bnl.gov>
 * \version $Revision:   $
 * \date $Date: $
 */

#if ROOT_VERSION_CODE >= ROOT_VERSION(6, 00, 0)

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/SubsysReco.h>
#include <decayfinder/DecayFinder.h>

R__LOAD_LIBRARY(libdecayfinder.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4dst.so)

#endif

using namespace std;

int Fun4All_testingGround(){

  const int nEvents = 1;
  int verbosity = 1;
  //---------------
  // Load libraries
  //---------------
  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4dst.so");
  //---------------
  // Fun4All server
  //---------------
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(verbosity);

  //--------------
  // IO management
  //--------------
  // Hits file
  Fun4AllInputManager *hitsin = new Fun4AllDstInputManager("DSTin");

  hitsin->AddFile("DST_HF_BOTTOM_pythia8-0000000001-00000.root");
  se->registerInputManager(hitsin);

  DecayFinder* myFinder = new DecayFinder("myTestFinder");
  myFinder->Verbosity(verbosity);
  myFinder->setDecayDescriptor("[Bs0 -> K^+ Kaon^-]cc");
  se->registerSubsystem(myFinder);

  DecayFinder* myFinder2 = new DecayFinder("myTestFinder2");
  myFinder2->Verbosity(verbosity);
  myFinder2->setDecayDescriptor("[Bs0 -> (J/psi -> muon^+ mu^-) (phi -> K^+ K^-)]cc");
  se->registerSubsystem(myFinder2);

  DecayFinder* myFinder3 = new DecayFinder("myTestFinder3");
  myFinder3->Verbosity(verbosity);
  myFinder3->setDecayDescriptor("B- -> (D0->Kaon^+pion^-)pi^-");
  se->registerSubsystem(myFinder3);

  DecayFinder* myFinder4 = new DecayFinder("myTestFinder4");
  myFinder4->Verbosity(verbosity);
  myFinder4->setDecayDescriptor("[B- -> proton^+ ( D0 -> K^+ pi^- ) ( D- -> K^+ pi^- pi^-) pi^-]CC");
  se->registerSubsystem(myFinder4);

  //-----------------
  // Event processing
  //-----------------
  if (nEvents < 0)
    return 0;

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
