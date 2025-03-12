// c++ includes --
#include <iostream>
#include <string>

// root includes --
#include <TROOT.h>
#include <TSystem.h>

// fun4all includes --
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllUtils.h>
#include <fun4all/SubsysReco.h>

// jet background cut includes --
#include <jetbackground/JetBackgroundCut.h>

// jet includes --
#include <jetbackground/RetowerCEMC.h>
#include <jetbackground/FastJetAlgoSub.h>
#include <jetbase/JetReco.h>
#include <jetbase/TowerJetInput.h>

#include <phool/recoConsts.h>

// My Analysis includes --
#include <jetvalidation/JetValidationv2.h>
#include <jetvalidation/EventCheck.h>

using std::cout;
using std::endl;
using std::istringstream;
using std::pair;
using std::string;

#include "Calo_Calib.C"

R__LOAD_LIBRARY(libJetValidation.so)

void Fun4All_JetValv2(const string &input_CALO,
                      const string &input_GLOBAL,
                      const string &outputFile = "test.root",
                      UInt_t nEvents = 0)
{
  cout << "#############################" << endl;
  cout << "Run Parameters" << endl;
  cout << "input file: " << input_CALO << endl;
  cout << "input file: " << input_GLOBAL << endl;
  cout << "output file: " << outputFile << endl;
  cout << "Events: " << nEvents << endl;
  cout << "#############################" << endl;

  Fun4AllServer *se = Fun4AllServer::instance();

  recoConsts *rc = recoConsts::instance();

  rc->set_StringFlag("CDB_GLOBALTAG", "MDC2");

  pair<Int_t, Int_t> runseg = Fun4AllUtils::GetRunSegment(input_CALO);
  Int_t runnumber = runseg.first;
  rc->set_uint64Flag("TIMESTAMP", runnumber);

  // //! Quiet mode. Only output critical messages. Intended for batch production mode.
  // VERBOSITY_QUIET = 0,

  // //! Output some useful messages during manual command line running
  // VERBOSITY_SOME = 1,

  // //! Output more messages
  // VERBOSITY_MORE = 2,

  // //! Output even more messages
  // VERBOSITY_EVEN_MORE = 3,

  // //! Output a lot of messages
  // VERBOSITY_A_LOT = 4,

  Fun4AllInputManager *in = new Fun4AllDstInputManager("DST_CALO");
  in->AddFile(input_CALO.c_str());
  se->registerInputManager(in);

  Fun4AllInputManager *in2 = new Fun4AllDstInputManager("DST_GLOBAL");
  in2->AddFile(input_GLOBAL.c_str());
  se->registerInputManager(in2);

  EventCheck *myEventCheck = new EventCheck();
  myEventCheck->set_zvtx_max(30); /*cm*/
  se->registerSubsystem(myEventCheck);

  Process_Calo_Calib();

  string tower_prefix = "TOWERINFO_CALIB";
  RetowerCEMC *rcemc = new RetowerCEMC();
  rcemc->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  rcemc->set_towerinfo(true);
  rcemc->set_frac_cut(0.5); //fraction of retower that must be masked to mask the full retower
  rcemc->set_towerNodePrefix(tower_prefix);
  se->registerSubsystem(rcemc);

  // do unsubtracted jet reconstruction
  string jetreco_input_prefix = tower_prefix;
  JetReco* _jetRecoUnsub = new JetReco();
  _jetRecoUnsub->add_input(new TowerJetInput(Jet::CEMC_TOWERINFO_RETOWER, jetreco_input_prefix));
  _jetRecoUnsub->add_input(new TowerJetInput(Jet::HCALIN_TOWERINFO, jetreco_input_prefix));
  _jetRecoUnsub->add_input(new TowerJetInput(Jet::HCALOUT_TOWERINFO, jetreco_input_prefix));
  _jetRecoUnsub->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.4), "AntiKt_unsubtracted_r04");
  _jetRecoUnsub->set_algo_node("ANTIKT");
  _jetRecoUnsub->set_input_node("TOWER");
  se->registerSubsystem(_jetRecoUnsub);

  JetBackgroundCut *jocl = new JetBackgroundCut("AntiKt_unsubtracted_r04","JOCL", 0, false);
  se->registerSubsystem(jocl);

  JetValidationv2 *myJetVal = new JetValidationv2();
  myJetVal->set_outputFile(outputFile);
  myJetVal->set_xj_cut(0.15);
  myJetVal->set_isSim(true);
  myJetVal->set_pt_background(60); /*GeV*/
  se->registerSubsystem(myJetVal);

  se->run(nEvents);
  se->End();
  se->PrintTimer();
  cout << "All done!" << endl;

  gSystem->Exit(0);
  std::quick_exit(0);
}

#ifndef __CINT__
int main(int argc, char *argv[])
{
  if (argc < 3 || argc > 5)
  {
    cout << "usage: ./bin/Fun4All_JetValv2 input_JET input_JETCALO [outputFile] [events]" << endl;
    cout << "input_JET: Location of fileList containing dst JET." << endl;
    cout << "input_JETCALO: Location of fileList containing dst JETCALO." << endl;
    cout << "outputFile: name of output QA file. Default: test.root" << endl;
    cout << "events: Number of events to analyze. Default: all" << endl;
    return 1;
  }

  string outputFile = "test.root";
  UInt_t events = 0;

  if (argc >= 4)
  {
    outputFile = argv[3];
  }
  if (argc >= 5)
  {
    events = atoi(argv[4]);
  }

  Fun4All_JetValv2(argv[1], argv[2], outputFile, events);

  cout << "done" << endl;
  return 0;
}
#endif
