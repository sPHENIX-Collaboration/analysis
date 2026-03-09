#ifndef MACRO_FUN4ALLMYSIMPLERECO_C
#define MACRO_FUN4ALLMYSIMPLERECO_C

#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include <waveform/WaveFormPacket.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libWaveForm.so)

void Fun4All_WaveFormPacket(const int nEvents = 3, const std::string &infile = "/sphenix/lustre01/sphnxpro/physics/slurp/calophysics/run_00047900_00048000/DST_TRIGGERED_EVENT_run2pp_new_2024p003-00047997-0000.root")
{
  ///////////////////////////////////////////
  // Make the Server
  //////////////////////////////////////////
  gSystem->Load("libg4dst.so");
  Fun4AllServer *se = Fun4AllServer::instance();
  WaveFormPacket *myreco = new WaveFormPacket();
  se->registerSubsystem(myreco);
// this (dummy) input manager just drives the event loop
  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTin");
  in->AddFile(infile);
  in->Verbosity(1);
  se->registerInputManager( in );
// events = 0 => run till end of input file
  if (nEvents <= 0)
  {
    return;
  }
  cout << endl << "Running over " << nEvents << " Events" << endl;
  se->run(nEvents);
  cout << endl << "Calling End in Fun4All_WaveFormPacket.C" << endl;
  se->End();
  cout << endl << "All done, calling delete Fun4AllServer" << endl;
  delete se;
  cout << endl << "gSystem->Exit(0)" << endl;
  gSystem->Exit(0);
}

#endif // MACRO_FUN4ALLMYSIMPLERECO_C
