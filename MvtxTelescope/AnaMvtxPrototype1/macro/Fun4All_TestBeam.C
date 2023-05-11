#include <string>

using namespace std;

void Fun4All_TestBeam(int nEvents = 250000,
                      const char *input_file = "./beamtest2018/beam/beam_00000088-0000.prdf",
                      const char *output_file = "beam_00000088.root"
                     )
{
  gSystem->Load("libfun4all");
  // gSystem->Load("libPrototype4.so");
  gSystem->Load("libMvtxPrototype1.so");
  gSystem->Load("libAnaMvtxPrototype1.so");

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);
  // se->Verbosity(Fun4AllServer::VERBOSITY_SOME);

  char ifile[strlen(input_file) + 1]; // + 1 for the \0 which marks the end of string
  strcpy(ifile, input_file);
  strtok(ifile, "_");
  int runnumber = atoi(strtok(0, "-"));
  int segnumber = atoi(strtok(strtok(0, "-"), "."));
  cout << "runnumber: " << runnumber << " segment " << segnumber << endl;

  recoConsts *rc = recoConsts::instance();
  rc->set_IntFlag("RUNNUMBER", runnumber);

  MvtxUnpackPRDF *unpack = new MvtxUnpackPRDF();
  unpack->Verbosity(0);
  se->registerSubsystem(unpack);

  MvtxApplyHotDead *hotdead = new MvtxApplyHotDead("MvtxApplyHotDead",
      "hotmap/hotmap_testbeam_00000088.txt");
  hotdead->Verbosity(0);
  hotdead->PrintHotDeadMap();
  se->registerSubsystem(hotdead);

  MvtxClusterizer *clus = new MvtxClusterizer();
  clus->Verbosity(0);
  se->registerSubsystem(clus);

  MvtxDefUtil util;
  MvtxAlign *align = new MvtxAlign();
  align->Verbosity(0);
  // align->AddAlignmentPar(util.GenHitSetKey(0, 0, 0), +38, 0, +4);
  // align->AddAlignmentPar(util.GenHitSetKey(1, 0, 1), +26, 2.2 / 28e-4, +8);
  // align->AddAlignmentPar(util.GenHitSetKey(2, 0, 2), +12, 4.4 / 28e-4, +13);
  // align->AddAlignmentPar(util.GenHitSetKey(3, 0, 3), 0, 6.6 / 28e-4, 0);
  // align->PrintAlignmentPars();
  align->SetAlignmentParFileDir("beamcenter/");
  se->registerSubsystem(align);

  AnaMvtxPrototype1* ana = new AnaMvtxPrototype1();
  // ana->Verbosity(Fun4AllServer::VERBOSITY_SOME);
  ana->Verbosity(0);
  se->registerSubsystem(ana);


  Fun4AllDstOutputManager *out_Manager = new Fun4AllDstOutputManager("DSTOUT", output_file);
  se->registerOutputManager(out_Manager);

  Fun4AllInputManager *in = new Fun4AllPrdfInputManager("PRDFin");
  in->fileopen(input_file);
  se->registerInputManager(in);

  se->run(nEvents);

  se->End();
}
