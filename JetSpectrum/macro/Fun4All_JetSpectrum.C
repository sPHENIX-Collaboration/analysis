#ifndef MACRO_FUN4ALL_JETSPECTRUM_C
#define MACRO_FUN4ALL_JETSPECTRUM_C

#include <fstream>
#include <limits>
#include <iostream>

#include <fun4all/Fun4AllServer.h>
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllUtils.h>

#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>

#include <mbd/MbdReco.h>
#include <globalvertex/GlobalVertexReco.h>
#include <GlobalVariables.C>
#include <G4_Global.C>

#include <jetbase/JetReco.h>
#include <jetbase/TowerJetInput.h>
#include <jetbackground/RetowerCEMC.h>

#include <Calo_Calib.C>
#include <calotreegen/caloTreeGen.h> 

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libjetbase.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libmbd.so)
R__LOAD_LIBRARY(libglobalvertex.so)
R__LOAD_LIBRARY(libcaloTreeGen.so)
R__LOAD_LIBRARY(libffarawobjects.so)
R__LOAD_LIBRARY(libg4jets.so)

void Fun4All_JetSpectrum(
  const string &filedstlist0 = "example_list/dst_jet2.list",
  const string &filedstlist1 = "example_list/dst_jetcalo2.list",
	const string &outputFile = "output_data.root",
  int n_event = 0) {

  int verbosity = 0;

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(verbosity);
  recoConsts *rc = recoConsts::instance();

  int runnumber;
  std::ifstream file(filedstlist0);
  std::string line;
  std::getline(file, line);
  file.close();
  std::regex pattern(R"((\d+)-\d+\.root)");
  std::smatch matches;    
  if (std::regex_search(line, matches, pattern) && matches.size() > 1) {
    runnumber = std::stoi(matches[1].str());
    std::cout << "Run number: " << runnumber << std::endl;
  } else {
    std::cerr << "Run number is not clear." << std::endl;
    return;
  }
  rc->set_StringFlag("CDB_GLOBALTAG","ProdA_2024");
  rc->set_uint64Flag("TIMESTAMP",runnumber);
  CDBInterface::instance()->Verbosity(0); 

  Fun4AllInputManager *in0 = new Fun4AllDstInputManager("DSTJet");
  in0->AddListFile(filedstlist0, 1);
  se->registerInputManager(in0);

  Fun4AllInputManager *in1 = new Fun4AllDstInputManager("DSTJetCalo");
  in1->AddListFile(filedstlist1, 1);
  se->registerInputManager(in1);

  Process_Calo_Calib();

  caloTreeGen *_caloTreeGen = new caloTreeGen("caloTreeGen",outputFile);
  _caloTreeGen->SetVerbosity(verbosity);
  se->registerSubsystem(_caloTreeGen);
  
  se->run(n_event);
  se->End();
  se->PrintTimer();
  gSystem->Exit(0);
}
#endif
