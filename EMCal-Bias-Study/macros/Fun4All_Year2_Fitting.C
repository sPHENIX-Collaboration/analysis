// c++ includes --
#include <iostream>
#include <string>

// root includes --
#include <TROOT.h>
#include <TSystem.h>

#include "Calo_Fitting.C"

#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>

#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllUtils.h>
#include <fun4all/SubsysReco.h>

#include <fun4allraw/Fun4AllPrdfInputManager.h>

#include <phool/recoConsts.h>

#include <calotower/CaloTower.h>

R__LOAD_LIBRARY(libCaloTower.so)

using std::cout;
using std::endl;
using std::pair;
using std::string;

void Fun4All_Year2_Fitting(const string &fname,
                           const string &outfile = "test.root",
                           int nEvents = 0,
                           bool doAllWaveforms = true,
                           const string &dbtag = "ProdA_2024")
{
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);

  recoConsts *rc = recoConsts::instance();

  pair<int, int> runseg = Fun4AllUtils::GetRunSegment(fname);
  int runnumber = runseg.first;

  // conditions DB flags and timestamp
  rc->set_StringFlag("CDB_GLOBALTAG", dbtag);
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

  CDBInterface::instance()->Verbosity(1);

  FlagHandler *flag = new FlagHandler();
  se->registerSubsystem(flag);

  Process_Calo_Fitting();

  CaloTower *calo = new CaloTower();
  calo->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  calo->setOutputFile(outfile);
  calo->setDoAllWaveforms(doAllWaveforms);
  calo->add_nphi_neta_low(128,0); // S48 IB5
  calo->add_nphi_neta_low(136,0); // S49 IB5
  calo->add_nphi_neta_low(152,0); // S51 IB5
  se->registerSubsystem(calo);

  Fun4AllInputManager *In = new Fun4AllPrdfInputManager("in");
  In->AddFile(fname);
  se->registerInputManager(In);

  se->run(nEvents);
  se->End();

  CDBInterface::instance()->Print();  // print used DB files
  se->PrintTimer();
  delete se;
  cout << "All done!" << endl;
  std::quick_exit(0);
  gSystem->Exit(0);
}

#ifndef __CINT__
int main(int argc, char *argv[])
{
  if (argc < 2 || argc > 6)
  {
    cout << "usage: ./bin/Fun4All_Year2_Fitting input [outFile] [events] [doAllWaveforms] [dbtag]" << endl;
    cout << "input: input PRDF." << endl;
    cout << "outFile: name of output QA file. Default: test.root" << endl;
    cout << "events: Number of events to analyze. Default: all" << endl;
    cout << "doAllWaveforms: Analyze all waveforms. Default: true" << endl;
    cout << "dbtag: Database Tag. Default: ProdA_2024" << endl;
    return 1;
  }

  string outFile = "test.root";
  UInt_t events = 0;
  Bool_t doAllWaveforms = true;
  string dbtag = "ProdA_2024";

  if (argc >= 3)
  {
    outFile = argv[2];
  }
  if (argc >= 4)
  {
    events = atoi(argv[3]);
  }
  if (argc >= 5)
  {
    doAllWaveforms = atoi(argv[4]);
  }
  if (argc >= 6)
  {
    dbtag = argv[5];
  }

  Fun4All_Year2_Fitting(argv[1], outFile, events, doAllWaveforms, dbtag);

  cout << "done" << endl;
  return 0;
}
#endif
